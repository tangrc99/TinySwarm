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
CMAKE_SOURCE_DIR = /Users/tangrenchu/CLionProjects/TinySwarm

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug

# Include any dependencies generated for this target.
include Yolo/CMakeFiles/yolo.dir/depend.make
# Include the progress variables for this target.
include Yolo/CMakeFiles/yolo.dir/progress.make

# Include the compile flags for this target's objects.
include Yolo/CMakeFiles/yolo.dir/flags.make

Yolo/CMakeFiles/yolo.dir/main.cpp.o: Yolo/CMakeFiles/yolo.dir/flags.make
Yolo/CMakeFiles/yolo.dir/main.cpp.o: ../Yolo/main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object Yolo/CMakeFiles/yolo.dir/main.cpp.o"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/Yolo && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/yolo.dir/main.cpp.o -c /Users/tangrenchu/CLionProjects/TinySwarm/Yolo/main.cpp

Yolo/CMakeFiles/yolo.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/yolo.dir/main.cpp.i"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/Yolo && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/tangrenchu/CLionProjects/TinySwarm/Yolo/main.cpp > CMakeFiles/yolo.dir/main.cpp.i

Yolo/CMakeFiles/yolo.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/yolo.dir/main.cpp.s"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/Yolo && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/tangrenchu/CLionProjects/TinySwarm/Yolo/main.cpp -o CMakeFiles/yolo.dir/main.cpp.s

Yolo/CMakeFiles/yolo.dir/RpcInterface.cpp.o: Yolo/CMakeFiles/yolo.dir/flags.make
Yolo/CMakeFiles/yolo.dir/RpcInterface.cpp.o: ../Yolo/RpcInterface.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object Yolo/CMakeFiles/yolo.dir/RpcInterface.cpp.o"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/Yolo && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/yolo.dir/RpcInterface.cpp.o -c /Users/tangrenchu/CLionProjects/TinySwarm/Yolo/RpcInterface.cpp

Yolo/CMakeFiles/yolo.dir/RpcInterface.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/yolo.dir/RpcInterface.cpp.i"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/Yolo && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/tangrenchu/CLionProjects/TinySwarm/Yolo/RpcInterface.cpp > CMakeFiles/yolo.dir/RpcInterface.cpp.i

Yolo/CMakeFiles/yolo.dir/RpcInterface.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/yolo.dir/RpcInterface.cpp.s"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/Yolo && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/tangrenchu/CLionProjects/TinySwarm/Yolo/RpcInterface.cpp -o CMakeFiles/yolo.dir/RpcInterface.cpp.s

Yolo/CMakeFiles/yolo.dir/__/Common/HTTPData.cpp.o: Yolo/CMakeFiles/yolo.dir/flags.make
Yolo/CMakeFiles/yolo.dir/__/Common/HTTPData.cpp.o: ../Common/HTTPData.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object Yolo/CMakeFiles/yolo.dir/__/Common/HTTPData.cpp.o"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/Yolo && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/yolo.dir/__/Common/HTTPData.cpp.o -c /Users/tangrenchu/CLionProjects/TinySwarm/Common/HTTPData.cpp

Yolo/CMakeFiles/yolo.dir/__/Common/HTTPData.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/yolo.dir/__/Common/HTTPData.cpp.i"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/Yolo && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/tangrenchu/CLionProjects/TinySwarm/Common/HTTPData.cpp > CMakeFiles/yolo.dir/__/Common/HTTPData.cpp.i

Yolo/CMakeFiles/yolo.dir/__/Common/HTTPData.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/yolo.dir/__/Common/HTTPData.cpp.s"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/Yolo && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/tangrenchu/CLionProjects/TinySwarm/Common/HTTPData.cpp -o CMakeFiles/yolo.dir/__/Common/HTTPData.cpp.s

Yolo/CMakeFiles/yolo.dir/__/Common/IPAddress.cpp.o: Yolo/CMakeFiles/yolo.dir/flags.make
Yolo/CMakeFiles/yolo.dir/__/Common/IPAddress.cpp.o: ../Common/IPAddress.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object Yolo/CMakeFiles/yolo.dir/__/Common/IPAddress.cpp.o"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/Yolo && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/yolo.dir/__/Common/IPAddress.cpp.o -c /Users/tangrenchu/CLionProjects/TinySwarm/Common/IPAddress.cpp

Yolo/CMakeFiles/yolo.dir/__/Common/IPAddress.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/yolo.dir/__/Common/IPAddress.cpp.i"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/Yolo && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/tangrenchu/CLionProjects/TinySwarm/Common/IPAddress.cpp > CMakeFiles/yolo.dir/__/Common/IPAddress.cpp.i

Yolo/CMakeFiles/yolo.dir/__/Common/IPAddress.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/yolo.dir/__/Common/IPAddress.cpp.s"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/Yolo && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/tangrenchu/CLionProjects/TinySwarm/Common/IPAddress.cpp -o CMakeFiles/yolo.dir/__/Common/IPAddress.cpp.s

Yolo/CMakeFiles/yolo.dir/__/Common/MsgPack.pb.cc.o: Yolo/CMakeFiles/yolo.dir/flags.make
Yolo/CMakeFiles/yolo.dir/__/Common/MsgPack.pb.cc.o: ../Common/MsgPack.pb.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object Yolo/CMakeFiles/yolo.dir/__/Common/MsgPack.pb.cc.o"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/Yolo && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/yolo.dir/__/Common/MsgPack.pb.cc.o -c /Users/tangrenchu/CLionProjects/TinySwarm/Common/MsgPack.pb.cc

Yolo/CMakeFiles/yolo.dir/__/Common/MsgPack.pb.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/yolo.dir/__/Common/MsgPack.pb.cc.i"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/Yolo && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/tangrenchu/CLionProjects/TinySwarm/Common/MsgPack.pb.cc > CMakeFiles/yolo.dir/__/Common/MsgPack.pb.cc.i

Yolo/CMakeFiles/yolo.dir/__/Common/MsgPack.pb.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/yolo.dir/__/Common/MsgPack.pb.cc.s"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/Yolo && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/tangrenchu/CLionProjects/TinySwarm/Common/MsgPack.pb.cc -o CMakeFiles/yolo.dir/__/Common/MsgPack.pb.cc.s

Yolo/CMakeFiles/yolo.dir/__/Common/RPCServer.cpp.o: Yolo/CMakeFiles/yolo.dir/flags.make
Yolo/CMakeFiles/yolo.dir/__/Common/RPCServer.cpp.o: ../Common/RPCServer.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object Yolo/CMakeFiles/yolo.dir/__/Common/RPCServer.cpp.o"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/Yolo && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/yolo.dir/__/Common/RPCServer.cpp.o -c /Users/tangrenchu/CLionProjects/TinySwarm/Common/RPCServer.cpp

Yolo/CMakeFiles/yolo.dir/__/Common/RPCServer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/yolo.dir/__/Common/RPCServer.cpp.i"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/Yolo && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/tangrenchu/CLionProjects/TinySwarm/Common/RPCServer.cpp > CMakeFiles/yolo.dir/__/Common/RPCServer.cpp.i

Yolo/CMakeFiles/yolo.dir/__/Common/RPCServer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/yolo.dir/__/Common/RPCServer.cpp.s"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/Yolo && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/tangrenchu/CLionProjects/TinySwarm/Common/RPCServer.cpp -o CMakeFiles/yolo.dir/__/Common/RPCServer.cpp.s

Yolo/CMakeFiles/yolo.dir/__/LogUtils/LoggerControl.cpp.o: Yolo/CMakeFiles/yolo.dir/flags.make
Yolo/CMakeFiles/yolo.dir/__/LogUtils/LoggerControl.cpp.o: ../LogUtils/LoggerControl.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object Yolo/CMakeFiles/yolo.dir/__/LogUtils/LoggerControl.cpp.o"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/Yolo && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/yolo.dir/__/LogUtils/LoggerControl.cpp.o -c /Users/tangrenchu/CLionProjects/TinySwarm/LogUtils/LoggerControl.cpp

Yolo/CMakeFiles/yolo.dir/__/LogUtils/LoggerControl.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/yolo.dir/__/LogUtils/LoggerControl.cpp.i"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/Yolo && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/tangrenchu/CLionProjects/TinySwarm/LogUtils/LoggerControl.cpp > CMakeFiles/yolo.dir/__/LogUtils/LoggerControl.cpp.i

Yolo/CMakeFiles/yolo.dir/__/LogUtils/LoggerControl.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/yolo.dir/__/LogUtils/LoggerControl.cpp.s"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/Yolo && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/tangrenchu/CLionProjects/TinySwarm/LogUtils/LoggerControl.cpp -o CMakeFiles/yolo.dir/__/LogUtils/LoggerControl.cpp.s

Yolo/CMakeFiles/yolo.dir/YoloInfer.pb.cc.o: Yolo/CMakeFiles/yolo.dir/flags.make
Yolo/CMakeFiles/yolo.dir/YoloInfer.pb.cc.o: ../Yolo/YoloInfer.pb.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building CXX object Yolo/CMakeFiles/yolo.dir/YoloInfer.pb.cc.o"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/Yolo && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/yolo.dir/YoloInfer.pb.cc.o -c /Users/tangrenchu/CLionProjects/TinySwarm/Yolo/YoloInfer.pb.cc

Yolo/CMakeFiles/yolo.dir/YoloInfer.pb.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/yolo.dir/YoloInfer.pb.cc.i"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/Yolo && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/tangrenchu/CLionProjects/TinySwarm/Yolo/YoloInfer.pb.cc > CMakeFiles/yolo.dir/YoloInfer.pb.cc.i

Yolo/CMakeFiles/yolo.dir/YoloInfer.pb.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/yolo.dir/YoloInfer.pb.cc.s"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/Yolo && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/tangrenchu/CLionProjects/TinySwarm/Yolo/YoloInfer.pb.cc -o CMakeFiles/yolo.dir/YoloInfer.pb.cc.s

# Object files for target yolo
yolo_OBJECTS = \
"CMakeFiles/yolo.dir/main.cpp.o" \
"CMakeFiles/yolo.dir/RpcInterface.cpp.o" \
"CMakeFiles/yolo.dir/__/Common/HTTPData.cpp.o" \
"CMakeFiles/yolo.dir/__/Common/IPAddress.cpp.o" \
"CMakeFiles/yolo.dir/__/Common/MsgPack.pb.cc.o" \
"CMakeFiles/yolo.dir/__/Common/RPCServer.cpp.o" \
"CMakeFiles/yolo.dir/__/LogUtils/LoggerControl.cpp.o" \
"CMakeFiles/yolo.dir/YoloInfer.pb.cc.o"

# External object files for target yolo
yolo_EXTERNAL_OBJECTS =

Yolo/yolo: Yolo/CMakeFiles/yolo.dir/main.cpp.o
Yolo/yolo: Yolo/CMakeFiles/yolo.dir/RpcInterface.cpp.o
Yolo/yolo: Yolo/CMakeFiles/yolo.dir/__/Common/HTTPData.cpp.o
Yolo/yolo: Yolo/CMakeFiles/yolo.dir/__/Common/IPAddress.cpp.o
Yolo/yolo: Yolo/CMakeFiles/yolo.dir/__/Common/MsgPack.pb.cc.o
Yolo/yolo: Yolo/CMakeFiles/yolo.dir/__/Common/RPCServer.cpp.o
Yolo/yolo: Yolo/CMakeFiles/yolo.dir/__/LogUtils/LoggerControl.cpp.o
Yolo/yolo: Yolo/CMakeFiles/yolo.dir/YoloInfer.pb.cc.o
Yolo/yolo: Yolo/CMakeFiles/yolo.dir/build.make
Yolo/yolo: Yolo/CMakeFiles/yolo.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Linking CXX executable yolo"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/Yolo && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/yolo.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
Yolo/CMakeFiles/yolo.dir/build: Yolo/yolo
.PHONY : Yolo/CMakeFiles/yolo.dir/build

Yolo/CMakeFiles/yolo.dir/clean:
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/Yolo && $(CMAKE_COMMAND) -P CMakeFiles/yolo.dir/cmake_clean.cmake
.PHONY : Yolo/CMakeFiles/yolo.dir/clean

Yolo/CMakeFiles/yolo.dir/depend:
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/tangrenchu/CLionProjects/TinySwarm /Users/tangrenchu/CLionProjects/TinySwarm/Yolo /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/Yolo /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/Yolo/CMakeFiles/yolo.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : Yolo/CMakeFiles/yolo.dir/depend
