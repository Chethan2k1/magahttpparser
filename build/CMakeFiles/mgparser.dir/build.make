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
CMAKE_SOURCE_DIR = /home/crdy/Documents/magahttpparser

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/crdy/Documents/magahttpparser/build

# Include any dependencies generated for this target.
include CMakeFiles/mgparser.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/mgparser.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/mgparser.dir/flags.make

CMakeFiles/mgparser.dir/src/parser.cpp.o: CMakeFiles/mgparser.dir/flags.make
CMakeFiles/mgparser.dir/src/parser.cpp.o: ../src/parser.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/crdy/Documents/magahttpparser/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/mgparser.dir/src/parser.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/mgparser.dir/src/parser.cpp.o -c /home/crdy/Documents/magahttpparser/src/parser.cpp

CMakeFiles/mgparser.dir/src/parser.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/mgparser.dir/src/parser.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/crdy/Documents/magahttpparser/src/parser.cpp > CMakeFiles/mgparser.dir/src/parser.cpp.i

CMakeFiles/mgparser.dir/src/parser.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/mgparser.dir/src/parser.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/crdy/Documents/magahttpparser/src/parser.cpp -o CMakeFiles/mgparser.dir/src/parser.cpp.s

# Object files for target mgparser
mgparser_OBJECTS = \
"CMakeFiles/mgparser.dir/src/parser.cpp.o"

# External object files for target mgparser
mgparser_EXTERNAL_OBJECTS =

libmgparser.a: CMakeFiles/mgparser.dir/src/parser.cpp.o
libmgparser.a: CMakeFiles/mgparser.dir/build.make
libmgparser.a: CMakeFiles/mgparser.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/crdy/Documents/magahttpparser/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library libmgparser.a"
	$(CMAKE_COMMAND) -P CMakeFiles/mgparser.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/mgparser.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/mgparser.dir/build: libmgparser.a

.PHONY : CMakeFiles/mgparser.dir/build

CMakeFiles/mgparser.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/mgparser.dir/cmake_clean.cmake
.PHONY : CMakeFiles/mgparser.dir/clean

CMakeFiles/mgparser.dir/depend:
	cd /home/crdy/Documents/magahttpparser/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/crdy/Documents/magahttpparser /home/crdy/Documents/magahttpparser /home/crdy/Documents/magahttpparser/build /home/crdy/Documents/magahttpparser/build /home/crdy/Documents/magahttpparser/build/CMakeFiles/mgparser.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/mgparser.dir/depend
