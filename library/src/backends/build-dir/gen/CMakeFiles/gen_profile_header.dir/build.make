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
CMAKE_SOURCE_DIR = /home/upmem0037/xialinl/research/libaray/src/backends

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/upmem0037/xialinl/research/libaray/src/backends/build-dir

# Utility rule file for gen_profile_header.

# Include the progress variables for this target.
include gen/CMakeFiles/gen_profile_header.dir/progress.make

gen/CMakeFiles/gen_profile_header: gen/include/dpu_profile_properties.h


gen/include/dpu_profile_properties.h: ../gen/gen_profile_files.py
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/upmem0037/xialinl/research/libaray/src/backends/build-dir/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Generating /home/upmem0037/xialinl/research/libaray/src/backends/build-dir/gen/include/dpu_profile_properties.h from /home/upmem0037/xialinl/research/libaray/src/backends/gen/gen_profile_files.py"
	cd /home/upmem0037/xialinl/research/libaray/src/backends/build-dir/gen && /usr/bin/python3.7 /home/upmem0037/xialinl/research/libaray/src/backends/gen/gen_profile_files.py defs /home/upmem0037/xialinl/research/libaray/src/backends/build-dir/gen/include/dpu_profile_properties.h

gen_profile_header: gen/CMakeFiles/gen_profile_header
gen_profile_header: gen/include/dpu_profile_properties.h
gen_profile_header: gen/CMakeFiles/gen_profile_header.dir/build.make

.PHONY : gen_profile_header

# Rule to build all files generated by this target.
gen/CMakeFiles/gen_profile_header.dir/build: gen_profile_header

.PHONY : gen/CMakeFiles/gen_profile_header.dir/build

gen/CMakeFiles/gen_profile_header.dir/clean:
	cd /home/upmem0037/xialinl/research/libaray/src/backends/build-dir/gen && $(CMAKE_COMMAND) -P CMakeFiles/gen_profile_header.dir/cmake_clean.cmake
.PHONY : gen/CMakeFiles/gen_profile_header.dir/clean

gen/CMakeFiles/gen_profile_header.dir/depend:
	cd /home/upmem0037/xialinl/research/libaray/src/backends/build-dir && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/upmem0037/xialinl/research/libaray/src/backends /home/upmem0037/xialinl/research/libaray/src/backends/gen /home/upmem0037/xialinl/research/libaray/src/backends/build-dir /home/upmem0037/xialinl/research/libaray/src/backends/build-dir/gen /home/upmem0037/xialinl/research/libaray/src/backends/build-dir/gen/CMakeFiles/gen_profile_header.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : gen/CMakeFiles/gen_profile_header.dir/depend
