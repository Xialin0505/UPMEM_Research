# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.13

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


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
CMAKE_SOURCE_DIR = /home/upmem0037/xialinl/research/libaray/src/dpu-rt

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/upmem0037/xialinl/research/libaray/src/dpu-rt/build-dir

# Include any dependencies generated for this target.
include CMakeFiles/internalStateReset.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/internalStateReset.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/internalStateReset.dir/flags.make

CMakeFiles/internalStateReset.dir/src/misc/internalStateReset.c.o: CMakeFiles/internalStateReset.dir/flags.make
CMakeFiles/internalStateReset.dir/src/misc/internalStateReset.c.o: ../src/misc/internalStateReset.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/upmem0037/xialinl/research/libaray/src/dpu-rt/build-dir/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/internalStateReset.dir/src/misc/internalStateReset.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/internalStateReset.dir/src/misc/internalStateReset.c.o   -c /home/upmem0037/xialinl/research/libaray/src/dpu-rt/src/misc/internalStateReset.c

CMakeFiles/internalStateReset.dir/src/misc/internalStateReset.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/internalStateReset.dir/src/misc/internalStateReset.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/upmem0037/xialinl/research/libaray/src/dpu-rt/src/misc/internalStateReset.c > CMakeFiles/internalStateReset.dir/src/misc/internalStateReset.c.i

CMakeFiles/internalStateReset.dir/src/misc/internalStateReset.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/internalStateReset.dir/src/misc/internalStateReset.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/upmem0037/xialinl/research/libaray/src/dpu-rt/src/misc/internalStateReset.c -o CMakeFiles/internalStateReset.dir/src/misc/internalStateReset.c.s

# Object files for target internalStateReset
internalStateReset_OBJECTS = \
"CMakeFiles/internalStateReset.dir/src/misc/internalStateReset.c.o"

# External object files for target internalStateReset
internalStateReset_EXTERNAL_OBJECTS =

internalStateReset: CMakeFiles/internalStateReset.dir/src/misc/internalStateReset.c.o
internalStateReset: CMakeFiles/internalStateReset.dir/build.make
internalStateReset: CMakeFiles/internalStateReset.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/upmem0037/xialinl/research/libaray/src/dpu-rt/build-dir/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable internalStateReset"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/internalStateReset.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/internalStateReset.dir/build: internalStateReset

.PHONY : CMakeFiles/internalStateReset.dir/build

CMakeFiles/internalStateReset.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/internalStateReset.dir/cmake_clean.cmake
.PHONY : CMakeFiles/internalStateReset.dir/clean

CMakeFiles/internalStateReset.dir/depend:
	cd /home/upmem0037/xialinl/research/libaray/src/dpu-rt/build-dir && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/upmem0037/xialinl/research/libaray/src/dpu-rt /home/upmem0037/xialinl/research/libaray/src/dpu-rt /home/upmem0037/xialinl/research/libaray/src/dpu-rt/build-dir /home/upmem0037/xialinl/research/libaray/src/dpu-rt/build-dir /home/upmem0037/xialinl/research/libaray/src/dpu-rt/build-dir/CMakeFiles/internalStateReset.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/internalStateReset.dir/depend
