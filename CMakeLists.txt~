cmake_minimum_required(VERSION 2.8)
find_package(OpenCV)
INCLUDE_DIRECTORIES(${OPENCV_INCLUDE_DIR})
INCLUDE_DIRECTORIES(.)
link_directories(.)

 find_package (Threads)
 link_directories(../bin)

 add_executable(bowPredict sample.cpp pthread_pool.cpp)
 SET(EXECUTABLE_OUTPUT_PATH ../bin)

 if (WIN32)
 TARGET_LINK_LIBRARIES(bowPredict bowsdk pthreadVC2)
 else (WIN32)
 target_link_libraries (bowPredict bowsdk ${CMAKE_THREAD_LIBS_INIT})
 endif(WIN32)



TARGET_LINK_LIBRARIES(bowPredict ${CMAKE_THREAD_LIBS_INIT})
TARGET_LINK_LIBRARIES(bowPredict libbowsdk.so)
IF(OpenCV_FOUND)
  TARGET_LINK_LIBRARIES(bowPredict ${OpenCV_LIBS}  )
ENDIF()



