CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(libtaskprocpp)

SET(libtaskprocpp_VERSION_MAJOR 0)
SET(libtaskprocpp_VERSION_MINOR 1)

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

FIND_PACKAGE(Threads)

INCLUDE_DIRECTORIES(./include)
file(GLOB_RECURSE SRCS src/*.cpp)
SET(libtaskprocpp_SRCS ${SRCS})

add_definitions(-DUSE_NANO_SECOND_SLEEP)
add_definitions(-DFORCE_SLEEP_COMPLETED)

ADD_LIBRARY(taskprocpp ${libtaskprocpp_SRCS})

TARGET_LINK_LIBRARIES(taskprocpp ${CMAKE_THREAD_LIBS_INIT} stl)
