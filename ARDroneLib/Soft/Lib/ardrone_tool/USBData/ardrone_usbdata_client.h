#ifndef _ARDRONE_USBDATA_CLIENT_H_
#define _ARDRONE_USBDATA_CLIENT_H_

#include <VP_Os/vp_os_types.h>
#include <VP_Api/vp_api_thread_helper.h>

#include <ardrone_api.h>
//#include <ardrone_tool/Control/ardrone_usbdata_control.h>
//#include <ardrone_tool/Navdata/ardrone_usbdata_file.h>
//#include <ardrone_tool/Navdata/ardrone_general_usbdata.h>
#include <config.h>

#define USBDATA_MAX_SIZE 128

// Facility to declare a set of usbdata handler
// Handler to resume control thread is mandatory
#define BEGIN_USBDATA_HANDLER_TABLE                                 \
    ardrone_usbdata_handler_t ardrone_usbdata_handler_table[] = { 

#define END_USBDATA_HANDLER_TABLE                                       \
  { NULL, NULL, NULL, NULL }                                            \
  };

#define USBDATA_HANDLER_TABLE_ENTRY( init, process, release, init_data_ptr ) \
    { (ardrone_usbdata_handler_init_t)init, process, release, init_data_ptr },

typedef C_RESULT (*ardrone_usbdata_handler_init_t)( void* data );
typedef C_RESULT (*ardrone_usbdata_handler_process_t)( const uint8_t* const usbdata );
typedef C_RESULT (*ardrone_usbdata_handler_release_t)( void );

typedef struct _ardrone_usbdata_handler_t {
    ardrone_usbdata_handler_init_t    init;
    ardrone_usbdata_handler_process_t process;
    ardrone_usbdata_handler_release_t release;

    void*                             data; // Data used during initialization
} ardrone_usbdata_handler_t;


extern ardrone_usbdata_handler_t ardrone_usbdata_handler_table[] WEAK;

uint32_t ardrone_usbdata_client_get_num_retries(void);
C_RESULT ardrone_usbdata_client_init(void);
C_RESULT ardrone_usbdata_client_suspend(void);
C_RESULT ardrone_usbdata_client_resume(void);
C_RESULT ardrone_usbdata_client_shutdown(void);
C_RESULT ardrone_usbdata_open_server(void);

PROTO_THREAD_ROUTINE( usbdata_update , nomParams );

#endif // _ARDRONE_USBDATA_H_
