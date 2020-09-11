cmake_minimum_required(VERSION 2.8)
project(zireael)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O0 -ggdb -std=c++11 -pthread -Wall -Wno-deprecated -Werror -Wno-unused-function")

set(LIB_SRC
    zireael/Coroutine/Context.cc
    zireael/Coroutine/Coroutine.cc
    zireael/Coroutine/Epoll.cc
    zireael/Coroutine/mstime.cc
    zireael/Coroutine/Timer.cc
    zireael/Coroutine/Processor.cc
    zireael/Coroutine/ProcessorSelect.cc
    zireael/Coroutine/Scheduler.cc
    zireael/Coroutine/Co_api.cc
    zireael/Coroutine/Socket.cc
    zireael/Coroutine/Mutex.cc
    )

add_library(zireael SHARED ${LIB_SRC})

add_executable(test tests/test.cc)
add_dependencies(test zireael)
target_link_libraries(test zireael)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)