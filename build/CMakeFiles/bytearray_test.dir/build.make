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
include CMakeFiles/bytearray_test.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/bytearray_test.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/bytearray_test.dir/flags.make

CMakeFiles/bytearray_test.dir/tests/test_bytearray.cc.o: CMakeFiles/bytearray_test.dir/flags.make
CMakeFiles/bytearray_test.dir/tests/test_bytearray.cc.o: ../tests/test_bytearray.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/fang/workspace/fangser/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/bytearray_test.dir/tests/test_bytearray.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/bytearray_test.dir/tests/test_bytearray.cc.o -c /home/fang/workspace/fangser/tests/test_bytearray.cc

CMakeFiles/bytearray_test.dir/tests/test_bytearray.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/bytearray_test.dir/tests/test_bytearray.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/fang/workspace/fangser/tests/test_bytearray.cc > CMakeFiles/bytearray_test.dir/tests/test_bytearray.cc.i

CMakeFiles/bytearray_test.dir/tests/test_bytearray.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/bytearray_test.dir/tests/test_bytearray.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/fang/workspace/fangser/tests/test_bytearray.cc -o CMakeFiles/bytearray_test.dir/tests/test_bytearray.cc.s

# Object files for target bytearray_test
bytearray_test_OBJECTS = \
"CMakeFiles/bytearray_test.dir/tests/test_bytearray.cc.o"

# External object files for target bytearray_test
bytearray_test_EXTERNAL_OBJECTS =

../bin/bytearray_test: CMakeFiles/bytearray_test.dir/tests/test_bytearray.cc.o
../bin/bytearray_test: CMakeFiles/bytearray_test.dir/build.make
../bin/bytearray_test: ../lib/libfangsev.so
../bin/bytearray_test: CMakeFiles/bytearray_test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/fang/workspace/fangser/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../bin/bytearray_test"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/bytearray_test.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/bytearray_test.dir/build: ../bin/bytearray_test

.PHONY : CMakeFiles/bytearray_test.dir/build

CMakeFiles/bytearray_test.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/bytearray_test.dir/cmake_clean.cmake
.PHONY : CMakeFiles/bytearray_test.dir/clean

CMakeFiles/bytearray_test.dir/depend:
	cd /home/fang/workspace/fangser/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/fang/workspace/fangser /home/fang/workspace/fangser /home/fang/workspace/fangser/build /home/fang/workspace/fangser/build /home/fang/workspace/fangser/build/CMakeFiles/bytearray_test.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/bytearray_test.dir/depend

