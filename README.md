# Druckwelle #

**Turbo GeoServer - 100% C/C++**

 * Web Map Service (WMS)
 * Web Map Tile Service (WMTS)
 * Web Feature Service (WFS)

Aiming to serve blistering fast geo raster data beyond the WMS/WMTS specification like raw data or custom formats.
Druckwelle does not feature a full implementation of WMS/WMTS Specifications and is not planned to do some day.

### Features ###

 * Raw output formats for data layers (float32, uint64, int16, etc.)
 * building and serving of WMTS caches

### Built-in WMS Layers ###

 * Example Layer (showing how to implement the basics for your own layer)
 * High Quality Elevation Map Service (combining ASTER (1 arc second) + SRTM_v4.1 (3 arc seconds))
 * OpenStreetMap to Signed Distance Field Rasterization

---

## Build Requirements ##

 * CMake 2.8
 * Visual Studio 2015 (Windows only)

## Windows ##

 1. run GenerateLibAndInc.bat
 * run CreateProjectFiles.bat
 * open Visual Studio Solution from sln directory and build

## Linux / OSX ##

 1. download Poco Libraries Basic Edition (http://pocoproject.org/), compile, and install
 * install package libmicrohttpd-dev
 * install package libgdal-dev
 * cmake -G "Unix Makefile"
 * make

---

## To-do: ##

 * replace all 3rdparty lib copies by submodules
 * replace POCO and libmicrohttpd library by cpprestsdk (https://github.com/Microsoft/cpprestsdk)

---

## License ##

This is free and unencumbered software released into the public domain. See UNLICENSE file for details.
