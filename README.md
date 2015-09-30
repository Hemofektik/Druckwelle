# Druckwelle #

Turbo Web Map Service (WMS) and Web Map Tile Service (WMTS) - 100% C/C++

### Features ###

 * Raw output formats for data layers (float32, uint64, int16, etc.)
 * building and serving of WMTS caches

### Built-in WMS Layers ###

 * Example Layer (showing how to implement the basics for your own layer)
 * High Quality Elevation Map Service (combining ASTER (1 arc second) + SRTM_v4.1 (3 arc seconds))



## Requirements ##

 * CMake 2.8
 * Visual Studio 2015 (Windows only)

## Windows ##

 * run GenerateLibAndInc.bat
 * run CreateProjectFiles.bat
 * open Visual Studio Solution from sln directory and build

## Linux/OSX ##

 * download Poco Libraries Basic Edition (http://pocoproject.org/), compile, and install
 * install libmicrohttpd-dev
 * install libgdal-dev

 * cmake -G "Unix Makefile"
 * make
