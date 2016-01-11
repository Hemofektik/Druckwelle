project (DruckwelleWMS_Test)
file(GLOB_RECURSE SOURCES *.cpp *.h *.hpp *.c)

foreach(f ${SOURCES})
    # Get the path of the file relative to ${DIRECTORY},
    # then alter it (not compulsory)
    file(RELATIVE_PATH SRCGR ${CMAKE_CURRENT_SOURCE_DIR} ${f})
    set(SRCGR "Sources/${SRCGR}")

    # Extract the folder, ie remove the filename part
    string(REGEX REPLACE "(.*)(/[^/]*)$" "\\1" SRCGR ${SRCGR})

    # Source_group expects \\ (double antislash), not / (slash)
    string(REPLACE / \\ SRCGR ${SRCGR})
    source_group("${SRCGR}" FILES ${f})
endforeach()

set(ADDITIONAL_SOURCES 
	${CMAKE_SOURCE_DIR}/src/utils/ImageProcessor.cpp
	${CMAKE_SOURCE_DIR}/src/utils/Elevation.cpp
	${CMAKE_SOURCE_DIR}/3rdparty/shapelib/shpopen.c 
	${CMAKE_SOURCE_DIR}/3rdparty/shapelib/shptree.c 
	${CMAKE_SOURCE_DIR}/3rdparty/shapelib/dbfopen.c 
	${CMAKE_SOURCE_DIR}/3rdparty/shapelib/safileio.c
)

foreach(f ${ADDITIONAL_SOURCES})
    # Get the path of the file relative to ${DIRECTORY},
    # then alter it (not compulsory)
    file(RELATIVE_PATH SRCGR ${CMAKE_SOURCE_DIR} ${f})
    set(SRCGR "DruckwelleWMS/${SRCGR}")

    # Extract the folder, ie remove the filename part
    string(REGEX REPLACE "(.*)(/[^/]*)$" "\\1" SRCGR ${SRCGR})

    # Source_group expects \\ (double antislash), not / (slash)
    string(REPLACE / \\ SRCGR ${SRCGR})
    source_group("${SRCGR}" FILES ${f})
endforeach()

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
	${GDAL_INCLUDE_DIR}
)

if(MSVC)
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
		${Boost_FILESYSTEM_LIBRARY}
		${Boost_SYSTEM_LIBRARY}
		microhttpd
		PocoFoundation
		PocoNet
		PocoXML
	)
endif()



add_executable (
   druckwelle_test
   ${SOURCES} 
   ${ADDITIONAL_SOURCES}  
)
