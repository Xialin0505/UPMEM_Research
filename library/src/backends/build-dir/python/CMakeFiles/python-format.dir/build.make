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

# Utility rule file for python-format.

# Include the progress variables for this target.
include python/CMakeFiles/python-format.dir/progress.make

python/CMakeFiles/python-format:
	cd /home/upmem0037/xialinl/research/libaray/src/backends/python && PYFORMAT-NOTFOUND dpu/__init__.py dpu/compiler.py dpu/driver.py dpu/vpd/db.py dpu/vpd/dimm.py tests/*.py --in-place --aggressive --aggressive

python-format: python/CMakeFiles/python-format
python-format: python/CMakeFiles/python-format.dir/build.make

.PHONY : python-format

# Rule to build all files generated by this target.
python/CMakeFiles/python-format.dir/build: python-format

.PHONY : python/CMakeFiles/python-format.dir/build

python/CMakeFiles/python-format.dir/clean:
	cd /home/upmem0037/xialinl/research/libaray/src/backends/build-dir/python && $(CMAKE_COMMAND) -P CMakeFiles/python-format.dir/cmake_clean.cmake
.PHONY : python/CMakeFiles/python-format.dir/clean

python/CMakeFiles/python-format.dir/depend:
	cd /home/upmem0037/xialinl/research/libaray/src/backends/build-dir && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/upmem0037/xialinl/research/libaray/src/backends /home/upmem0037/xialinl/research/libaray/src/backends/python /home/upmem0037/xialinl/research/libaray/src/backends/build-dir /home/upmem0037/xialinl/research/libaray/src/backends/build-dir/python /home/upmem0037/xialinl/research/libaray/src/backends/build-dir/python/CMakeFiles/python-format.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : python/CMakeFiles/python-format.dir/depend

