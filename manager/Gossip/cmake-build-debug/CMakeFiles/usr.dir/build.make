# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.20

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

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake

# The command to remove a file.
RM = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/tangrenchu/CLionProjects/TinySwarm/manager/Gossip

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/tangrenchu/CLionProjects/TinySwarm/manager/Gossip/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/usr.dir/depend.make
# Include the progress variables for this target.
include CMakeFiles/usr.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/usr.dir/flags.make

CMakeFiles/usr.dir/src/UserInterface.cpp.o: CMakeFiles/usr.dir/flags.make
CMakeFiles/usr.dir/src/UserInterface.cpp.o: ../src/UserInterface.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/tangrenchu/CLionProjects/TinySwarm/manager/Gossip/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/usr.dir/src/UserInterface.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/usr.dir/src/UserInterface.cpp.o -c /Users/tangrenchu/CLionProjects/TinySwarm/manager/Gossip/src/UserInterface.cpp

CMakeFiles/usr.dir/src/UserInterface.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/usr.dir/src/UserInterface.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/tangrenchu/CLionProjects/TinySwarm/manager/Gossip/src/UserInterface.cpp > CMakeFiles/usr.dir/src/UserInterface.cpp.i

CMakeFiles/usr.dir/src/UserInterface.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/usr.dir/src/UserInterface.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/tangrenchu/CLionProjects/TinySwarm/manager/Gossip/src/UserInterface.cpp -o CMakeFiles/usr.dir/src/UserInterface.cpp.s

# Object files for target usr
usr_OBJECTS = \
"CMakeFiles/usr.dir/src/UserInterface.cpp.o"

# External object files for target usr
usr_EXTERNAL_OBJECTS =

../bin/usr: CMakeFiles/usr.dir/src/UserInterface.cpp.o
../bin/usr: CMakeFiles/usr.dir/build.make
../bin/usr: CMakeFiles/usr.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/tangrenchu/CLionProjects/TinySwarm/manager/Gossip/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../bin/usr"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/usr.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/usr.dir/build: ../bin/usr
.PHONY : CMakeFiles/usr.dir/build

CMakeFiles/usr.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/usr.dir/cmake_clean.cmake
.PHONY : CMakeFiles/usr.dir/clean

CMakeFiles/usr.dir/depend:
	cd /Users/tangrenchu/CLionProjects/TinySwarm/manager/Gossip/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/tangrenchu/CLionProjects/TinySwarm/manager/Gossip /Users/tangrenchu/CLionProjects/TinySwarm/manager/Gossip /Users/tangrenchu/CLionProjects/TinySwarm/manager/Gossip/cmake-build-debug /Users/tangrenchu/CLionProjects/TinySwarm/manager/Gossip/cmake-build-debug /Users/tangrenchu/CLionProjects/TinySwarm/manager/Gossip/cmake-build-debug/CMakeFiles/usr.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/usr.dir/depend

