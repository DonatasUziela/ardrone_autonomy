#include <errno.h>
#include <string.h>

#include <config.h>

#include <VP_Os/vp_os_print.h>
#include <VP_Com/vp_com.h>

#include <ardrone_api.h>
#include <ardrone_tool/ardrone_tool.h>
#include <ardrone_tool/USBData/ardrone_usbdata_client.h>
#include <ardrone_tool/Com/config_com.h>

#ifndef _WIN32
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>
#endif



static bool_t usbdata_thread_in_pause = TRUE;
static bool_t bContinue = TRUE;
static vp_os_cond_t usbdata_client_condition;
static vp_os_mutex_t usbdata_client_mutex;

static vp_com_socket_t usbdata_socket;
static Read usbdata_read      = NULL;
static Write usbdata_write    = NULL;

uint8_t usbdata_buffer[USBDATA_MAX_SIZE];

C_RESULT ardrone_usbdata_client_init(void)
{
  C_RESULT res;

  PRINT("ardrone_usbdata_client_init\n");
  
  wifi_config_socket(&usbdata_socket, VP_COM_CLIENT, USBDATA_PORT, wifi_ardrone_ip);
  usbdata_socket.protocol = VP_COM_UDP;
  usbdata_socket.is_multicast = 0;      // disable multicast for Navdata
  usbdata_socket.multicast_base_addr = MULTICAST_BASE_ADDR;

  vp_os_mutex_init(&usbdata_client_mutex);
  vp_os_cond_init(&usbdata_client_condition, &usbdata_client_mutex);
	
  res = C_OK;

  return res;
}

C_RESULT ardrone_usbdata_client_suspend(void)
{
	vp_os_mutex_lock(&usbdata_client_mutex);
	usbdata_thread_in_pause = TRUE;
	vp_os_mutex_unlock(&usbdata_client_mutex);	
	
	return C_OK;
}

C_RESULT ardrone_usbdata_client_resume(void)
{
	vp_os_mutex_lock(&usbdata_client_mutex);
	vp_os_cond_signal(&usbdata_client_condition);
	usbdata_thread_in_pause = FALSE;
	vp_os_mutex_unlock(&usbdata_client_mutex);	

	return C_OK;
}

C_RESULT ardrone_usbdata_open_server(void)
{
  // Flag value :
  // 1 -> Unicast
  // 2 -> Multicast
  int32_t flag = 1, len = sizeof(flag);

  if( usbdata_write != NULL )
  {
    if (usbdata_socket.is_multicast == 1)
      flag = 2;

    usbdata_write(&usbdata_socket, (const uint8_t*) &flag, &len);
  }

  return C_OK;
}

DEFINE_THREAD_ROUTINE( usbdata_update, nomParams )
{
  C_RESULT res;
  int32_t  i, size;
  struct timeval tv;
#ifdef _WIN32
  int timeout_for_windows=1000/*milliseconds*/;
#endif


  tv.tv_sec   = 1/*second*/;
  tv.tv_usec  = 0;

  res = C_OK;

  if( VP_FAILED(vp_com_open(wifi_com(), &usbdata_socket, &usbdata_read, &usbdata_write)) )
  {
    printf("VP_Com : Failed to open socket for usbdata\n");
    res = C_FAIL;
  }

  if( VP_SUCCEEDED(res) )
  {
    PRINT("Thread usbdata_update in progress...\n");

#ifdef _WIN32
	setsockopt((int32_t)usbdata_socket.priv, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout_for_windows, sizeof(timeout_for_windows));
#else
	setsockopt((int32_t)usbdata_socket.priv, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
#endif


    i = 0;
    while( ardrone_usbdata_handler_table[i].init != NULL )
    {
      // if init failed for an handler we set its process function to null
      // We keep its release function for cleanup

      if( VP_FAILED( ardrone_usbdata_handler_table[i].init(ardrone_usbdata_handler_table[i].data) ) ){
        ardrone_usbdata_handler_table[i].process = NULL;
      }
      i ++;
    }

   usbdata_thread_in_pause = FALSE;
  
    while( VP_SUCCEEDED(res) 
           && !ardrone_tool_exit() 
           && bContinue )
    {
	  if(usbdata_thread_in_pause)
	  {
		vp_os_mutex_lock(&usbdata_client_mutex);
		vp_os_cond_wait(&usbdata_client_condition);
		vp_os_mutex_unlock(&usbdata_client_mutex);
	  }
          
      if( usbdata_read == NULL )
      {
	res = C_FAIL;
        continue;
      }

      size = USBDATA_MAX_SIZE;
      res = usbdata_read( (void*)&usbdata_socket, &usbdata_buffer[0], &size );
    
      if( VP_SUCCEEDED( res ) )
      {
	i = 0;
	while( ardrone_usbdata_handler_table[i].init != NULL )
	{

	  if( ardrone_usbdata_handler_table[i].process != NULL ){
	    
	    ardrone_usbdata_handler_table[i].process( &usbdata_buffer[0] );
	  }
	  
	  i++;
	}
      }
    }

    // Release resources alllocated by handlers
    i = 0;
    while( ardrone_usbdata_handler_table[i].init != NULL )
    {
      ardrone_usbdata_handler_table[i].release();

      i ++;
    }
  }

  vp_com_close(wifi_com(), &usbdata_socket);

  DEBUG_PRINT_SDK("Thread usbdata_update ended\n");

  return (THREAD_RET)res;
}


C_RESULT ardrone_usbdata_client_shutdown(void)
{
   bContinue = FALSE;

   vp_os_mutex_lock(&usbdata_client_mutex);
   vp_os_cond_signal(&usbdata_client_condition);
   vp_os_mutex_unlock(&usbdata_client_mutex);
   
   return C_OK;
}


