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
include manager/CMakeFiles/manager.dir/depend.make
# Include the progress variables for this target.
include manager/CMakeFiles/manager.dir/progress.make

# Include the compile flags for this target's objects.
include manager/CMakeFiles/manager.dir/flags.make

manager/CMakeFiles/manager.dir/main.cpp.o: manager/CMakeFiles/manager.dir/flags.make
manager/CMakeFiles/manager.dir/main.cpp.o: ../manager/main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object manager/CMakeFiles/manager.dir/main.cpp.o"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/manager && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/manager.dir/main.cpp.o -c /Users/tangrenchu/CLionProjects/TinySwarm/manager/main.cpp

manager/CMakeFiles/manager.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/manager.dir/main.cpp.i"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/manager && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/tangrenchu/CLionProjects/TinySwarm/manager/main.cpp > CMakeFiles/manager.dir/main.cpp.i

manager/CMakeFiles/manager.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/manager.dir/main.cpp.s"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/manager && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/tangrenchu/CLionProjects/TinySwarm/manager/main.cpp -o CMakeFiles/manager.dir/main.cpp.s

manager/CMakeFiles/manager.dir/Manager.cpp.o: manager/CMakeFiles/manager.dir/flags.make
manager/CMakeFiles/manager.dir/Manager.cpp.o: ../manager/Manager.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object manager/CMakeFiles/manager.dir/Manager.cpp.o"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/manager && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/manager.dir/Manager.cpp.o -c /Users/tangrenchu/CLionProjects/TinySwarm/manager/Manager.cpp

manager/CMakeFiles/manager.dir/Manager.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/manager.dir/Manager.cpp.i"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/manager && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/tangrenchu/CLionProjects/TinySwarm/manager/Manager.cpp > CMakeFiles/manager.dir/Manager.cpp.i

manager/CMakeFiles/manager.dir/Manager.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/manager.dir/Manager.cpp.s"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/manager && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/tangrenchu/CLionProjects/TinySwarm/manager/Manager.cpp -o CMakeFiles/manager.dir/Manager.cpp.s

manager/CMakeFiles/manager.dir/WorkerRpcInterface.pb.cc.o: manager/CMakeFiles/manager.dir/flags.make
manager/CMakeFiles/manager.dir/WorkerRpcInterface.pb.cc.o: ../manager/WorkerRpcInterface.pb.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object manager/CMakeFiles/manager.dir/WorkerRpcInterface.pb.cc.o"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/manager && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/manager.dir/WorkerRpcInterface.pb.cc.o -c /Users/tangrenchu/CLionProjects/TinySwarm/manager/WorkerRpcInterface.pb.cc

manager/CMakeFiles/manager.dir/WorkerRpcInterface.pb.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/manager.dir/WorkerRpcInterface.pb.cc.i"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/manager && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/tangrenchu/CLionProjects/TinySwarm/manager/WorkerRpcInterface.pb.cc > CMakeFiles/manager.dir/WorkerRpcInterface.pb.cc.i

manager/CMakeFiles/manager.dir/WorkerRpcInterface.pb.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/manager.dir/WorkerRpcInterface.pb.cc.s"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/manager && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/tangrenchu/CLionProjects/TinySwarm/manager/WorkerRpcInterface.pb.cc -o CMakeFiles/manager.dir/WorkerRpcInterface.pb.cc.s

manager/CMakeFiles/manager.dir/__/Common/HTTPData.cpp.o: manager/CMakeFiles/manager.dir/flags.make
manager/CMakeFiles/manager.dir/__/Common/HTTPData.cpp.o: ../Common/HTTPData.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object manager/CMakeFiles/manager.dir/__/Common/HTTPData.cpp.o"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/manager && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/manager.dir/__/Common/HTTPData.cpp.o -c /Users/tangrenchu/CLionProjects/TinySwarm/Common/HTTPData.cpp

manager/CMakeFiles/manager.dir/__/Common/HTTPData.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/manager.dir/__/Common/HTTPData.cpp.i"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/manager && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/tangrenchu/CLionProjects/TinySwarm/Common/HTTPData.cpp > CMakeFiles/manager.dir/__/Common/HTTPData.cpp.i

manager/CMakeFiles/manager.dir/__/Common/HTTPData.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/manager.dir/__/Common/HTTPData.cpp.s"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/manager && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/tangrenchu/CLionProjects/TinySwarm/Common/HTTPData.cpp -o CMakeFiles/manager.dir/__/Common/HTTPData.cpp.s

manager/CMakeFiles/manager.dir/__/Common/IPAddress.cpp.o: manager/CMakeFiles/manager.dir/flags.make
manager/CMakeFiles/manager.dir/__/Common/IPAddress.cpp.o: ../Common/IPAddress.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object manager/CMakeFiles/manager.dir/__/Common/IPAddress.cpp.o"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/manager && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/manager.dir/__/Common/IPAddress.cpp.o -c /Users/tangrenchu/CLionProjects/TinySwarm/Common/IPAddress.cpp

manager/CMakeFiles/manager.dir/__/Common/IPAddress.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/manager.dir/__/Common/IPAddress.cpp.i"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/manager && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/tangrenchu/CLionProjects/TinySwarm/Common/IPAddress.cpp > CMakeFiles/manager.dir/__/Common/IPAddress.cpp.i

manager/CMakeFiles/manager.dir/__/Common/IPAddress.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/manager.dir/__/Common/IPAddress.cpp.s"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/manager && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/tangrenchu/CLionProjects/TinySwarm/Common/IPAddress.cpp -o CMakeFiles/manager.dir/__/Common/IPAddress.cpp.s

manager/CMakeFiles/manager.dir/__/Common/MsgPack.pb.cc.o: manager/CMakeFiles/manager.dir/flags.make
manager/CMakeFiles/manager.dir/__/Common/MsgPack.pb.cc.o: ../Common/MsgPack.pb.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object manager/CMakeFiles/manager.dir/__/Common/MsgPack.pb.cc.o"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/manager && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/manager.dir/__/Common/MsgPack.pb.cc.o -c /Users/tangrenchu/CLionProjects/TinySwarm/Common/MsgPack.pb.cc

manager/CMakeFiles/manager.dir/__/Common/MsgPack.pb.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/manager.dir/__/Common/MsgPack.pb.cc.i"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/manager && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/tangrenchu/CLionProjects/TinySwarm/Common/MsgPack.pb.cc > CMakeFiles/manager.dir/__/Common/MsgPack.pb.cc.i

manager/CMakeFiles/manager.dir/__/Common/MsgPack.pb.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/manager.dir/__/Common/MsgPack.pb.cc.s"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/manager && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/tangrenchu/CLionProjects/TinySwarm/Common/MsgPack.pb.cc -o CMakeFiles/manager.dir/__/Common/MsgPack.pb.cc.s

manager/CMakeFiles/manager.dir/__/Common/RPCServer.cpp.o: manager/CMakeFiles/manager.dir/flags.make
manager/CMakeFiles/manager.dir/__/Common/RPCServer.cpp.o: ../Common/RPCServer.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object manager/CMakeFiles/manager.dir/__/Common/RPCServer.cpp.o"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/manager && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/manager.dir/__/Common/RPCServer.cpp.o -c /Users/tangrenchu/CLionProjects/TinySwarm/Common/RPCServer.cpp

manager/CMakeFiles/manager.dir/__/Common/RPCServer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/manager.dir/__/Common/RPCServer.cpp.i"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/manager && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/tangrenchu/CLionProjects/TinySwarm/Common/RPCServer.cpp > CMakeFiles/manager.dir/__/Common/RPCServer.cpp.i

manager/CMakeFiles/manager.dir/__/Common/RPCServer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/manager.dir/__/Common/RPCServer.cpp.s"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/manager && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/tangrenchu/CLionProjects/TinySwarm/Common/RPCServer.cpp -o CMakeFiles/manager.dir/__/Common/RPCServer.cpp.s

manager/CMakeFiles/manager.dir/Proxy/NginxProxy.cpp.o: manager/CMakeFiles/manager.dir/flags.make
manager/CMakeFiles/manager.dir/Proxy/NginxProxy.cpp.o: ../manager/Proxy/NginxProxy.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building CXX object manager/CMakeFiles/manager.dir/Proxy/NginxProxy.cpp.o"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/manager && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/manager.dir/Proxy/NginxProxy.cpp.o -c /Users/tangrenchu/CLionProjects/TinySwarm/manager/Proxy/NginxProxy.cpp

manager/CMakeFiles/manager.dir/Proxy/NginxProxy.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/manager.dir/Proxy/NginxProxy.cpp.i"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/manager && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/tangrenchu/CLionProjects/TinySwarm/manager/Proxy/NginxProxy.cpp > CMakeFiles/manager.dir/Proxy/NginxProxy.cpp.i

manager/CMakeFiles/manager.dir/Proxy/NginxProxy.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/manager.dir/Proxy/NginxProxy.cpp.s"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/manager && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/tangrenchu/CLionProjects/TinySwarm/manager/Proxy/NginxProxy.cpp -o CMakeFiles/manager.dir/Proxy/NginxProxy.cpp.s

manager/CMakeFiles/manager.dir/Proxy/Proxy.cpp.o: manager/CMakeFiles/manager.dir/flags.make
manager/CMakeFiles/manager.dir/Proxy/Proxy.cpp.o: ../manager/Proxy/Proxy.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building CXX object manager/CMakeFiles/manager.dir/Proxy/Proxy.cpp.o"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/manager && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/manager.dir/Proxy/Proxy.cpp.o -c /Users/tangrenchu/CLionProjects/TinySwarm/manager/Proxy/Proxy.cpp

manager/CMakeFiles/manager.dir/Proxy/Proxy.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/manager.dir/Proxy/Proxy.cpp.i"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/manager && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/tangrenchu/CLionProjects/TinySwarm/manager/Proxy/Proxy.cpp > CMakeFiles/manager.dir/Proxy/Proxy.cpp.i

manager/CMakeFiles/manager.dir/Proxy/Proxy.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/manager.dir/Proxy/Proxy.cpp.s"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/manager && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/tangrenchu/CLionProjects/TinySwarm/manager/Proxy/Proxy.cpp -o CMakeFiles/manager.dir/Proxy/Proxy.cpp.s

manager/CMakeFiles/manager.dir/__/LogUtils/LoggerControl.cpp.o: manager/CMakeFiles/manager.dir/flags.make
manager/CMakeFiles/manager.dir/__/LogUtils/LoggerControl.cpp.o: ../LogUtils/LoggerControl.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Building CXX object manager/CMakeFiles/manager.dir/__/LogUtils/LoggerControl.cpp.o"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/manager && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/manager.dir/__/LogUtils/LoggerControl.cpp.o -c /Users/tangrenchu/CLionProjects/TinySwarm/LogUtils/LoggerControl.cpp

manager/CMakeFiles/manager.dir/__/LogUtils/LoggerControl.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/manager.dir/__/LogUtils/LoggerControl.cpp.i"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/manager && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/tangrenchu/CLionProjects/TinySwarm/LogUtils/LoggerControl.cpp > CMakeFiles/manager.dir/__/LogUtils/LoggerControl.cpp.i

manager/CMakeFiles/manager.dir/__/LogUtils/LoggerControl.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/manager.dir/__/LogUtils/LoggerControl.cpp.s"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/manager && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/tangrenchu/CLionProjects/TinySwarm/LogUtils/LoggerControl.cpp -o CMakeFiles/manager.dir/__/LogUtils/LoggerControl.cpp.s

manager/CMakeFiles/manager.dir/ManagerRpcInterface.pb.cc.o: manager/CMakeFiles/manager.dir/flags.make
manager/CMakeFiles/manager.dir/ManagerRpcInterface.pb.cc.o: ../manager/ManagerRpcInterface.pb.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_11) "Building CXX object manager/CMakeFiles/manager.dir/ManagerRpcInterface.pb.cc.o"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/manager && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/manager.dir/ManagerRpcInterface.pb.cc.o -c /Users/tangrenchu/CLionProjects/TinySwarm/manager/ManagerRpcInterface.pb.cc

manager/CMakeFiles/manager.dir/ManagerRpcInterface.pb.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/manager.dir/ManagerRpcInterface.pb.cc.i"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/manager && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/tangrenchu/CLionProjects/TinySwarm/manager/ManagerRpcInterface.pb.cc > CMakeFiles/manager.dir/ManagerRpcInterface.pb.cc.i

manager/CMakeFiles/manager.dir/ManagerRpcInterface.pb.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/manager.dir/ManagerRpcInterface.pb.cc.s"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/manager && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/tangrenchu/CLionProjects/TinySwarm/manager/ManagerRpcInterface.pb.cc -o CMakeFiles/manager.dir/ManagerRpcInterface.pb.cc.s

manager/CMakeFiles/manager.dir/LogQueue.cpp.o: manager/CMakeFiles/manager.dir/flags.make
manager/CMakeFiles/manager.dir/LogQueue.cpp.o: ../manager/LogQueue.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_12) "Building CXX object manager/CMakeFiles/manager.dir/LogQueue.cpp.o"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/manager && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/manager.dir/LogQueue.cpp.o -c /Users/tangrenchu/CLionProjects/TinySwarm/manager/LogQueue.cpp

manager/CMakeFiles/manager.dir/LogQueue.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/manager.dir/LogQueue.cpp.i"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/manager && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/tangrenchu/CLionProjects/TinySwarm/manager/LogQueue.cpp > CMakeFiles/manager.dir/LogQueue.cpp.i

manager/CMakeFiles/manager.dir/LogQueue.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/manager.dir/LogQueue.cpp.s"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/manager && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/tangrenchu/CLionProjects/TinySwarm/manager/LogQueue.cpp -o CMakeFiles/manager.dir/LogQueue.cpp.s

manager/CMakeFiles/manager.dir/RPCInterface.cpp.o: manager/CMakeFiles/manager.dir/flags.make
manager/CMakeFiles/manager.dir/RPCInterface.cpp.o: ../manager/RPCInterface.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_13) "Building CXX object manager/CMakeFiles/manager.dir/RPCInterface.cpp.o"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/manager && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/manager.dir/RPCInterface.cpp.o -c /Users/tangrenchu/CLionProjects/TinySwarm/manager/RPCInterface.cpp

manager/CMakeFiles/manager.dir/RPCInterface.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/manager.dir/RPCInterface.cpp.i"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/manager && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/tangrenchu/CLionProjects/TinySwarm/manager/RPCInterface.cpp > CMakeFiles/manager.dir/RPCInterface.cpp.i

manager/CMakeFiles/manager.dir/RPCInterface.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/manager.dir/RPCInterface.cpp.s"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/manager && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/tangrenchu/CLionProjects/TinySwarm/manager/RPCInterface.cpp -o CMakeFiles/manager.dir/RPCInterface.cpp.s

manager/CMakeFiles/manager.dir/PodDescriptor.cpp.o: manager/CMakeFiles/manager.dir/flags.make
manager/CMakeFiles/manager.dir/PodDescriptor.cpp.o: ../manager/PodDescriptor.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_14) "Building CXX object manager/CMakeFiles/manager.dir/PodDescriptor.cpp.o"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/manager && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/manager.dir/PodDescriptor.cpp.o -c /Users/tangrenchu/CLionProjects/TinySwarm/manager/PodDescriptor.cpp

manager/CMakeFiles/manager.dir/PodDescriptor.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/manager.dir/PodDescriptor.cpp.i"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/manager && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/tangrenchu/CLionProjects/TinySwarm/manager/PodDescriptor.cpp > CMakeFiles/manager.dir/PodDescriptor.cpp.i

manager/CMakeFiles/manager.dir/PodDescriptor.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/manager.dir/PodDescriptor.cpp.s"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/manager && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/tangrenchu/CLionProjects/TinySwarm/manager/PodDescriptor.cpp -o CMakeFiles/manager.dir/PodDescriptor.cpp.s

manager/CMakeFiles/manager.dir/ServiceManager.cpp.o: manager/CMakeFiles/manager.dir/flags.make
manager/CMakeFiles/manager.dir/ServiceManager.cpp.o: ../manager/ServiceManager.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_15) "Building CXX object manager/CMakeFiles/manager.dir/ServiceManager.cpp.o"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/manager && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/manager.dir/ServiceManager.cpp.o -c /Users/tangrenchu/CLionProjects/TinySwarm/manager/ServiceManager.cpp

manager/CMakeFiles/manager.dir/ServiceManager.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/manager.dir/ServiceManager.cpp.i"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/manager && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/tangrenchu/CLionProjects/TinySwarm/manager/ServiceManager.cpp > CMakeFiles/manager.dir/ServiceManager.cpp.i

manager/CMakeFiles/manager.dir/ServiceManager.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/manager.dir/ServiceManager.cpp.s"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/manager && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/tangrenchu/CLionProjects/TinySwarm/manager/ServiceManager.cpp -o CMakeFiles/manager.dir/ServiceManager.cpp.s

# Object files for target manager
manager_OBJECTS = \
"CMakeFiles/manager.dir/main.cpp.o" \
"CMakeFiles/manager.dir/Manager.cpp.o" \
"CMakeFiles/manager.dir/WorkerRpcInterface.pb.cc.o" \
"CMakeFiles/manager.dir/__/Common/HTTPData.cpp.o" \
"CMakeFiles/manager.dir/__/Common/IPAddress.cpp.o" \
"CMakeFiles/manager.dir/__/Common/MsgPack.pb.cc.o" \
"CMakeFiles/manager.dir/__/Common/RPCServer.cpp.o" \
"CMakeFiles/manager.dir/Proxy/NginxProxy.cpp.o" \
"CMakeFiles/manager.dir/Proxy/Proxy.cpp.o" \
"CMakeFiles/manager.dir/__/LogUtils/LoggerControl.cpp.o" \
"CMakeFiles/manager.dir/ManagerRpcInterface.pb.cc.o" \
"CMakeFiles/manager.dir/LogQueue.cpp.o" \
"CMakeFiles/manager.dir/RPCInterface.cpp.o" \
"CMakeFiles/manager.dir/PodDescriptor.cpp.o" \
"CMakeFiles/manager.dir/ServiceManager.cpp.o"

# External object files for target manager
manager_EXTERNAL_OBJECTS =

manager/manager: manager/CMakeFiles/manager.dir/main.cpp.o
manager/manager: manager/CMakeFiles/manager.dir/Manager.cpp.o
manager/manager: manager/CMakeFiles/manager.dir/WorkerRpcInterface.pb.cc.o
manager/manager: manager/CMakeFiles/manager.dir/__/Common/HTTPData.cpp.o
manager/manager: manager/CMakeFiles/manager.dir/__/Common/IPAddress.cpp.o
manager/manager: manager/CMakeFiles/manager.dir/__/Common/MsgPack.pb.cc.o
manager/manager: manager/CMakeFiles/manager.dir/__/Common/RPCServer.cpp.o
manager/manager: manager/CMakeFiles/manager.dir/Proxy/NginxProxy.cpp.o
manager/manager: manager/CMakeFiles/manager.dir/Proxy/Proxy.cpp.o
manager/manager: manager/CMakeFiles/manager.dir/__/LogUtils/LoggerControl.cpp.o
manager/manager: manager/CMakeFiles/manager.dir/ManagerRpcInterface.pb.cc.o
manager/manager: manager/CMakeFiles/manager.dir/LogQueue.cpp.o
manager/manager: manager/CMakeFiles/manager.dir/RPCInterface.cpp.o
manager/manager: manager/CMakeFiles/manager.dir/PodDescriptor.cpp.o
manager/manager: manager/CMakeFiles/manager.dir/ServiceManager.cpp.o
manager/manager: manager/CMakeFiles/manager.dir/build.make
manager/manager: manager/CMakeFiles/manager.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_16) "Linking CXX executable manager"
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/manager && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/manager.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
manager/CMakeFiles/manager.dir/build: manager/manager
.PHONY : manager/CMakeFiles/manager.dir/build

manager/CMakeFiles/manager.dir/clean:
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/manager && $(CMAKE_COMMAND) -P CMakeFiles/manager.dir/cmake_clean.cmake
.PHONY : manager/CMakeFiles/manager.dir/clean

manager/CMakeFiles/manager.dir/depend:
	cd /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/tangrenchu/CLionProjects/TinySwarm /Users/tangrenchu/CLionProjects/TinySwarm/manager /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/manager /Users/tangrenchu/CLionProjects/TinySwarm/cmake-build-debug/manager/CMakeFiles/manager.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : manager/CMakeFiles/manager.dir/depend
