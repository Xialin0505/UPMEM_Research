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

# Include any dependencies generated for this target.
include vpd/CMakeFiles/dpuvpd.dir/depend.make

# Include the progress variables for this target.
include vpd/CMakeFiles/dpuvpd.dir/progress.make

# Include the compile flags for this target's objects.
include vpd/CMakeFiles/dpuvpd.dir/flags.make

vpd/CMakeFiles/dpuvpd.dir/src/dpu_vpd.c.o: vpd/CMakeFiles/dpuvpd.dir/flags.make
vpd/CMakeFiles/dpuvpd.dir/src/dpu_vpd.c.o: ../vpd/src/dpu_vpd.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/upmem0037/xialinl/research/libaray/src/backends/build-dir/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object vpd/CMakeFiles/dpuvpd.dir/src/dpu_vpd.c.o"
	cd /home/upmem0037/xialinl/research/libaray/src/backends/build-dir/vpd && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/dpuvpd.dir/src/dpu_vpd.c.o   -c /home/upmem0037/xialinl/research/libaray/src/backends/vpd/src/dpu_vpd.c

vpd/CMakeFiles/dpuvpd.dir/src/dpu_vpd.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/dpuvpd.dir/src/dpu_vpd.c.i"
	cd /home/upmem0037/xialinl/research/libaray/src/backends/build-dir/vpd && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/upmem0037/xialinl/research/libaray/src/backends/vpd/src/dpu_vpd.c > CMakeFiles/dpuvpd.dir/src/dpu_vpd.c.i

vpd/CMakeFiles/dpuvpd.dir/src/dpu_vpd.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/dpuvpd.dir/src/dpu_vpd.c.s"
	cd /home/upmem0037/xialinl/research/libaray/src/backends/build-dir/vpd && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/upmem0037/xialinl/research/libaray/src/backends/vpd/src/dpu_vpd.c -o CMakeFiles/dpuvpd.dir/src/dpu_vpd.c.s

vpd/CMakeFiles/dpuvpd.dir/src/dpu_vpd_container.c.o: vpd/CMakeFiles/dpuvpd.dir/flags.make
vpd/CMakeFiles/dpuvpd.dir/src/dpu_vpd_container.c.o: ../vpd/src/dpu_vpd_container.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/upmem0037/xialinl/research/libaray/src/backends/build-dir/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object vpd/CMakeFiles/dpuvpd.dir/src/dpu_vpd_container.c.o"
	cd /home/upmem0037/xialinl/research/libaray/src/backends/build-dir/vpd && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/dpuvpd.dir/src/dpu_vpd_container.c.o   -c /home/upmem0037/xialinl/research/libaray/src/backends/vpd/src/dpu_vpd_container.c

vpd/CMakeFiles/dpuvpd.dir/src/dpu_vpd_container.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/dpuvpd.dir/src/dpu_vpd_container.c.i"
	cd /home/upmem0037/xialinl/research/libaray/src/backends/build-dir/vpd && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/upmem0037/xialinl/research/libaray/src/backends/vpd/src/dpu_vpd_container.c > CMakeFiles/dpuvpd.dir/src/dpu_vpd_container.c.i

vpd/CMakeFiles/dpuvpd.dir/src/dpu_vpd_container.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/dpuvpd.dir/src/dpu_vpd_container.c.s"
	cd /home/upmem0037/xialinl/research/libaray/src/backends/build-dir/vpd && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/upmem0037/xialinl/research/libaray/src/backends/vpd/src/dpu_vpd_container.c -o CMakeFiles/dpuvpd.dir/src/dpu_vpd_container.c.s

vpd/CMakeFiles/dpuvpd.dir/src/dpu_vpd_decode.c.o: vpd/CMakeFiles/dpuvpd.dir/flags.make
vpd/CMakeFiles/dpuvpd.dir/src/dpu_vpd_decode.c.o: ../vpd/src/dpu_vpd_decode.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/upmem0037/xialinl/research/libaray/src/backends/build-dir/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object vpd/CMakeFiles/dpuvpd.dir/src/dpu_vpd_decode.c.o"
	cd /home/upmem0037/xialinl/research/libaray/src/backends/build-dir/vpd && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/dpuvpd.dir/src/dpu_vpd_decode.c.o   -c /home/upmem0037/xialinl/research/libaray/src/backends/vpd/src/dpu_vpd_decode.c

vpd/CMakeFiles/dpuvpd.dir/src/dpu_vpd_decode.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/dpuvpd.dir/src/dpu_vpd_decode.c.i"
	cd /home/upmem0037/xialinl/research/libaray/src/backends/build-dir/vpd && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/upmem0037/xialinl/research/libaray/src/backends/vpd/src/dpu_vpd_decode.c > CMakeFiles/dpuvpd.dir/src/dpu_vpd_decode.c.i

vpd/CMakeFiles/dpuvpd.dir/src/dpu_vpd_decode.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/dpuvpd.dir/src/dpu_vpd_decode.c.s"
	cd /home/upmem0037/xialinl/research/libaray/src/backends/build-dir/vpd && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/upmem0037/xialinl/research/libaray/src/backends/vpd/src/dpu_vpd_decode.c -o CMakeFiles/dpuvpd.dir/src/dpu_vpd_decode.c.s

vpd/CMakeFiles/dpuvpd.dir/src/dpu_vpd_encode.c.o: vpd/CMakeFiles/dpuvpd.dir/flags.make
vpd/CMakeFiles/dpuvpd.dir/src/dpu_vpd_encode.c.o: ../vpd/src/dpu_vpd_encode.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/upmem0037/xialinl/research/libaray/src/backends/build-dir/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building C object vpd/CMakeFiles/dpuvpd.dir/src/dpu_vpd_encode.c.o"
	cd /home/upmem0037/xialinl/research/libaray/src/backends/build-dir/vpd && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/dpuvpd.dir/src/dpu_vpd_encode.c.o   -c /home/upmem0037/xialinl/research/libaray/src/backends/vpd/src/dpu_vpd_encode.c

vpd/CMakeFiles/dpuvpd.dir/src/dpu_vpd_encode.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/dpuvpd.dir/src/dpu_vpd_encode.c.i"
	cd /home/upmem0037/xialinl/research/libaray/src/backends/build-dir/vpd && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/upmem0037/xialinl/research/libaray/src/backends/vpd/src/dpu_vpd_encode.c > CMakeFiles/dpuvpd.dir/src/dpu_vpd_encode.c.i

vpd/CMakeFiles/dpuvpd.dir/src/dpu_vpd_encode.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/dpuvpd.dir/src/dpu_vpd_encode.c.s"
	cd /home/upmem0037/xialinl/research/libaray/src/backends/build-dir/vpd && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/upmem0037/xialinl/research/libaray/src/backends/vpd/src/dpu_vpd_encode.c -o CMakeFiles/dpuvpd.dir/src/dpu_vpd_encode.c.s

# Object files for target dpuvpd
dpuvpd_OBJECTS = \
"CMakeFiles/dpuvpd.dir/src/dpu_vpd.c.o" \
"CMakeFiles/dpuvpd.dir/src/dpu_vpd_container.c.o" \
"CMakeFiles/dpuvpd.dir/src/dpu_vpd_decode.c.o" \
"CMakeFiles/dpuvpd.dir/src/dpu_vpd_encode.c.o"

# External object files for target dpuvpd
dpuvpd_EXTERNAL_OBJECTS =

vpd/libdpuvpd.so.0.0: vpd/CMakeFiles/dpuvpd.dir/src/dpu_vpd.c.o
vpd/libdpuvpd.so.0.0: vpd/CMakeFiles/dpuvpd.dir/src/dpu_vpd_container.c.o
vpd/libdpuvpd.so.0.0: vpd/CMakeFiles/dpuvpd.dir/src/dpu_vpd_decode.c.o
vpd/libdpuvpd.so.0.0: vpd/CMakeFiles/dpuvpd.dir/src/dpu_vpd_encode.c.o
vpd/libdpuvpd.so.0.0: vpd/CMakeFiles/dpuvpd.dir/build.make
vpd/libdpuvpd.so.0.0: verbose/libdpuverbose.so.0.0
vpd/libdpuvpd.so.0.0: vpd/CMakeFiles/dpuvpd.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/upmem0037/xialinl/research/libaray/src/backends/build-dir/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Linking C shared library libdpuvpd.so"
	cd /home/upmem0037/xialinl/research/libaray/src/backends/build-dir/vpd && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/dpuvpd.dir/link.txt --verbose=$(VERBOSE)
	cd /home/upmem0037/xialinl/research/libaray/src/backends/build-dir/vpd && $(CMAKE_COMMAND) -E cmake_symlink_library libdpuvpd.so.0.0 libdpuvpd.so.0.0 libdpuvpd.so

vpd/libdpuvpd.so: vpd/libdpuvpd.so.0.0
	@$(CMAKE_COMMAND) -E touch_nocreate vpd/libdpuvpd.so

# Rule to build all files generated by this target.
vpd/CMakeFiles/dpuvpd.dir/build: vpd/libdpuvpd.so

.PHONY : vpd/CMakeFiles/dpuvpd.dir/build

vpd/CMakeFiles/dpuvpd.dir/clean:
	cd /home/upmem0037/xialinl/research/libaray/src/backends/build-dir/vpd && $(CMAKE_COMMAND) -P CMakeFiles/dpuvpd.dir/cmake_clean.cmake
.PHONY : vpd/CMakeFiles/dpuvpd.dir/clean

vpd/CMakeFiles/dpuvpd.dir/depend:
	cd /home/upmem0037/xialinl/research/libaray/src/backends/build-dir && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/upmem0037/xialinl/research/libaray/src/backends /home/upmem0037/xialinl/research/libaray/src/backends/vpd /home/upmem0037/xialinl/research/libaray/src/backends/build-dir /home/upmem0037/xialinl/research/libaray/src/backends/build-dir/vpd /home/upmem0037/xialinl/research/libaray/src/backends/build-dir/vpd/CMakeFiles/dpuvpd.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : vpd/CMakeFiles/dpuvpd.dir/depend

