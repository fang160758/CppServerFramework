# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Produce verbose output by default.
VERBOSE = 1

# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/fang/workspace/fangser

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/fang/workspace/fangser/build

# Include any dependencies generated for this target.
include CMakeFiles/fangsev.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/fangsev.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/fangsev.dir/flags.make

../http/http11_parser.rl.cc: ../http/http11_parser.rl
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/fang/workspace/fangser/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Generating ../http/http11_parser.rl.cc"
	cd /home/fang/workspace/fangser/http
	ragel /home/fang/workspace/fangser/http/http11_parser.rl -o /home/fang/workspace/fangser/http/http11_parser.rl.cc -l -C -G2 --error-format=msvc

../http/httpclient_parser.rl.cc: ../http/httpclient_parser.rl
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/fang/workspace/fangser/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Generating ../http/httpclient_parser.rl.cc"
	cd /home/fang/workspace/fangser/http
	ragel /home/fang/workspace/fangser/http/httpclient_parser.rl -o /home/fang/workspace/fangser/http/httpclient_parser.rl.cc -l -C -G2 --error-format=msvc

../inc/uri.rl.cc: ../inc/uri.rl
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/fang/workspace/fangser/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Generating ../inc/uri.rl.cc"
	cd /home/fang/workspace/fangser/inc
	ragel /home/fang/workspace/fangser/inc/uri.rl -o /home/fang/workspace/fangser/inc/uri.rl.cc -l -C -G2 --error-format=msvc

CMakeFiles/fangsev.dir/src/hook.cc.o: CMakeFiles/fangsev.dir/flags.make
CMakeFiles/fangsev.dir/src/hook.cc.o: ../src/hook.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/fang/workspace/fangser/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/fangsev.dir/src/hook.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/fangsev.dir/src/hook.cc.o -c /home/fang/workspace/fangser/src/hook.cc

CMakeFiles/fangsev.dir/src/hook.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/fangsev.dir/src/hook.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/fang/workspace/fangser/src/hook.cc > CMakeFiles/fangsev.dir/src/hook.cc.i

CMakeFiles/fangsev.dir/src/hook.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/fangsev.dir/src/hook.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/fang/workspace/fangser/src/hook.cc -o CMakeFiles/fangsev.dir/src/hook.cc.s

CMakeFiles/fangsev.dir/src/fd_manager.cc.o: CMakeFiles/fangsev.dir/flags.make
CMakeFiles/fangsev.dir/src/fd_manager.cc.o: ../src/fd_manager.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/fang/workspace/fangser/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/fangsev.dir/src/fd_manager.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/fangsev.dir/src/fd_manager.cc.o -c /home/fang/workspace/fangser/src/fd_manager.cc

CMakeFiles/fangsev.dir/src/fd_manager.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/fangsev.dir/src/fd_manager.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/fang/workspace/fangser/src/fd_manager.cc > CMakeFiles/fangsev.dir/src/fd_manager.cc.i

CMakeFiles/fangsev.dir/src/fd_manager.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/fangsev.dir/src/fd_manager.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/fang/workspace/fangser/src/fd_manager.cc -o CMakeFiles/fangsev.dir/src/fd_manager.cc.s

CMakeFiles/fangsev.dir/src/mutex.cc.o: CMakeFiles/fangsev.dir/flags.make
CMakeFiles/fangsev.dir/src/mutex.cc.o: ../src/mutex.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/fang/workspace/fangser/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object CMakeFiles/fangsev.dir/src/mutex.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/fangsev.dir/src/mutex.cc.o -c /home/fang/workspace/fangser/src/mutex.cc

CMakeFiles/fangsev.dir/src/mutex.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/fangsev.dir/src/mutex.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/fang/workspace/fangser/src/mutex.cc > CMakeFiles/fangsev.dir/src/mutex.cc.i

CMakeFiles/fangsev.dir/src/mutex.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/fangsev.dir/src/mutex.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/fang/workspace/fangser/src/mutex.cc -o CMakeFiles/fangsev.dir/src/mutex.cc.s

CMakeFiles/fangsev.dir/src/timer.cc.o: CMakeFiles/fangsev.dir/flags.make
CMakeFiles/fangsev.dir/src/timer.cc.o: ../src/timer.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/fang/workspace/fangser/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object CMakeFiles/fangsev.dir/src/timer.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/fangsev.dir/src/timer.cc.o -c /home/fang/workspace/fangser/src/timer.cc

CMakeFiles/fangsev.dir/src/timer.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/fangsev.dir/src/timer.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/fang/workspace/fangser/src/timer.cc > CMakeFiles/fangsev.dir/src/timer.cc.i

CMakeFiles/fangsev.dir/src/timer.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/fangsev.dir/src/timer.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/fang/workspace/fangser/src/timer.cc -o CMakeFiles/fangsev.dir/src/timer.cc.s

CMakeFiles/fangsev.dir/src/address.cc.o: CMakeFiles/fangsev.dir/flags.make
CMakeFiles/fangsev.dir/src/address.cc.o: ../src/address.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/fang/workspace/fangser/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building CXX object CMakeFiles/fangsev.dir/src/address.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/fangsev.dir/src/address.cc.o -c /home/fang/workspace/fangser/src/address.cc

CMakeFiles/fangsev.dir/src/address.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/fangsev.dir/src/address.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/fang/workspace/fangser/src/address.cc > CMakeFiles/fangsev.dir/src/address.cc.i

CMakeFiles/fangsev.dir/src/address.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/fangsev.dir/src/address.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/fang/workspace/fangser/src/address.cc -o CMakeFiles/fangsev.dir/src/address.cc.s

CMakeFiles/fangsev.dir/src/socket.cc.o: CMakeFiles/fangsev.dir/flags.make
CMakeFiles/fangsev.dir/src/socket.cc.o: ../src/socket.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/fang/workspace/fangser/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building CXX object CMakeFiles/fangsev.dir/src/socket.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/fangsev.dir/src/socket.cc.o -c /home/fang/workspace/fangser/src/socket.cc

CMakeFiles/fangsev.dir/src/socket.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/fangsev.dir/src/socket.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/fang/workspace/fangser/src/socket.cc > CMakeFiles/fangsev.dir/src/socket.cc.i

CMakeFiles/fangsev.dir/src/socket.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/fangsev.dir/src/socket.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/fang/workspace/fangser/src/socket.cc -o CMakeFiles/fangsev.dir/src/socket.cc.s

CMakeFiles/fangsev.dir/src/bytearray.cc.o: CMakeFiles/fangsev.dir/flags.make
CMakeFiles/fangsev.dir/src/bytearray.cc.o: ../src/bytearray.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/fang/workspace/fangser/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Building CXX object CMakeFiles/fangsev.dir/src/bytearray.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/fangsev.dir/src/bytearray.cc.o -c /home/fang/workspace/fangser/src/bytearray.cc

CMakeFiles/fangsev.dir/src/bytearray.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/fangsev.dir/src/bytearray.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/fang/workspace/fangser/src/bytearray.cc > CMakeFiles/fangsev.dir/src/bytearray.cc.i

CMakeFiles/fangsev.dir/src/bytearray.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/fangsev.dir/src/bytearray.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/fang/workspace/fangser/src/bytearray.cc -o CMakeFiles/fangsev.dir/src/bytearray.cc.s

CMakeFiles/fangsev.dir/src/log.cc.o: CMakeFiles/fangsev.dir/flags.make
CMakeFiles/fangsev.dir/src/log.cc.o: ../src/log.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/fang/workspace/fangser/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_11) "Building CXX object CMakeFiles/fangsev.dir/src/log.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/fangsev.dir/src/log.cc.o -c /home/fang/workspace/fangser/src/log.cc

CMakeFiles/fangsev.dir/src/log.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/fangsev.dir/src/log.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/fang/workspace/fangser/src/log.cc > CMakeFiles/fangsev.dir/src/log.cc.i

CMakeFiles/fangsev.dir/src/log.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/fangsev.dir/src/log.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/fang/workspace/fangser/src/log.cc -o CMakeFiles/fangsev.dir/src/log.cc.s

CMakeFiles/fangsev.dir/src/helpc.cc.o: CMakeFiles/fangsev.dir/flags.make
CMakeFiles/fangsev.dir/src/helpc.cc.o: ../src/helpc.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/fang/workspace/fangser/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_12) "Building CXX object CMakeFiles/fangsev.dir/src/helpc.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/fangsev.dir/src/helpc.cc.o -c /home/fang/workspace/fangser/src/helpc.cc

CMakeFiles/fangsev.dir/src/helpc.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/fangsev.dir/src/helpc.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/fang/workspace/fangser/src/helpc.cc > CMakeFiles/fangsev.dir/src/helpc.cc.i

CMakeFiles/fangsev.dir/src/helpc.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/fangsev.dir/src/helpc.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/fang/workspace/fangser/src/helpc.cc -o CMakeFiles/fangsev.dir/src/helpc.cc.s

CMakeFiles/fangsev.dir/src/fiber.cc.o: CMakeFiles/fangsev.dir/flags.make
CMakeFiles/fangsev.dir/src/fiber.cc.o: ../src/fiber.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/fang/workspace/fangser/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_13) "Building CXX object CMakeFiles/fangsev.dir/src/fiber.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/fangsev.dir/src/fiber.cc.o -c /home/fang/workspace/fangser/src/fiber.cc

CMakeFiles/fangsev.dir/src/fiber.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/fangsev.dir/src/fiber.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/fang/workspace/fangser/src/fiber.cc > CMakeFiles/fangsev.dir/src/fiber.cc.i

CMakeFiles/fangsev.dir/src/fiber.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/fangsev.dir/src/fiber.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/fang/workspace/fangser/src/fiber.cc -o CMakeFiles/fangsev.dir/src/fiber.cc.s

CMakeFiles/fangsev.dir/src/thread.cc.o: CMakeFiles/fangsev.dir/flags.make
CMakeFiles/fangsev.dir/src/thread.cc.o: ../src/thread.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/fang/workspace/fangser/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_14) "Building CXX object CMakeFiles/fangsev.dir/src/thread.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/fangsev.dir/src/thread.cc.o -c /home/fang/workspace/fangser/src/thread.cc

CMakeFiles/fangsev.dir/src/thread.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/fangsev.dir/src/thread.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/fang/workspace/fangser/src/thread.cc > CMakeFiles/fangsev.dir/src/thread.cc.i

CMakeFiles/fangsev.dir/src/thread.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/fangsev.dir/src/thread.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/fang/workspace/fangser/src/thread.cc -o CMakeFiles/fangsev.dir/src/thread.cc.s

CMakeFiles/fangsev.dir/src/scheduler.cc.o: CMakeFiles/fangsev.dir/flags.make
CMakeFiles/fangsev.dir/src/scheduler.cc.o: ../src/scheduler.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/fang/workspace/fangser/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_15) "Building CXX object CMakeFiles/fangsev.dir/src/scheduler.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/fangsev.dir/src/scheduler.cc.o -c /home/fang/workspace/fangser/src/scheduler.cc

CMakeFiles/fangsev.dir/src/scheduler.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/fangsev.dir/src/scheduler.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/fang/workspace/fangser/src/scheduler.cc > CMakeFiles/fangsev.dir/src/scheduler.cc.i

CMakeFiles/fangsev.dir/src/scheduler.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/fangsev.dir/src/scheduler.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/fang/workspace/fangser/src/scheduler.cc -o CMakeFiles/fangsev.dir/src/scheduler.cc.s

CMakeFiles/fangsev.dir/src/iomanager.cc.o: CMakeFiles/fangsev.dir/flags.make
CMakeFiles/fangsev.dir/src/iomanager.cc.o: ../src/iomanager.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/fang/workspace/fangser/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_16) "Building CXX object CMakeFiles/fangsev.dir/src/iomanager.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/fangsev.dir/src/iomanager.cc.o -c /home/fang/workspace/fangser/src/iomanager.cc

CMakeFiles/fangsev.dir/src/iomanager.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/fangsev.dir/src/iomanager.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/fang/workspace/fangser/src/iomanager.cc > CMakeFiles/fangsev.dir/src/iomanager.cc.i

CMakeFiles/fangsev.dir/src/iomanager.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/fangsev.dir/src/iomanager.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/fang/workspace/fangser/src/iomanager.cc -o CMakeFiles/fangsev.dir/src/iomanager.cc.s

CMakeFiles/fangsev.dir/src/stream.cc.o: CMakeFiles/fangsev.dir/flags.make
CMakeFiles/fangsev.dir/src/stream.cc.o: ../src/stream.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/fang/workspace/fangser/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_17) "Building CXX object CMakeFiles/fangsev.dir/src/stream.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/fangsev.dir/src/stream.cc.o -c /home/fang/workspace/fangser/src/stream.cc

CMakeFiles/fangsev.dir/src/stream.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/fangsev.dir/src/stream.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/fang/workspace/fangser/src/stream.cc > CMakeFiles/fangsev.dir/src/stream.cc.i

CMakeFiles/fangsev.dir/src/stream.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/fangsev.dir/src/stream.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/fang/workspace/fangser/src/stream.cc -o CMakeFiles/fangsev.dir/src/stream.cc.s

CMakeFiles/fangsev.dir/src/config.cc.o: CMakeFiles/fangsev.dir/flags.make
CMakeFiles/fangsev.dir/src/config.cc.o: ../src/config.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/fang/workspace/fangser/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_18) "Building CXX object CMakeFiles/fangsev.dir/src/config.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/fangsev.dir/src/config.cc.o -c /home/fang/workspace/fangser/src/config.cc

CMakeFiles/fangsev.dir/src/config.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/fangsev.dir/src/config.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/fang/workspace/fangser/src/config.cc > CMakeFiles/fangsev.dir/src/config.cc.i

CMakeFiles/fangsev.dir/src/config.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/fangsev.dir/src/config.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/fang/workspace/fangser/src/config.cc -o CMakeFiles/fangsev.dir/src/config.cc.s

CMakeFiles/fangsev.dir/src/daemon.cc.o: CMakeFiles/fangsev.dir/flags.make
CMakeFiles/fangsev.dir/src/daemon.cc.o: ../src/daemon.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/fang/workspace/fangser/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_19) "Building CXX object CMakeFiles/fangsev.dir/src/daemon.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/fangsev.dir/src/daemon.cc.o -c /home/fang/workspace/fangser/src/daemon.cc

CMakeFiles/fangsev.dir/src/daemon.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/fangsev.dir/src/daemon.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/fang/workspace/fangser/src/daemon.cc > CMakeFiles/fangsev.dir/src/daemon.cc.i

CMakeFiles/fangsev.dir/src/daemon.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/fangsev.dir/src/daemon.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/fang/workspace/fangser/src/daemon.cc -o CMakeFiles/fangsev.dir/src/daemon.cc.s

CMakeFiles/fangsev.dir/src/env.cc.o: CMakeFiles/fangsev.dir/flags.make
CMakeFiles/fangsev.dir/src/env.cc.o: ../src/env.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/fang/workspace/fangser/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_20) "Building CXX object CMakeFiles/fangsev.dir/src/env.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/fangsev.dir/src/env.cc.o -c /home/fang/workspace/fangser/src/env.cc

CMakeFiles/fangsev.dir/src/env.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/fangsev.dir/src/env.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/fang/workspace/fangser/src/env.cc > CMakeFiles/fangsev.dir/src/env.cc.i

CMakeFiles/fangsev.dir/src/env.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/fangsev.dir/src/env.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/fang/workspace/fangser/src/env.cc -o CMakeFiles/fangsev.dir/src/env.cc.s

CMakeFiles/fangsev.dir/http/http.cc.o: CMakeFiles/fangsev.dir/flags.make
CMakeFiles/fangsev.dir/http/http.cc.o: ../http/http.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/fang/workspace/fangser/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_21) "Building CXX object CMakeFiles/fangsev.dir/http/http.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/fangsev.dir/http/http.cc.o -c /home/fang/workspace/fangser/http/http.cc

CMakeFiles/fangsev.dir/http/http.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/fangsev.dir/http/http.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/fang/workspace/fangser/http/http.cc > CMakeFiles/fangsev.dir/http/http.cc.i

CMakeFiles/fangsev.dir/http/http.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/fangsev.dir/http/http.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/fang/workspace/fangser/http/http.cc -o CMakeFiles/fangsev.dir/http/http.cc.s

CMakeFiles/fangsev.dir/http/http_parser.cc.o: CMakeFiles/fangsev.dir/flags.make
CMakeFiles/fangsev.dir/http/http_parser.cc.o: ../http/http_parser.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/fang/workspace/fangser/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_22) "Building CXX object CMakeFiles/fangsev.dir/http/http_parser.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/fangsev.dir/http/http_parser.cc.o -c /home/fang/workspace/fangser/http/http_parser.cc

CMakeFiles/fangsev.dir/http/http_parser.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/fangsev.dir/http/http_parser.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/fang/workspace/fangser/http/http_parser.cc > CMakeFiles/fangsev.dir/http/http_parser.cc.i

CMakeFiles/fangsev.dir/http/http_parser.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/fangsev.dir/http/http_parser.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/fang/workspace/fangser/http/http_parser.cc -o CMakeFiles/fangsev.dir/http/http_parser.cc.s

CMakeFiles/fangsev.dir/http/http_session.cc.o: CMakeFiles/fangsev.dir/flags.make
CMakeFiles/fangsev.dir/http/http_session.cc.o: ../http/http_session.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/fang/workspace/fangser/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_23) "Building CXX object CMakeFiles/fangsev.dir/http/http_session.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/fangsev.dir/http/http_session.cc.o -c /home/fang/workspace/fangser/http/http_session.cc

CMakeFiles/fangsev.dir/http/http_session.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/fangsev.dir/http/http_session.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/fang/workspace/fangser/http/http_session.cc > CMakeFiles/fangsev.dir/http/http_session.cc.i

CMakeFiles/fangsev.dir/http/http_session.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/fangsev.dir/http/http_session.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/fang/workspace/fangser/http/http_session.cc -o CMakeFiles/fangsev.dir/http/http_session.cc.s

CMakeFiles/fangsev.dir/http/http_connection.cc.o: CMakeFiles/fangsev.dir/flags.make
CMakeFiles/fangsev.dir/http/http_connection.cc.o: ../http/http_connection.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/fang/workspace/fangser/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_24) "Building CXX object CMakeFiles/fangsev.dir/http/http_connection.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/fangsev.dir/http/http_connection.cc.o -c /home/fang/workspace/fangser/http/http_connection.cc

CMakeFiles/fangsev.dir/http/http_connection.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/fangsev.dir/http/http_connection.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/fang/workspace/fangser/http/http_connection.cc > CMakeFiles/fangsev.dir/http/http_connection.cc.i

CMakeFiles/fangsev.dir/http/http_connection.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/fangsev.dir/http/http_connection.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/fang/workspace/fangser/http/http_connection.cc -o CMakeFiles/fangsev.dir/http/http_connection.cc.s

CMakeFiles/fangsev.dir/http/servlet.cc.o: CMakeFiles/fangsev.dir/flags.make
CMakeFiles/fangsev.dir/http/servlet.cc.o: ../http/servlet.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/fang/workspace/fangser/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_25) "Building CXX object CMakeFiles/fangsev.dir/http/servlet.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/fangsev.dir/http/servlet.cc.o -c /home/fang/workspace/fangser/http/servlet.cc

CMakeFiles/fangsev.dir/http/servlet.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/fangsev.dir/http/servlet.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/fang/workspace/fangser/http/servlet.cc > CMakeFiles/fangsev.dir/http/servlet.cc.i

CMakeFiles/fangsev.dir/http/servlet.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/fangsev.dir/http/servlet.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/fang/workspace/fangser/http/servlet.cc -o CMakeFiles/fangsev.dir/http/servlet.cc.s

CMakeFiles/fangsev.dir/stream/socket_stream.cc.o: CMakeFiles/fangsev.dir/flags.make
CMakeFiles/fangsev.dir/stream/socket_stream.cc.o: ../stream/socket_stream.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/fang/workspace/fangser/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_26) "Building CXX object CMakeFiles/fangsev.dir/stream/socket_stream.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/fangsev.dir/stream/socket_stream.cc.o -c /home/fang/workspace/fangser/stream/socket_stream.cc

CMakeFiles/fangsev.dir/stream/socket_stream.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/fangsev.dir/stream/socket_stream.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/fang/workspace/fangser/stream/socket_stream.cc > CMakeFiles/fangsev.dir/stream/socket_stream.cc.i

CMakeFiles/fangsev.dir/stream/socket_stream.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/fangsev.dir/stream/socket_stream.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/fang/workspace/fangser/stream/socket_stream.cc -o CMakeFiles/fangsev.dir/stream/socket_stream.cc.s

CMakeFiles/fangsev.dir/stream/zlib_stream.cc.o: CMakeFiles/fangsev.dir/flags.make
CMakeFiles/fangsev.dir/stream/zlib_stream.cc.o: ../stream/zlib_stream.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/fang/workspace/fangser/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_27) "Building CXX object CMakeFiles/fangsev.dir/stream/zlib_stream.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/fangsev.dir/stream/zlib_stream.cc.o -c /home/fang/workspace/fangser/stream/zlib_stream.cc

CMakeFiles/fangsev.dir/stream/zlib_stream.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/fangsev.dir/stream/zlib_stream.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/fang/workspace/fangser/stream/zlib_stream.cc > CMakeFiles/fangsev.dir/stream/zlib_stream.cc.i

CMakeFiles/fangsev.dir/stream/zlib_stream.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/fangsev.dir/stream/zlib_stream.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/fang/workspace/fangser/stream/zlib_stream.cc -o CMakeFiles/fangsev.dir/stream/zlib_stream.cc.s

CMakeFiles/fangsev.dir/http/http11_parser.rl.cc.o: CMakeFiles/fangsev.dir/flags.make
CMakeFiles/fangsev.dir/http/http11_parser.rl.cc.o: ../http/http11_parser.rl.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/fang/workspace/fangser/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_28) "Building CXX object CMakeFiles/fangsev.dir/http/http11_parser.rl.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/fangsev.dir/http/http11_parser.rl.cc.o -c /home/fang/workspace/fangser/http/http11_parser.rl.cc

CMakeFiles/fangsev.dir/http/http11_parser.rl.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/fangsev.dir/http/http11_parser.rl.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/fang/workspace/fangser/http/http11_parser.rl.cc > CMakeFiles/fangsev.dir/http/http11_parser.rl.cc.i

CMakeFiles/fangsev.dir/http/http11_parser.rl.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/fangsev.dir/http/http11_parser.rl.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/fang/workspace/fangser/http/http11_parser.rl.cc -o CMakeFiles/fangsev.dir/http/http11_parser.rl.cc.s

CMakeFiles/fangsev.dir/http/httpclient_parser.rl.cc.o: CMakeFiles/fangsev.dir/flags.make
CMakeFiles/fangsev.dir/http/httpclient_parser.rl.cc.o: ../http/httpclient_parser.rl.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/fang/workspace/fangser/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_29) "Building CXX object CMakeFiles/fangsev.dir/http/httpclient_parser.rl.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/fangsev.dir/http/httpclient_parser.rl.cc.o -c /home/fang/workspace/fangser/http/httpclient_parser.rl.cc

CMakeFiles/fangsev.dir/http/httpclient_parser.rl.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/fangsev.dir/http/httpclient_parser.rl.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/fang/workspace/fangser/http/httpclient_parser.rl.cc > CMakeFiles/fangsev.dir/http/httpclient_parser.rl.cc.i

CMakeFiles/fangsev.dir/http/httpclient_parser.rl.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/fangsev.dir/http/httpclient_parser.rl.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/fang/workspace/fangser/http/httpclient_parser.rl.cc -o CMakeFiles/fangsev.dir/http/httpclient_parser.rl.cc.s

CMakeFiles/fangsev.dir/inc/uri.rl.cc.o: CMakeFiles/fangsev.dir/flags.make
CMakeFiles/fangsev.dir/inc/uri.rl.cc.o: ../inc/uri.rl.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/fang/workspace/fangser/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_30) "Building CXX object CMakeFiles/fangsev.dir/inc/uri.rl.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/fangsev.dir/inc/uri.rl.cc.o -c /home/fang/workspace/fangser/inc/uri.rl.cc

CMakeFiles/fangsev.dir/inc/uri.rl.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/fangsev.dir/inc/uri.rl.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/fang/workspace/fangser/inc/uri.rl.cc > CMakeFiles/fangsev.dir/inc/uri.rl.cc.i

CMakeFiles/fangsev.dir/inc/uri.rl.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/fangsev.dir/inc/uri.rl.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/fang/workspace/fangser/inc/uri.rl.cc -o CMakeFiles/fangsev.dir/inc/uri.rl.cc.s

# Object files for target fangsev
fangsev_OBJECTS = \
"CMakeFiles/fangsev.dir/src/hook.cc.o" \
"CMakeFiles/fangsev.dir/src/fd_manager.cc.o" \
"CMakeFiles/fangsev.dir/src/mutex.cc.o" \
"CMakeFiles/fangsev.dir/src/timer.cc.o" \
"CMakeFiles/fangsev.dir/src/address.cc.o" \
"CMakeFiles/fangsev.dir/src/socket.cc.o" \
"CMakeFiles/fangsev.dir/src/bytearray.cc.o" \
"CMakeFiles/fangsev.dir/src/log.cc.o" \
"CMakeFiles/fangsev.dir/src/helpc.cc.o" \
"CMakeFiles/fangsev.dir/src/fiber.cc.o" \
"CMakeFiles/fangsev.dir/src/thread.cc.o" \
"CMakeFiles/fangsev.dir/src/scheduler.cc.o" \
"CMakeFiles/fangsev.dir/src/iomanager.cc.o" \
"CMakeFiles/fangsev.dir/src/stream.cc.o" \
"CMakeFiles/fangsev.dir/src/config.cc.o" \
"CMakeFiles/fangsev.dir/src/daemon.cc.o" \
"CMakeFiles/fangsev.dir/src/env.cc.o" \
"CMakeFiles/fangsev.dir/http/http.cc.o" \
"CMakeFiles/fangsev.dir/http/http_parser.cc.o" \
"CMakeFiles/fangsev.dir/http/http_session.cc.o" \
"CMakeFiles/fangsev.dir/http/http_connection.cc.o" \
"CMakeFiles/fangsev.dir/http/servlet.cc.o" \
"CMakeFiles/fangsev.dir/stream/socket_stream.cc.o" \
"CMakeFiles/fangsev.dir/stream/zlib_stream.cc.o" \
"CMakeFiles/fangsev.dir/http/http11_parser.rl.cc.o" \
"CMakeFiles/fangsev.dir/http/httpclient_parser.rl.cc.o" \
"CMakeFiles/fangsev.dir/inc/uri.rl.cc.o"

# External object files for target fangsev
fangsev_EXTERNAL_OBJECTS =

../lib/libfangsev.so: CMakeFiles/fangsev.dir/src/hook.cc.o
../lib/libfangsev.so: CMakeFiles/fangsev.dir/src/fd_manager.cc.o
../lib/libfangsev.so: CMakeFiles/fangsev.dir/src/mutex.cc.o
../lib/libfangsev.so: CMakeFiles/fangsev.dir/src/timer.cc.o
../lib/libfangsev.so: CMakeFiles/fangsev.dir/src/address.cc.o
../lib/libfangsev.so: CMakeFiles/fangsev.dir/src/socket.cc.o
../lib/libfangsev.so: CMakeFiles/fangsev.dir/src/bytearray.cc.o
../lib/libfangsev.so: CMakeFiles/fangsev.dir/src/log.cc.o
../lib/libfangsev.so: CMakeFiles/fangsev.dir/src/helpc.cc.o
../lib/libfangsev.so: CMakeFiles/fangsev.dir/src/fiber.cc.o
../lib/libfangsev.so: CMakeFiles/fangsev.dir/src/thread.cc.o
../lib/libfangsev.so: CMakeFiles/fangsev.dir/src/scheduler.cc.o
../lib/libfangsev.so: CMakeFiles/fangsev.dir/src/iomanager.cc.o
../lib/libfangsev.so: CMakeFiles/fangsev.dir/src/stream.cc.o
../lib/libfangsev.so: CMakeFiles/fangsev.dir/src/config.cc.o
../lib/libfangsev.so: CMakeFiles/fangsev.dir/src/daemon.cc.o
../lib/libfangsev.so: CMakeFiles/fangsev.dir/src/env.cc.o
../lib/libfangsev.so: CMakeFiles/fangsev.dir/http/http.cc.o
../lib/libfangsev.so: CMakeFiles/fangsev.dir/http/http_parser.cc.o
../lib/libfangsev.so: CMakeFiles/fangsev.dir/http/http_session.cc.o
../lib/libfangsev.so: CMakeFiles/fangsev.dir/http/http_connection.cc.o
../lib/libfangsev.so: CMakeFiles/fangsev.dir/http/servlet.cc.o
../lib/libfangsev.so: CMakeFiles/fangsev.dir/stream/socket_stream.cc.o
../lib/libfangsev.so: CMakeFiles/fangsev.dir/stream/zlib_stream.cc.o
../lib/libfangsev.so: CMakeFiles/fangsev.dir/http/http11_parser.rl.cc.o
../lib/libfangsev.so: CMakeFiles/fangsev.dir/http/httpclient_parser.rl.cc.o
../lib/libfangsev.so: CMakeFiles/fangsev.dir/inc/uri.rl.cc.o
../lib/libfangsev.so: CMakeFiles/fangsev.dir/build.make
../lib/libfangsev.so: CMakeFiles/fangsev.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/fang/workspace/fangser/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_31) "Linking CXX shared library ../lib/libfangsev.so"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/fangsev.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/fangsev.dir/build: ../lib/libfangsev.so

.PHONY : CMakeFiles/fangsev.dir/build

CMakeFiles/fangsev.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/fangsev.dir/cmake_clean.cmake
.PHONY : CMakeFiles/fangsev.dir/clean

CMakeFiles/fangsev.dir/depend: ../http/http11_parser.rl.cc
CMakeFiles/fangsev.dir/depend: ../http/httpclient_parser.rl.cc
CMakeFiles/fangsev.dir/depend: ../inc/uri.rl.cc
	cd /home/fang/workspace/fangser/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/fang/workspace/fangser /home/fang/workspace/fangser /home/fang/workspace/fangser/build /home/fang/workspace/fangser/build /home/fang/workspace/fangser/build/CMakeFiles/fangsev.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/fangsev.dir/depend

