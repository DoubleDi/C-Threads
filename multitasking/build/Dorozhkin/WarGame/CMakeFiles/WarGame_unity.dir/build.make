# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.7

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
CMAKE_COMMAND = /usr/local/Cellar/cmake/3.7.2/bin/cmake

# The command to remove a file.
RM = /usr/local/Cellar/cmake/3.7.2/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/d.dorogkin/Documents/cmc/cpp_spring_2017/msu_cpp_spring_2017

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/d.dorogkin/Documents/cmc/cpp_spring_2017/msu_cpp_spring_2017/build

# Include any dependencies generated for this target.
include Dorozhkin/WarGame/CMakeFiles/WarGame_unity.dir/depend.make

# Include the progress variables for this target.
include Dorozhkin/WarGame/CMakeFiles/WarGame_unity.dir/progress.make

# Include the compile flags for this target's objects.
include Dorozhkin/WarGame/CMakeFiles/WarGame_unity.dir/flags.make

Dorozhkin/WarGame/cotire/WarGame_CXX_prefix.hxx: Dorozhkin/WarGame/cotire/WarGame_CXX_prefix.cxx
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/Users/d.dorogkin/Documents/cmc/cpp_spring_2017/msu_cpp_spring_2017/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Generating CXX prefix header Dorozhkin/WarGame/cotire/WarGame_CXX_prefix.hxx"
	/usr/local/Cellar/cmake/3.7.2/bin/cmake -DCOTIRE_BUILD_TYPE:STRING= -DCOTIRE_VERBOSE:BOOL=$(VERBOSE) -P /Users/d.dorogkin/Documents/cmc/cpp_spring_2017/msu_cpp_spring_2017/dependencies/cotire-1.7.9/CMake/cotire.cmake combine /Users/d.dorogkin/Documents/cmc/cpp_spring_2017/msu_cpp_spring_2017/build/Dorozhkin/WarGame/WarGame_CXX_cotire.cmake /Users/d.dorogkin/Documents/cmc/cpp_spring_2017/msu_cpp_spring_2017/build/Dorozhkin/WarGame/cotire/WarGame_CXX_prefix.hxx /Users/d.dorogkin/Documents/cmc/cpp_spring_2017/msu_cpp_spring_2017/build/Dorozhkin/WarGame/cotire/WarGame_CXX_prefix.cxx

Dorozhkin/WarGame/cotire/WarGame_CXX_unity.cxx: Dorozhkin/WarGame/WarGame_CXX_cotire.cmake
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/Users/d.dorogkin/Documents/cmc/cpp_spring_2017/msu_cpp_spring_2017/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Generating CXX unity source Dorozhkin/WarGame/cotire/WarGame_CXX_unity.cxx"
	cd /Users/d.dorogkin/Documents/cmc/cpp_spring_2017/msu_cpp_spring_2017/Dorozhkin/WarGame && /usr/local/Cellar/cmake/3.7.2/bin/cmake -DCOTIRE_BUILD_TYPE:STRING= -DCOTIRE_VERBOSE:BOOL=$(VERBOSE) -P /Users/d.dorogkin/Documents/cmc/cpp_spring_2017/msu_cpp_spring_2017/dependencies/cotire-1.7.9/CMake/cotire.cmake unity /Users/d.dorogkin/Documents/cmc/cpp_spring_2017/msu_cpp_spring_2017/build/Dorozhkin/WarGame/WarGame_CXX_cotire.cmake /Users/d.dorogkin/Documents/cmc/cpp_spring_2017/msu_cpp_spring_2017/build/Dorozhkin/WarGame/cotire/WarGame_CXX_unity.cxx

Dorozhkin/WarGame/cotire/WarGame_CXX_prefix.cxx: ../src/stdafx.h
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/Users/d.dorogkin/Documents/cmc/cpp_spring_2017/msu_cpp_spring_2017/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Generating CXX prefix source Dorozhkin/WarGame/cotire/WarGame_CXX_prefix.cxx"
	/usr/local/Cellar/cmake/3.7.2/bin/cmake -DCOTIRE_BUILD_TYPE:STRING= -DCOTIRE_VERBOSE:BOOL=$(VERBOSE) -P /Users/d.dorogkin/Documents/cmc/cpp_spring_2017/msu_cpp_spring_2017/dependencies/cotire-1.7.9/CMake/cotire.cmake combine /Users/d.dorogkin/Documents/cmc/cpp_spring_2017/msu_cpp_spring_2017/build/Dorozhkin/WarGame/WarGame_CXX_cotire.cmake /Users/d.dorogkin/Documents/cmc/cpp_spring_2017/msu_cpp_spring_2017/build/Dorozhkin/WarGame/cotire/WarGame_CXX_prefix.cxx /Users/d.dorogkin/Documents/cmc/cpp_spring_2017/msu_cpp_spring_2017/src/stdafx.h

Dorozhkin/WarGame/CMakeFiles/WarGame_unity.dir/cotire/WarGame_CXX_unity.cxx.o: Dorozhkin/WarGame/CMakeFiles/WarGame_unity.dir/flags.make
Dorozhkin/WarGame/CMakeFiles/WarGame_unity.dir/cotire/WarGame_CXX_unity.cxx.o: Dorozhkin/WarGame/cotire/WarGame_CXX_unity.cxx
Dorozhkin/WarGame/CMakeFiles/WarGame_unity.dir/cotire/WarGame_CXX_unity.cxx.o: Dorozhkin/WarGame/cotire/WarGame_CXX_prefix.hxx
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/d.dorogkin/Documents/cmc/cpp_spring_2017/msu_cpp_spring_2017/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object Dorozhkin/WarGame/CMakeFiles/WarGame_unity.dir/cotire/WarGame_CXX_unity.cxx.o"
	cd /Users/d.dorogkin/Documents/cmc/cpp_spring_2017/msu_cpp_spring_2017/build/Dorozhkin/WarGame && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS)  -Qunused-arguments -include "/Users/d.dorogkin/Documents/cmc/cpp_spring_2017/msu_cpp_spring_2017/build/Dorozhkin/WarGame/cotire/WarGame_CXX_prefix.hxx"  -o CMakeFiles/WarGame_unity.dir/cotire/WarGame_CXX_unity.cxx.o -c /Users/d.dorogkin/Documents/cmc/cpp_spring_2017/msu_cpp_spring_2017/build/Dorozhkin/WarGame/cotire/WarGame_CXX_unity.cxx

Dorozhkin/WarGame/CMakeFiles/WarGame_unity.dir/cotire/WarGame_CXX_unity.cxx.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/WarGame_unity.dir/cotire/WarGame_CXX_unity.cxx.i"
	cd /Users/d.dorogkin/Documents/cmc/cpp_spring_2017/msu_cpp_spring_2017/build/Dorozhkin/WarGame && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS)  -Qunused-arguments -include "/Users/d.dorogkin/Documents/cmc/cpp_spring_2017/msu_cpp_spring_2017/build/Dorozhkin/WarGame/cotire/WarGame_CXX_prefix.hxx"  -E /Users/d.dorogkin/Documents/cmc/cpp_spring_2017/msu_cpp_spring_2017/build/Dorozhkin/WarGame/cotire/WarGame_CXX_unity.cxx > CMakeFiles/WarGame_unity.dir/cotire/WarGame_CXX_unity.cxx.i

Dorozhkin/WarGame/CMakeFiles/WarGame_unity.dir/cotire/WarGame_CXX_unity.cxx.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/WarGame_unity.dir/cotire/WarGame_CXX_unity.cxx.s"
	cd /Users/d.dorogkin/Documents/cmc/cpp_spring_2017/msu_cpp_spring_2017/build/Dorozhkin/WarGame && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS)  -Qunused-arguments -include "/Users/d.dorogkin/Documents/cmc/cpp_spring_2017/msu_cpp_spring_2017/build/Dorozhkin/WarGame/cotire/WarGame_CXX_prefix.hxx"  -S /Users/d.dorogkin/Documents/cmc/cpp_spring_2017/msu_cpp_spring_2017/build/Dorozhkin/WarGame/cotire/WarGame_CXX_unity.cxx -o CMakeFiles/WarGame_unity.dir/cotire/WarGame_CXX_unity.cxx.s

Dorozhkin/WarGame/CMakeFiles/WarGame_unity.dir/cotire/WarGame_CXX_unity.cxx.o.requires:

.PHONY : Dorozhkin/WarGame/CMakeFiles/WarGame_unity.dir/cotire/WarGame_CXX_unity.cxx.o.requires

Dorozhkin/WarGame/CMakeFiles/WarGame_unity.dir/cotire/WarGame_CXX_unity.cxx.o.provides: Dorozhkin/WarGame/CMakeFiles/WarGame_unity.dir/cotire/WarGame_CXX_unity.cxx.o.requires
	$(MAKE) -f Dorozhkin/WarGame/CMakeFiles/WarGame_unity.dir/build.make Dorozhkin/WarGame/CMakeFiles/WarGame_unity.dir/cotire/WarGame_CXX_unity.cxx.o.provides.build
.PHONY : Dorozhkin/WarGame/CMakeFiles/WarGame_unity.dir/cotire/WarGame_CXX_unity.cxx.o.provides

Dorozhkin/WarGame/CMakeFiles/WarGame_unity.dir/cotire/WarGame_CXX_unity.cxx.o.provides.build: Dorozhkin/WarGame/CMakeFiles/WarGame_unity.dir/cotire/WarGame_CXX_unity.cxx.o


# Object files for target WarGame_unity
WarGame_unity_OBJECTS = \
"CMakeFiles/WarGame_unity.dir/cotire/WarGame_CXX_unity.cxx.o"

# External object files for target WarGame_unity
WarGame_unity_EXTERNAL_OBJECTS =

Dorozhkin/WarGame/WarGame: Dorozhkin/WarGame/CMakeFiles/WarGame_unity.dir/cotire/WarGame_CXX_unity.cxx.o
Dorozhkin/WarGame/WarGame: Dorozhkin/WarGame/CMakeFiles/WarGame_unity.dir/build.make
Dorozhkin/WarGame/WarGame: Dorozhkin/WarGame/CMakeFiles/WarGame_unity.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/d.dorogkin/Documents/cmc/cpp_spring_2017/msu_cpp_spring_2017/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Linking CXX executable WarGame"
	cd /Users/d.dorogkin/Documents/cmc/cpp_spring_2017/msu_cpp_spring_2017/build/Dorozhkin/WarGame && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/WarGame_unity.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
Dorozhkin/WarGame/CMakeFiles/WarGame_unity.dir/build: Dorozhkin/WarGame/WarGame

.PHONY : Dorozhkin/WarGame/CMakeFiles/WarGame_unity.dir/build

Dorozhkin/WarGame/CMakeFiles/WarGame_unity.dir/requires: Dorozhkin/WarGame/CMakeFiles/WarGame_unity.dir/cotire/WarGame_CXX_unity.cxx.o.requires

.PHONY : Dorozhkin/WarGame/CMakeFiles/WarGame_unity.dir/requires

Dorozhkin/WarGame/CMakeFiles/WarGame_unity.dir/clean:
	cd /Users/d.dorogkin/Documents/cmc/cpp_spring_2017/msu_cpp_spring_2017/build/Dorozhkin/WarGame && $(CMAKE_COMMAND) -P CMakeFiles/WarGame_unity.dir/cmake_clean.cmake
.PHONY : Dorozhkin/WarGame/CMakeFiles/WarGame_unity.dir/clean

Dorozhkin/WarGame/CMakeFiles/WarGame_unity.dir/depend: Dorozhkin/WarGame/cotire/WarGame_CXX_prefix.hxx
Dorozhkin/WarGame/CMakeFiles/WarGame_unity.dir/depend: Dorozhkin/WarGame/cotire/WarGame_CXX_unity.cxx
Dorozhkin/WarGame/CMakeFiles/WarGame_unity.dir/depend: Dorozhkin/WarGame/cotire/WarGame_CXX_prefix.cxx
	cd /Users/d.dorogkin/Documents/cmc/cpp_spring_2017/msu_cpp_spring_2017/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/d.dorogkin/Documents/cmc/cpp_spring_2017/msu_cpp_spring_2017 /Users/d.dorogkin/Documents/cmc/cpp_spring_2017/msu_cpp_spring_2017/Dorozhkin/WarGame /Users/d.dorogkin/Documents/cmc/cpp_spring_2017/msu_cpp_spring_2017/build /Users/d.dorogkin/Documents/cmc/cpp_spring_2017/msu_cpp_spring_2017/build/Dorozhkin/WarGame /Users/d.dorogkin/Documents/cmc/cpp_spring_2017/msu_cpp_spring_2017/build/Dorozhkin/WarGame/CMakeFiles/WarGame_unity.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : Dorozhkin/WarGame/CMakeFiles/WarGame_unity.dir/depend

