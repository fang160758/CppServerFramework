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

function(ragelmaker src_rl outputlist outputdir)
    #Create a custom build step that will call ragel on the provided src_rl file.
    #The output .cpp file will be appended to the variable name passed in outputlist.

    get_filename_component(src_file ${src_rl} NAME_WE)

    set(rl_out ${outputdir}/${src_file}.rl.cc)

    #adding to the list inside a function takes special care, we cannot use list(APPEND...)
    #because the results are local scope only
    set(${outputlist} ${${outputlist}} ${rl_out} PARENT_SCOPE)

    #Warning: The " -S -M -l -C -T0  --error-format=msvc" are added to match existing window invocation
    #we might want something different for mac and linux
    add_custom_command(
        OUTPUT ${rl_out}
        COMMAND cd ${outputdir}
        COMMAND ragel ${CMAKE_CURRENT_SOURCE_DIR}/${src_rl} -o ${rl_out} -l -C -G2  --error-format=msvc
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${src_rl}
        )
    set_source_files_properties(${rl_out} PROPERTIES GENERATED TRUE)
endfunction(ragelmaker)

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
        src/config.cc
        http/http.cc
        http/http_parser.cc
        http/http_session.cc
        http/http_connection.cc
        http/servlet.cc
        stream/socket_stream.cc
		)


ragelmaker(http/http11_parser.rl LIB_SRC ${CMAKE_CURRENT_SOURCE_DIR}/http)
ragelmaker(http/httpclient_parser.rl LIB_SRC ${CMAKE_CURRENT_SOURCE_DIR}/http)
ragelmaker(inc/uri.rl LIB_SRC ${CMAKE_CURRENT_SOURCE_DIR}/inc)

add_library(fangsev SHARED ${LIB_SRC})

set(LIBS
        fangsev
        dl
        pthread
        yaml-cpp
		)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

