# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

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
CMAKE_SOURCE_DIR = /home/rsc/Desktop/Druckwelle

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/rsc/Desktop/Druckwelle

# Include any dependencies generated for this target.
include src/CMakeFiles/druckwelle.dir/depend.make

# Include the progress variables for this target.
include src/CMakeFiles/druckwelle.dir/progress.make

# Include the compile flags for this target's objects.
include src/CMakeFiles/druckwelle.dir/flags.make

src/CMakeFiles/druckwelle.dir/layers/ExampleLayer.cpp.o: src/CMakeFiles/druckwelle.dir/flags.make
src/CMakeFiles/druckwelle.dir/layers/ExampleLayer.cpp.o: src/layers/ExampleLayer.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/rsc/Desktop/Druckwelle/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object src/CMakeFiles/druckwelle.dir/layers/ExampleLayer.cpp.o"
	cd /home/rsc/Desktop/Druckwelle/src && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/druckwelle.dir/layers/ExampleLayer.cpp.o -c /home/rsc/Desktop/Druckwelle/src/layers/ExampleLayer.cpp

src/CMakeFiles/druckwelle.dir/layers/ExampleLayer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/druckwelle.dir/layers/ExampleLayer.cpp.i"
	cd /home/rsc/Desktop/Druckwelle/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/rsc/Desktop/Druckwelle/src/layers/ExampleLayer.cpp > CMakeFiles/druckwelle.dir/layers/ExampleLayer.cpp.i

src/CMakeFiles/druckwelle.dir/layers/ExampleLayer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/druckwelle.dir/layers/ExampleLayer.cpp.s"
	cd /home/rsc/Desktop/Druckwelle/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/rsc/Desktop/Druckwelle/src/layers/ExampleLayer.cpp -o CMakeFiles/druckwelle.dir/layers/ExampleLayer.cpp.s

src/CMakeFiles/druckwelle.dir/layers/ExampleLayer.cpp.o.requires:
.PHONY : src/CMakeFiles/druckwelle.dir/layers/ExampleLayer.cpp.o.requires

src/CMakeFiles/druckwelle.dir/layers/ExampleLayer.cpp.o.provides: src/CMakeFiles/druckwelle.dir/layers/ExampleLayer.cpp.o.requires
	$(MAKE) -f src/CMakeFiles/druckwelle.dir/build.make src/CMakeFiles/druckwelle.dir/layers/ExampleLayer.cpp.o.provides.build
.PHONY : src/CMakeFiles/druckwelle.dir/layers/ExampleLayer.cpp.o.provides

src/CMakeFiles/druckwelle.dir/layers/ExampleLayer.cpp.o.provides.build: src/CMakeFiles/druckwelle.dir/layers/ExampleLayer.cpp.o

src/CMakeFiles/druckwelle.dir/layers/QualityElevation.cpp.o: src/CMakeFiles/druckwelle.dir/flags.make
src/CMakeFiles/druckwelle.dir/layers/QualityElevation.cpp.o: src/layers/QualityElevation.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/rsc/Desktop/Druckwelle/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object src/CMakeFiles/druckwelle.dir/layers/QualityElevation.cpp.o"
	cd /home/rsc/Desktop/Druckwelle/src && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/druckwelle.dir/layers/QualityElevation.cpp.o -c /home/rsc/Desktop/Druckwelle/src/layers/QualityElevation.cpp

src/CMakeFiles/druckwelle.dir/layers/QualityElevation.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/druckwelle.dir/layers/QualityElevation.cpp.i"
	cd /home/rsc/Desktop/Druckwelle/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/rsc/Desktop/Druckwelle/src/layers/QualityElevation.cpp > CMakeFiles/druckwelle.dir/layers/QualityElevation.cpp.i

src/CMakeFiles/druckwelle.dir/layers/QualityElevation.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/druckwelle.dir/layers/QualityElevation.cpp.s"
	cd /home/rsc/Desktop/Druckwelle/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/rsc/Desktop/Druckwelle/src/layers/QualityElevation.cpp -o CMakeFiles/druckwelle.dir/layers/QualityElevation.cpp.s

src/CMakeFiles/druckwelle.dir/layers/QualityElevation.cpp.o.requires:
.PHONY : src/CMakeFiles/druckwelle.dir/layers/QualityElevation.cpp.o.requires

src/CMakeFiles/druckwelle.dir/layers/QualityElevation.cpp.o.provides: src/CMakeFiles/druckwelle.dir/layers/QualityElevation.cpp.o.requires
	$(MAKE) -f src/CMakeFiles/druckwelle.dir/build.make src/CMakeFiles/druckwelle.dir/layers/QualityElevation.cpp.o.provides.build
.PHONY : src/CMakeFiles/druckwelle.dir/layers/QualityElevation.cpp.o.provides

src/CMakeFiles/druckwelle.dir/layers/QualityElevation.cpp.o.provides.build: src/CMakeFiles/druckwelle.dir/layers/QualityElevation.cpp.o

src/CMakeFiles/druckwelle.dir/layers/TileCache.cpp.o: src/CMakeFiles/druckwelle.dir/flags.make
src/CMakeFiles/druckwelle.dir/layers/TileCache.cpp.o: src/layers/TileCache.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/rsc/Desktop/Druckwelle/CMakeFiles $(CMAKE_PROGRESS_3)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object src/CMakeFiles/druckwelle.dir/layers/TileCache.cpp.o"
	cd /home/rsc/Desktop/Druckwelle/src && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/druckwelle.dir/layers/TileCache.cpp.o -c /home/rsc/Desktop/Druckwelle/src/layers/TileCache.cpp

src/CMakeFiles/druckwelle.dir/layers/TileCache.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/druckwelle.dir/layers/TileCache.cpp.i"
	cd /home/rsc/Desktop/Druckwelle/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/rsc/Desktop/Druckwelle/src/layers/TileCache.cpp > CMakeFiles/druckwelle.dir/layers/TileCache.cpp.i

src/CMakeFiles/druckwelle.dir/layers/TileCache.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/druckwelle.dir/layers/TileCache.cpp.s"
	cd /home/rsc/Desktop/Druckwelle/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/rsc/Desktop/Druckwelle/src/layers/TileCache.cpp -o CMakeFiles/druckwelle.dir/layers/TileCache.cpp.s

src/CMakeFiles/druckwelle.dir/layers/TileCache.cpp.o.requires:
.PHONY : src/CMakeFiles/druckwelle.dir/layers/TileCache.cpp.o.requires

src/CMakeFiles/druckwelle.dir/layers/TileCache.cpp.o.provides: src/CMakeFiles/druckwelle.dir/layers/TileCache.cpp.o.requires
	$(MAKE) -f src/CMakeFiles/druckwelle.dir/build.make src/CMakeFiles/druckwelle.dir/layers/TileCache.cpp.o.provides.build
.PHONY : src/CMakeFiles/druckwelle.dir/layers/TileCache.cpp.o.provides

src/CMakeFiles/druckwelle.dir/layers/TileCache.cpp.o.provides.build: src/CMakeFiles/druckwelle.dir/layers/TileCache.cpp.o

src/CMakeFiles/druckwelle.dir/WebMapService.cpp.o: src/CMakeFiles/druckwelle.dir/flags.make
src/CMakeFiles/druckwelle.dir/WebMapService.cpp.o: src/WebMapService.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/rsc/Desktop/Druckwelle/CMakeFiles $(CMAKE_PROGRESS_4)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object src/CMakeFiles/druckwelle.dir/WebMapService.cpp.o"
	cd /home/rsc/Desktop/Druckwelle/src && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/druckwelle.dir/WebMapService.cpp.o -c /home/rsc/Desktop/Druckwelle/src/WebMapService.cpp

src/CMakeFiles/druckwelle.dir/WebMapService.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/druckwelle.dir/WebMapService.cpp.i"
	cd /home/rsc/Desktop/Druckwelle/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/rsc/Desktop/Druckwelle/src/WebMapService.cpp > CMakeFiles/druckwelle.dir/WebMapService.cpp.i

src/CMakeFiles/druckwelle.dir/WebMapService.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/druckwelle.dir/WebMapService.cpp.s"
	cd /home/rsc/Desktop/Druckwelle/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/rsc/Desktop/Druckwelle/src/WebMapService.cpp -o CMakeFiles/druckwelle.dir/WebMapService.cpp.s

src/CMakeFiles/druckwelle.dir/WebMapService.cpp.o.requires:
.PHONY : src/CMakeFiles/druckwelle.dir/WebMapService.cpp.o.requires

src/CMakeFiles/druckwelle.dir/WebMapService.cpp.o.provides: src/CMakeFiles/druckwelle.dir/WebMapService.cpp.o.requires
	$(MAKE) -f src/CMakeFiles/druckwelle.dir/build.make src/CMakeFiles/druckwelle.dir/WebMapService.cpp.o.provides.build
.PHONY : src/CMakeFiles/druckwelle.dir/WebMapService.cpp.o.provides

src/CMakeFiles/druckwelle.dir/WebMapService.cpp.o.provides.build: src/CMakeFiles/druckwelle.dir/WebMapService.cpp.o

src/CMakeFiles/druckwelle.dir/utils/ImageProcessor.cpp.o: src/CMakeFiles/druckwelle.dir/flags.make
src/CMakeFiles/druckwelle.dir/utils/ImageProcessor.cpp.o: src/utils/ImageProcessor.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/rsc/Desktop/Druckwelle/CMakeFiles $(CMAKE_PROGRESS_5)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object src/CMakeFiles/druckwelle.dir/utils/ImageProcessor.cpp.o"
	cd /home/rsc/Desktop/Druckwelle/src && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/druckwelle.dir/utils/ImageProcessor.cpp.o -c /home/rsc/Desktop/Druckwelle/src/utils/ImageProcessor.cpp

src/CMakeFiles/druckwelle.dir/utils/ImageProcessor.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/druckwelle.dir/utils/ImageProcessor.cpp.i"
	cd /home/rsc/Desktop/Druckwelle/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/rsc/Desktop/Druckwelle/src/utils/ImageProcessor.cpp > CMakeFiles/druckwelle.dir/utils/ImageProcessor.cpp.i

src/CMakeFiles/druckwelle.dir/utils/ImageProcessor.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/druckwelle.dir/utils/ImageProcessor.cpp.s"
	cd /home/rsc/Desktop/Druckwelle/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/rsc/Desktop/Druckwelle/src/utils/ImageProcessor.cpp -o CMakeFiles/druckwelle.dir/utils/ImageProcessor.cpp.s

src/CMakeFiles/druckwelle.dir/utils/ImageProcessor.cpp.o.requires:
.PHONY : src/CMakeFiles/druckwelle.dir/utils/ImageProcessor.cpp.o.requires

src/CMakeFiles/druckwelle.dir/utils/ImageProcessor.cpp.o.provides: src/CMakeFiles/druckwelle.dir/utils/ImageProcessor.cpp.o.requires
	$(MAKE) -f src/CMakeFiles/druckwelle.dir/build.make src/CMakeFiles/druckwelle.dir/utils/ImageProcessor.cpp.o.provides.build
.PHONY : src/CMakeFiles/druckwelle.dir/utils/ImageProcessor.cpp.o.provides

src/CMakeFiles/druckwelle.dir/utils/ImageProcessor.cpp.o.provides.build: src/CMakeFiles/druckwelle.dir/utils/ImageProcessor.cpp.o

src/CMakeFiles/druckwelle.dir/WebServer.cpp.o: src/CMakeFiles/druckwelle.dir/flags.make
src/CMakeFiles/druckwelle.dir/WebServer.cpp.o: src/WebServer.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/rsc/Desktop/Druckwelle/CMakeFiles $(CMAKE_PROGRESS_6)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object src/CMakeFiles/druckwelle.dir/WebServer.cpp.o"
	cd /home/rsc/Desktop/Druckwelle/src && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/druckwelle.dir/WebServer.cpp.o -c /home/rsc/Desktop/Druckwelle/src/WebServer.cpp

src/CMakeFiles/druckwelle.dir/WebServer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/druckwelle.dir/WebServer.cpp.i"
	cd /home/rsc/Desktop/Druckwelle/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/rsc/Desktop/Druckwelle/src/WebServer.cpp > CMakeFiles/druckwelle.dir/WebServer.cpp.i

src/CMakeFiles/druckwelle.dir/WebServer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/druckwelle.dir/WebServer.cpp.s"
	cd /home/rsc/Desktop/Druckwelle/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/rsc/Desktop/Druckwelle/src/WebServer.cpp -o CMakeFiles/druckwelle.dir/WebServer.cpp.s

src/CMakeFiles/druckwelle.dir/WebServer.cpp.o.requires:
.PHONY : src/CMakeFiles/druckwelle.dir/WebServer.cpp.o.requires

src/CMakeFiles/druckwelle.dir/WebServer.cpp.o.provides: src/CMakeFiles/druckwelle.dir/WebServer.cpp.o.requires
	$(MAKE) -f src/CMakeFiles/druckwelle.dir/build.make src/CMakeFiles/druckwelle.dir/WebServer.cpp.o.provides.build
.PHONY : src/CMakeFiles/druckwelle.dir/WebServer.cpp.o.provides

src/CMakeFiles/druckwelle.dir/WebServer.cpp.o.provides.build: src/CMakeFiles/druckwelle.dir/WebServer.cpp.o

src/CMakeFiles/druckwelle.dir/main.cpp.o: src/CMakeFiles/druckwelle.dir/flags.make
src/CMakeFiles/druckwelle.dir/main.cpp.o: src/main.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/rsc/Desktop/Druckwelle/CMakeFiles $(CMAKE_PROGRESS_7)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object src/CMakeFiles/druckwelle.dir/main.cpp.o"
	cd /home/rsc/Desktop/Druckwelle/src && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/druckwelle.dir/main.cpp.o -c /home/rsc/Desktop/Druckwelle/src/main.cpp

src/CMakeFiles/druckwelle.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/druckwelle.dir/main.cpp.i"
	cd /home/rsc/Desktop/Druckwelle/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/rsc/Desktop/Druckwelle/src/main.cpp > CMakeFiles/druckwelle.dir/main.cpp.i

src/CMakeFiles/druckwelle.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/druckwelle.dir/main.cpp.s"
	cd /home/rsc/Desktop/Druckwelle/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/rsc/Desktop/Druckwelle/src/main.cpp -o CMakeFiles/druckwelle.dir/main.cpp.s

src/CMakeFiles/druckwelle.dir/main.cpp.o.requires:
.PHONY : src/CMakeFiles/druckwelle.dir/main.cpp.o.requires

src/CMakeFiles/druckwelle.dir/main.cpp.o.provides: src/CMakeFiles/druckwelle.dir/main.cpp.o.requires
	$(MAKE) -f src/CMakeFiles/druckwelle.dir/build.make src/CMakeFiles/druckwelle.dir/main.cpp.o.provides.build
.PHONY : src/CMakeFiles/druckwelle.dir/main.cpp.o.provides

src/CMakeFiles/druckwelle.dir/main.cpp.o.provides.build: src/CMakeFiles/druckwelle.dir/main.cpp.o

src/CMakeFiles/druckwelle.dir/WebMapTileService.cpp.o: src/CMakeFiles/druckwelle.dir/flags.make
src/CMakeFiles/druckwelle.dir/WebMapTileService.cpp.o: src/WebMapTileService.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/rsc/Desktop/Druckwelle/CMakeFiles $(CMAKE_PROGRESS_8)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object src/CMakeFiles/druckwelle.dir/WebMapTileService.cpp.o"
	cd /home/rsc/Desktop/Druckwelle/src && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/druckwelle.dir/WebMapTileService.cpp.o -c /home/rsc/Desktop/Druckwelle/src/WebMapTileService.cpp

src/CMakeFiles/druckwelle.dir/WebMapTileService.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/druckwelle.dir/WebMapTileService.cpp.i"
	cd /home/rsc/Desktop/Druckwelle/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/rsc/Desktop/Druckwelle/src/WebMapTileService.cpp > CMakeFiles/druckwelle.dir/WebMapTileService.cpp.i

src/CMakeFiles/druckwelle.dir/WebMapTileService.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/druckwelle.dir/WebMapTileService.cpp.s"
	cd /home/rsc/Desktop/Druckwelle/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/rsc/Desktop/Druckwelle/src/WebMapTileService.cpp -o CMakeFiles/druckwelle.dir/WebMapTileService.cpp.s

src/CMakeFiles/druckwelle.dir/WebMapTileService.cpp.o.requires:
.PHONY : src/CMakeFiles/druckwelle.dir/WebMapTileService.cpp.o.requires

src/CMakeFiles/druckwelle.dir/WebMapTileService.cpp.o.provides: src/CMakeFiles/druckwelle.dir/WebMapTileService.cpp.o.requires
	$(MAKE) -f src/CMakeFiles/druckwelle.dir/build.make src/CMakeFiles/druckwelle.dir/WebMapTileService.cpp.o.provides.build
.PHONY : src/CMakeFiles/druckwelle.dir/WebMapTileService.cpp.o.provides

src/CMakeFiles/druckwelle.dir/WebMapTileService.cpp.o.provides.build: src/CMakeFiles/druckwelle.dir/WebMapTileService.cpp.o

src/CMakeFiles/druckwelle.dir/dwcore.cpp.o: src/CMakeFiles/druckwelle.dir/flags.make
src/CMakeFiles/druckwelle.dir/dwcore.cpp.o: src/dwcore.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/rsc/Desktop/Druckwelle/CMakeFiles $(CMAKE_PROGRESS_9)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object src/CMakeFiles/druckwelle.dir/dwcore.cpp.o"
	cd /home/rsc/Desktop/Druckwelle/src && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/druckwelle.dir/dwcore.cpp.o -c /home/rsc/Desktop/Druckwelle/src/dwcore.cpp

src/CMakeFiles/druckwelle.dir/dwcore.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/druckwelle.dir/dwcore.cpp.i"
	cd /home/rsc/Desktop/Druckwelle/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/rsc/Desktop/Druckwelle/src/dwcore.cpp > CMakeFiles/druckwelle.dir/dwcore.cpp.i

src/CMakeFiles/druckwelle.dir/dwcore.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/druckwelle.dir/dwcore.cpp.s"
	cd /home/rsc/Desktop/Druckwelle/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/rsc/Desktop/Druckwelle/src/dwcore.cpp -o CMakeFiles/druckwelle.dir/dwcore.cpp.s

src/CMakeFiles/druckwelle.dir/dwcore.cpp.o.requires:
.PHONY : src/CMakeFiles/druckwelle.dir/dwcore.cpp.o.requires

src/CMakeFiles/druckwelle.dir/dwcore.cpp.o.provides: src/CMakeFiles/druckwelle.dir/dwcore.cpp.o.requires
	$(MAKE) -f src/CMakeFiles/druckwelle.dir/build.make src/CMakeFiles/druckwelle.dir/dwcore.cpp.o.provides.build
.PHONY : src/CMakeFiles/druckwelle.dir/dwcore.cpp.o.provides

src/CMakeFiles/druckwelle.dir/dwcore.cpp.o.provides.build: src/CMakeFiles/druckwelle.dir/dwcore.cpp.o

# Object files for target druckwelle
druckwelle_OBJECTS = \
"CMakeFiles/druckwelle.dir/layers/ExampleLayer.cpp.o" \
"CMakeFiles/druckwelle.dir/layers/QualityElevation.cpp.o" \
"CMakeFiles/druckwelle.dir/layers/TileCache.cpp.o" \
"CMakeFiles/druckwelle.dir/WebMapService.cpp.o" \
"CMakeFiles/druckwelle.dir/utils/ImageProcessor.cpp.o" \
"CMakeFiles/druckwelle.dir/WebServer.cpp.o" \
"CMakeFiles/druckwelle.dir/main.cpp.o" \
"CMakeFiles/druckwelle.dir/WebMapTileService.cpp.o" \
"CMakeFiles/druckwelle.dir/dwcore.cpp.o"

# External object files for target druckwelle
druckwelle_EXTERNAL_OBJECTS =

bin/druckwelle: src/CMakeFiles/druckwelle.dir/layers/ExampleLayer.cpp.o
bin/druckwelle: src/CMakeFiles/druckwelle.dir/layers/QualityElevation.cpp.o
bin/druckwelle: src/CMakeFiles/druckwelle.dir/layers/TileCache.cpp.o
bin/druckwelle: src/CMakeFiles/druckwelle.dir/WebMapService.cpp.o
bin/druckwelle: src/CMakeFiles/druckwelle.dir/utils/ImageProcessor.cpp.o
bin/druckwelle: src/CMakeFiles/druckwelle.dir/WebServer.cpp.o
bin/druckwelle: src/CMakeFiles/druckwelle.dir/main.cpp.o
bin/druckwelle: src/CMakeFiles/druckwelle.dir/WebMapTileService.cpp.o
bin/druckwelle: src/CMakeFiles/druckwelle.dir/dwcore.cpp.o
bin/druckwelle: src/CMakeFiles/druckwelle.dir/build.make
bin/druckwelle: lib/zlibwapi.lib
bin/druckwelle: lib/gdal_i.lib
bin/druckwelle: lib/gsl.lib
bin/druckwelle: lib/libmicrohttpd-dll.lib
bin/druckwelle: lib/PocoFoundation.lib
bin/druckwelle: lib/PocoNet.lib
bin/druckwelle: /usr/lib/libgdal.so
bin/druckwelle: lib/zlibwapi.lib
bin/druckwelle: lib/gdal_i.lib
bin/druckwelle: lib/gsl.lib
bin/druckwelle: lib/libmicrohttpd-dll.lib
bin/druckwelle: lib/PocoFoundation.lib
bin/druckwelle: lib/PocoNet.lib
bin/druckwelle: /usr/lib/libgdal.so
bin/druckwelle: src/CMakeFiles/druckwelle.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable ../bin/druckwelle"
	cd /home/rsc/Desktop/Druckwelle/src && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/druckwelle.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/CMakeFiles/druckwelle.dir/build: bin/druckwelle
.PHONY : src/CMakeFiles/druckwelle.dir/build

src/CMakeFiles/druckwelle.dir/requires: src/CMakeFiles/druckwelle.dir/layers/ExampleLayer.cpp.o.requires
src/CMakeFiles/druckwelle.dir/requires: src/CMakeFiles/druckwelle.dir/layers/QualityElevation.cpp.o.requires
src/CMakeFiles/druckwelle.dir/requires: src/CMakeFiles/druckwelle.dir/layers/TileCache.cpp.o.requires
src/CMakeFiles/druckwelle.dir/requires: src/CMakeFiles/druckwelle.dir/WebMapService.cpp.o.requires
src/CMakeFiles/druckwelle.dir/requires: src/CMakeFiles/druckwelle.dir/utils/ImageProcessor.cpp.o.requires
src/CMakeFiles/druckwelle.dir/requires: src/CMakeFiles/druckwelle.dir/WebServer.cpp.o.requires
src/CMakeFiles/druckwelle.dir/requires: src/CMakeFiles/druckwelle.dir/main.cpp.o.requires
src/CMakeFiles/druckwelle.dir/requires: src/CMakeFiles/druckwelle.dir/WebMapTileService.cpp.o.requires
src/CMakeFiles/druckwelle.dir/requires: src/CMakeFiles/druckwelle.dir/dwcore.cpp.o.requires
.PHONY : src/CMakeFiles/druckwelle.dir/requires

src/CMakeFiles/druckwelle.dir/clean:
	cd /home/rsc/Desktop/Druckwelle/src && $(CMAKE_COMMAND) -P CMakeFiles/druckwelle.dir/cmake_clean.cmake
.PHONY : src/CMakeFiles/druckwelle.dir/clean

src/CMakeFiles/druckwelle.dir/depend:
	cd /home/rsc/Desktop/Druckwelle && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/rsc/Desktop/Druckwelle /home/rsc/Desktop/Druckwelle/src /home/rsc/Desktop/Druckwelle /home/rsc/Desktop/Druckwelle/src /home/rsc/Desktop/Druckwelle/src/CMakeFiles/druckwelle.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/CMakeFiles/druckwelle.dir/depend

