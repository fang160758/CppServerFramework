cmake_minimum_required(VERSION 3.0)
project(fangsev)


set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O3 -fPIC -ggdb -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined -Wno-deprecated-declarations")
set(CMAKE_C_FLAGS "$ENV{CXXFLAGS} -rdynamic -O3 -fPIC -ggdb -std=c11 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined -Wno-deprecated-declarations")


include_directories(./inc)
include_directories(/usr)

link_directories(./lib)
link_directories(/usr/lib)
link_directories(/usr/lib64)


set(LIB_SRC
        src/hook.cc
        src/fd_manager.cc
        src/mutex.cc
        src/timer.cc
        src/address.cc
        src/socket.cc
        src/bytearray.cc
        src/log.cc
        src/helpc.cc
        src/fiber.cc
        src/thread.cc
        src/scheduler.cc
        src/iomanager.cc
        src/stream.cc
        http/http.cc
        http/http11_parser.cc
        http/httpclient_parser.cc
        http/http_parser.cc
        http/http_session.cc
        http/servlet.cc
        stream/socket_stream.cc
		)


add_library(fangsev SHARED ${LIB_SRC})

set(LIBS
        fangsev
        dl
        pthread
		)
		

#add_executable(hook_test tests/hook_test.cc)
#add_dependencies(hook_test fangsev)
#target_link_libraries(hook_test ${LIBS})

#add_executable(bytearray_test tests/test_bytearray.cc)
#add_dependencies(bytearray_test fangsev)
#target_link_libraries(bytearray_test ${LIBS})
#
#add_executable(http_test tests/http_test.cc)
#add_dependencies(http_test fangsev)
#target_link_libraries(http_test ${LIBS})
#
#add_executable(http_parser_test tests/http_parser_test.cc)
#add_dependencies(http_parser_test fangsev)
#target_link_libraries(http_parser_test ${LIBS})
#
add_executable(test_scheduler tests/test_scheduler.cc)
add_dependencies(test_scheduler fangsev)
target_link_libraries(test_scheduler ${LIBS})

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

