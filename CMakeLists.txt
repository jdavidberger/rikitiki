cmake_minimum_required (VERSION 2.6) 
project (rikitiki) 

SET(rikitiki_VERSION_MAJOR 0.1)
SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS On) 

EXEC_PROGRAM(hg ${CMAKE_CURRENT_SOURCE_DIR} ARGS tip OUTPUT_VARIABLE hg_tip)
SET(rikitiki_minor_version "unknown")
STRING(REGEX REPLACE "changeset:[ \t]*([0-9]+):(.*)" "\\1" rikitiki_VERSION_MINOR "${hg_tip}")
FILE(WRITE ${CMAKE_BINARY_DIR}/VERSION "v${rikitiki_VERSION_MAJOR}.${rikitiki_VERSION_MINOR}")
FILE(READ ${CMAKE_BINARY_DIR}/VERSION rikitiki_version)
message("Building rikitiki ${rikitiki_version}")

set(THIRD_PARTY_DIR ${CMAKE_SOURCE_DIR}/third)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

# MSVC warnings are dumb
if (MSVC)
   set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -wd4514 -wd4820 -wd4350 -wd4710 -wd4711")
   set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
   set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")			    				
endif (MSVC)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_C_FLAGS} -std=c++11")


SET(FEATURES "")

MACRO( disable_feature feature_)
       STRING(TOUPPER ${feature_} feature)
       SET("USE_${feature}" 0)
       MESSAGE("Not using ${feature_}")
ENDMACRO(disable_feature)

MACRO( enable_feature feature_)
       STRING(TOUPPER ${feature_} feature)       
       SET("USE_${feature}" 1)
       MESSAGE("Using ${feature}")
       SET(DOXYFILE_DEFINED "${DOXYFILE_DEFINED} RT_USE_${feature}")
       LIST(APPEND FEATURES "${feature}")
       SET(CONFIG 
"#define RT_USE_${feature} 1
${CONFIG}")

ENDMACRO(enable_feature)

MACRO( check_feature feature_ defaultvalue)
       STRING(TOUPPER ${feature_} feature)  
       IF("${USE_${feature}}" STREQUAL "")
       	SET("USE_${feature}" ${defaultvalue})
       endif()
       IF("${USE_${feature}}" EQUAL "1")
       	ENABLE_FEATURE(${feature})
	ELSE()
	DISABLE_FEATURE(${feature})
	endif()

ENDMACRO(check_feature)

CHECK_FEATURE(apache 0)
CHECK_FEATURE(mongoose 1)
CHECK_FEATURE(ctemplate 0)
CHECK_FEATURE(jsoncpp 0)
CHECK_FEATURE(rest 0)
CHECK_FEATURE(configuration 0)
CHECK_FEATURE(log 1)
CHECK_FEATURE(stencet 0) 
CHECK_FEATURE(websocket 0)
CHECK_FEATURE(cef 0)

IF("${MAKE_EXAMPLES}" STREQUAL "")
       set(MAKE_EXAMPLES 0)
ENDIF()

include_directories(include)

# Might need some of these...
include_directories(third/mxcomp/include)
include_directories(third/stencet/include)
add_subdirectory(third)


add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(Doxygen)


add_custom_target(memcheck
	COMMAND GLIBCXX_FORCE_NEW=1 valgrind --tool=memcheck --leak-check=full --log-file=log ./examples
	COMMENT "Started example server via valgrind" VERBATIM
)

add_custom_target(helgrind
	COMMAND valgrind --tool=helgrind --log-file=log ./examples
	COMMENT "Started example server via valgrind" VERBATIM
)

add_custom_target(release
	COMMAND hg archive -t tgz ${CMAKE_BINARY_DIR}/rikitiki-${rikitiki_version}-src.tar.gz
	COMMENT "Generating release tar" VERBATIM
)

set_property(TARGET helgrind PROPERTY FOLDER "testing")
set_property(TARGET memcheck PROPERTY FOLDER "testing")	
set_property(TARGET release PROPERTY FOLDER "testing")
set_property(TARGET test PROPERTY FOLDER "testing")

export(PACKAGE rikitiki)

install(DIRECTORY include/rikitiki DESTINATION include)
