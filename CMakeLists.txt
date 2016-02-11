cmake_minimum_required(VERSION 2.8)

project (Druckwelle)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  "${CMAKE_SOURCE_DIR}/bin")

if(CMAKE_COMPILER_IS_GNUCXX)
	#set(CMAKE_BUILD_TYPE Debug)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

add_definitions( -DPOCO_NO_AUTOMATIC_LIBS )
add_definitions( -DLITTLE_ENDIAN=1 )

if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")

	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /openmp")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /openmp")

	add_definitions( -DHAS_FILE_SYSTEM=1 )
	add_definitions( -DALLOW_AMP=1 )
else()
	add_definitions( -DHAS_FILE_SYSTEM=0 )
	add_definitions( -DALLOW_AMP=0 )

	find_package(GDAL)
	find_package(Boost COMPONENTS system filesystem REQUIRED)
endif()



include_directories(
	${CMAKE_SOURCE_DIR}/inc
	${CMAKE_SOURCE_DIR}/3rdparty/cpplinq
	${CMAKE_SOURCE_DIR}/3rdparty/stb
	${CMAKE_SOURCE_DIR}/3rdparty/ZFXMath/include
	${CMAKE_SOURCE_DIR}/3rdparty/shapelib
	${CMAKE_SOURCE_DIR}/3rdparty/loose_quadtree/src/include
	${CMAKE_SOURCE_DIR}/3rdparty/protobuf/src
	${GDAL_INCLUDE_DIR}
)

if(MSVC)
link_libraries (
	${CMAKE_SOURCE_DIR}/lib/zlibwapi.lib
	${CMAKE_SOURCE_DIR}/lib/gdal_i.lib
	${CMAKE_SOURCE_DIR}/lib/geos_c.lib
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
	${Boost_FILESYSTEM_LIBRARY}
	${Boost_SYSTEM_LIBRARY}
	microhttpd
	PocoFoundation
	PocoNet
	PocoXML
)
endif()

add_subdirectory(3rdparty/protobuf/cmake EXCLUDE_FROM_ALL)

set  (THIRDPARTY
	  libprotobuf-lite)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(TARGET ${THIRDPARTY} PROPERTY FOLDER "3rdparty")

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
