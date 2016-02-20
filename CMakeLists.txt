cmake_minimum_required(VERSION 2.8)

project (Druckwelle)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  "${CMAKE_SOURCE_DIR}/bin")



add_definitions( -DPOCO_NO_AUTOMATIC_LIBS )
add_definitions( -DLITTLE_ENDIAN=1 )

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/3rdparty/")

if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")

	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /openmp")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /openmp")

	INCLUDE_EXTERNAL_MSPROJECT(libconfig++ "${CMAKE_SOURCE_DIR}/3rdparty/libconfig/lib/libconfig++.vcxproj")

	add_definitions( -DALLOW_AMP=1 )
	add_definitions( -DNOMINMAX )
else()
   
    #set(CMAKE_BUILD_TYPE Debug)
    
    # Set C++11 feature set for both GCC and Clang 
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    
    # Debug symbols for GCC
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
 


	add_definitions( -DALLOW_AMP=0 )

	find_package(GDAL REQUIRED)
	find_package(ZLIB REQUIRED)
	find_package(LIBCONFIG REQUIRED)
endif()


include_directories(
	${CMAKE_SOURCE_DIR}/inc
	${CMAKE_SOURCE_DIR}/3rdparty/cpplinq
	${CMAKE_SOURCE_DIR}/3rdparty/stb
	${CMAKE_SOURCE_DIR}/3rdparty/ZFXMath/include
	${CMAKE_SOURCE_DIR}/3rdparty/shapelib
	${CMAKE_SOURCE_DIR}/3rdparty/loose_quadtree/src/include
	${CMAKE_SOURCE_DIR}/3rdparty/protobuf/src
	${CMAKE_SOURCE_DIR}/3rdparty/libconfig/contrib/chained
	${GDAL_INCLUDE_DIR}
)


if(MSVC)
include_directories(
	${CMAKE_SOURCE_DIR}/3rdparty/libconfig/lib
)
link_libraries (
	${CMAKE_SOURCE_DIR}/lib/zlibwapi.lib
	${CMAKE_SOURCE_DIR}/lib/gdal_i.lib
	${CMAKE_SOURCE_DIR}/lib/gsl.lib
	${CMAKE_SOURCE_DIR}/lib/libmicrohttpd-dll.lib
	optimized ${CMAKE_SOURCE_DIR}/lib/PocoFoundation.lib
	optimized ${CMAKE_SOURCE_DIR}/lib/PocoNet.lib
	optimized ${CMAKE_SOURCE_DIR}/lib/PocoXML.lib
	debug ${CMAKE_SOURCE_DIR}/lib/PocoFoundationd.lib
	debug ${CMAKE_SOURCE_DIR}/lib/PocoNetd.lib
	debug ${CMAKE_SOURCE_DIR}/lib/PocoXMLd.lib
)
else()
link_libraries (
	${GDAL_LIBRARY}
	${ZLIB_LIBRARIES}
	${LIBCONFIG_LIBRARIES}
	microhttpd
	PocoFoundation
	PocoNet
	PocoXML
	stdc++fs
)
endif()

add_subdirectory(3rdparty/protobuf/cmake EXCLUDE_FROM_ALL)

set  (THIRDPARTY
	  libprotobuf-lite)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(TARGET ${THIRDPARTY} libconfig++ PROPERTY FOLDER "3rdparty")

link_libraries (
	${THIRDPARTY}
)

add_subdirectory(src)
add_subdirectory(test)


if(CMAKE_CONFIGURATION_TYPES)
	set(CMAKE_CONFIGURATION_TYPES Debug Release RelWithDebInfo)
	set(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}" CACHE STRING
		"Reset the configurations to what we need"
		FORCE)
endif()
