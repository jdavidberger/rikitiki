cmake_minimum_required (VERSION 2.6) 
project (rikitiki) 
SET(rikitiki_major_version v0.1)

EXEC_PROGRAM(hg ARGS tip OUTPUT_VARIABLE hg_tip)
SET(rikitiki_minor_version "unknown")
STRING(REGEX REPLACE "changeset:[ \t]*([0-9]+):(.*)" "\\1" rikitiki_minor_version "${hg_tip}")
FILE(WRITE ${CMAKE_BINARY_DIR}/VERSION "${rikitiki_major_version}.${rikitiki_minor_version}")
FILE(READ ${CMAKE_BINARY_DIR}/VERSION rikitiki_version)

set(THIRD_PARTY_DIR ${CMAKE_SOURCE_DIR}/third)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_C_FLAGS} -std=c++11 -Wno-reorder")

# Not super useful, except for self documentation purposes
MACRO( disable_feature feature_)
       STRING(TOUPPER ${feature_} feature)
       SET(USE_${feature} 0) 
ENDMACRO(disable_feature)

MACRO( enable_feature feature_)
       STRING(TOUPPER ${feature_} feature)       
       SET("USE_${feature}" 1)
       ADD_DEFINITIONS(-DUSE_${feature})
ENDMACRO(enable_feature)

ENABLE_FEATURE(apache)
ENABLE_FEATURE(mongoose)
ENABLE_FEATURE(ctemplate)
ENABLE_FEATURE(jsoncpp)

set(MAKE_EXAMPLES 1)

add_subdirectory (src)


find_package(Doxygen)
if(DOXYGEN_FOUND)	
	configure_file(${CMAKE_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
add_custom_target(doc
	COMMAND	${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
	COMMAND	tar -cf ${CMAKE_BINARY_DIR}/rikitiki-${rikitiki_version}-doc.tar.gz -C ${CMAKE_BINARY_DIR}/doc .
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	COMMENT "Generating API documentation with Doxygen" VERBATIM
)
endif(DOXYGEN_FOUND)


add_custom_target(release
	COMMAND hg archive -t tgz ${CMAKE_BINARY_DIR}/rikitiki-${rikitiki_version}-src.tar.gz
	COMMENT "Generating release tar" VERBATIM
)
