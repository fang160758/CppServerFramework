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
include CMakeFiles/daemon_test.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/daemon_test.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/daemon_test.dir/flags.make

CMakeFiles/daemon_test.dir/tests/daemon_test.cc.o: CMakeFiles/daemon_test.dir/flags.make
CMakeFiles/daemon_test.dir/tests/daemon_test.cc.o: ../tests/daemon_test.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/fang/workspace/fangser/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/daemon_test.dir/tests/daemon_test.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/daemon_test.dir/tests/daemon_test.cc.o -c /home/fang/workspace/fangser/tests/daemon_test.cc

CMakeFiles/daemon_test.dir/tests/daemon_test.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/daemon_test.dir/tests/daemon_test.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/fang/workspace/fangser/tests/daemon_test.cc > CMakeFiles/daemon_test.dir/tests/daemon_test.cc.i

CMakeFiles/daemon_test.dir/tests/daemon_test.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/daemon_test.dir/tests/daemon_test.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/fang/workspace/fangser/tests/daemon_test.cc -o CMakeFiles/daemon_test.dir/tests/daemon_test.cc.s

# Object files for target daemon_test
daemon_test_OBJECTS = \
"CMakeFiles/daemon_test.dir/tests/daemon_test.cc.o"

# External object files for target daemon_test
daemon_test_EXTERNAL_OBJECTS =

../bin/daemon_test: CMakeFiles/daemon_test.dir/tests/daemon_test.cc.o
../bin/daemon_test: CMakeFiles/daemon_test.dir/build.make
../bin/daemon_test: ../lib/libfangsev.so
../bin/daemon_test: CMakeFiles/daemon_test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/fang/workspace/fangser/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../bin/daemon_test"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/daemon_test.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/daemon_test.dir/build: ../bin/daemon_test

.PHONY : CMakeFiles/daemon_test.dir/build

CMakeFiles/daemon_test.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/daemon_test.dir/cmake_clean.cmake
.PHONY : CMakeFiles/daemon_test.dir/clean

CMakeFiles/daemon_test.dir/depend:
	cd /home/fang/workspace/fangser/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/fang/workspace/fangser /home/fang/workspace/fangser /home/fang/workspace/fangser/build /home/fang/workspace/fangser/build /home/fang/workspace/fangser/build/CMakeFiles/daemon_test.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/daemon_test.dir/depend

