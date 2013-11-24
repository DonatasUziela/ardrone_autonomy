# Catkin User Guide: http://www.ros.org/doc/groovy/api/catkin/html/user_guide/user_guide.html
# Catkin CMake Standard: http://www.ros.org/doc/groovy/api/catkin/html/user_guide/standards.html
cmake_minimum_required(VERSION 2.8.3)
project(ardrone_autonomy)
# Load catkin and all dependencies required for this package
# TODO: remove all from COMPONENTS that are not catkin packages.
find_package(catkin REQUIRED COMPONENTS roscpp image_transport sensor_msgs std_srvs tf camera_info_manager message_generation)

# include_directories(include ${Boost_INCLUDE_DIR} ${catkin_INCLUDE_DIRS})
# CATKIN_MIGRATION: removed during catkin migration
# cmake_minimum_required(VERSION 2.4.6)

# CATKIN_MIGRATION: removed during catkin migration
# include($ENV{ROS_ROOT}/core/rosbuild/rosbuild.cmake)

# Set the build type.  Options are:
#  Coverage       : w/ debug symbols, w/o optimization, w/ code-coverage
#  Debug          : w/ debug symbols, w/o optimization
#  Release        : w/o debug symbols, w/ optimization
#  RelWithDebInfo : w/ debug symbols, w/ optimization
#  MinSizeRel     : w/o debug symbols, w/ optimization, stripped binaries
#set(ROS_BUILD_TYPE Release)


# CATKIN_MIGRATION: removed during catkin migration
# rosbuild_init()

#set the default path for built executables to the "bin" directory
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
#set the default path for built libraries to the "lib" directory
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

#uncomment if you have defined messages
add_message_files(
  FILES
  matrix33.msg                 navdata_pressure_raw.msg   navdata_vision_perf.msg
  navdata_adc_data_frame.msg   navdata_pwm.msg            navdata_vision_raw.msg
  navdata_altitude.msg         navdata_raw_measures.msg   navdata_watchdog.msg
  navdata_demo.msg             navdata_rc_references.msg  navdata_wifi.msg
  navdata_euler_angles.msg     navdata_references.msg     navdata_wind_speed.msg
  navdata_games.msg            navdata_time.msg           navdata_zimmu_3000.msg
  navdata_gyros_offsets.msg    navdata_trackers_send.msg  Ranges.msg
  navdata_hdvideo_stream.msg   navdata_trims.msg          
  navdata_kalman_pressure.msg  navdata_video_stream.msg   vector21.msg
  navdata_magneto.msg          navdata_vision_detect.msg  vector31.msg
  Navdata.msg                  navdata_vision.msg
  navdata_phys_measures.msg    navdata_vision_of.msg

)
#uncomment if you have defined services
add_service_files(
  FILES
  CamSelect.srv  FlightAnim.srv  LedAnim.srv
)

#common commands for building c++ executables and libraries
#add_library(${PROJECT_NAME} src/example.cpp)
#target_link_libraries(${PROJECT_NAME} another_library)
#
# CATKIN_MIGRATION: removed during catkin migration
# rosbuild_add_boost_directories()
#find_package(Boost REQUIRED COMPONENTS thread)
include_directories(${Boost_INCLUDE_DIRS})
#target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
#target_link_libraries(example ${PROJECT_NAME})


# CATKIN_MIGRATION: removed during catkin migration
# rosbuild_add_boost_directories()
set(SDK ARDroneLib/)
link_directories(${PROJECT_SOURCE_DIR}/lib/)
include_directories(${SDK} ${SDK}/FFMPEG/Includes ${SDK}/Soft/Common ${SDK}/Soft/Lib ${SDK}/VP_SDK ${SDK}/VP_SDK/VP_Os/linux )
add_executable(ardrone_driver src/ardrone_driver.cpp src/video.cpp src/ardrone_sdk.cpp src/teleop_twist.cpp)
#target_link_libraries(ardrone_driver vlib pc_ardrone sdk SDL avcodec avdevice avfilter avformat avutil swscale)
target_link_libraries(ardrone_driver pc_ardrone avcodec avutil swscale vlib  sdk SDL ${Boost_LIBRARIES} ${catkin_LIBRARIES})
## Generate added messages and services with any dependencies listed here
generate_messages(
    DEPENDENCIES sensor_msgs std_msgs
)

# catkin_package parameters: http://ros.org/doc/groovy/api/catkin/html/dev_guide/generated_cmake_api.html#catkin-package
# TODO: fill in what other packages will need to use this package
catkin_package(
    DEPENDS roscpp image_transport sensor_msgs std_srvs tf camera_info_manager libsdl-dev
    CATKIN_DEPENDS message_runtime
    INCLUDE_DIRS # TODO include
    LIBRARIES # TODO
)