# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.17

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Disable VCS-based implicit rules.
% : %,v


# Disable VCS-based implicit rules.
% : RCS/%


# Disable VCS-based implicit rules.
% : RCS/%,v


# Disable VCS-based implicit rules.
% : SCCS/s.%


# Disable VCS-based implicit rules.
% : s.%


.SUFFIXES: .hpux_make_needs_suffix_list


# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

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
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/sn/ucode/backup/json-c

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/sn/ucode/backup/json-c

# Include any dependencies generated for this target.
include tests/CMakeFiles/test_cast.dir/depend.make

# Include the progress variables for this target.
include tests/CMakeFiles/test_cast.dir/progress.make

# Include the compile flags for this target's objects.
include tests/CMakeFiles/test_cast.dir/flags.make

tests/CMakeFiles/test_cast.dir/test_cast.c.o: tests/CMakeFiles/test_cast.dir/flags.make
tests/CMakeFiles/test_cast.dir/test_cast.c.o: tests/test_cast.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/sn/ucode/backup/json-c/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object tests/CMakeFiles/test_cast.dir/test_cast.c.o"
	cd /Users/sn/ucode/backup/json-c/tests && /Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/test_cast.dir/test_cast.c.o   -c /Users/sn/ucode/backup/json-c/tests/test_cast.c

tests/CMakeFiles/test_cast.dir/test_cast.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/test_cast.dir/test_cast.c.i"
	cd /Users/sn/ucode/backup/json-c/tests && /Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/sn/ucode/backup/json-c/tests/test_cast.c > CMakeFiles/test_cast.dir/test_cast.c.i

tests/CMakeFiles/test_cast.dir/test_cast.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/test_cast.dir/test_cast.c.s"
	cd /Users/sn/ucode/backup/json-c/tests && /Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/sn/ucode/backup/json-c/tests/test_cast.c -o CMakeFiles/test_cast.dir/test_cast.c.s

# Object files for target test_cast
test_cast_OBJECTS = \
"CMakeFiles/test_cast.dir/test_cast.c.o"

# External object files for target test_cast
test_cast_EXTERNAL_OBJECTS =

tests/test_cast: tests/CMakeFiles/test_cast.dir/test_cast.c.o
tests/test_cast: tests/CMakeFiles/test_cast.dir/build.make
tests/test_cast: libjson-c.5.0.0.dylib
tests/test_cast: tests/CMakeFiles/test_cast.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/sn/ucode/backup/json-c/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable test_cast"
	cd /Users/sn/ucode/backup/json-c/tests && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test_cast.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
tests/CMakeFiles/test_cast.dir/build: tests/test_cast

.PHONY : tests/CMakeFiles/test_cast.dir/build

tests/CMakeFiles/test_cast.dir/clean:
	cd /Users/sn/ucode/backup/json-c/tests && $(CMAKE_COMMAND) -P CMakeFiles/test_cast.dir/cmake_clean.cmake
.PHONY : tests/CMakeFiles/test_cast.dir/clean

tests/CMakeFiles/test_cast.dir/depend:
	cd /Users/sn/ucode/backup/json-c && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/sn/ucode/backup/json-c /Users/sn/ucode/backup/json-c/tests /Users/sn/ucode/backup/json-c /Users/sn/ucode/backup/json-c/tests /Users/sn/ucode/backup/json-c/tests/CMakeFiles/test_cast.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : tests/CMakeFiles/test_cast.dir/depend

