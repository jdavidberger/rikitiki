#include(CppcheckTargets)

macro( add_module module)
    SET(module ${module})
	   
    set(imodule "${CMAKE_SOURCE_DIR}/include/rikitiki/examples/${module}")

    STRING(SUBSTRING ${module} 0 1 f)
    STRING(TOUPPER ${f} f)
    STRING(REGEX REPLACE "^.(.*)" "${f}\\1Module" modstruct "${module}")

    IF(USE_MONGOOSE)
		CONFIGURE_FILE(main.i ${CMAKE_CURRENT_BINARY_DIR}/main_${module}.cc )	
		add_executable(${module}_mongoose main_${module}.cc ${imodule}.h main.i)
		target_link_libraries(${module}_mongoose rt_mongoose mongoose)
		set_property(TARGET ${module}_mongoose PROPERTY FOLDER "examples/mongoose")
    ENDIF()

    IF(USE_IIS7)
		CONFIGURE_FILE(iis7_mod.i ${CMAKE_CURRENT_BINARY_DIR}/${module}_iis7_module.cc )	
		add_library(${module}_IIS7Module SHARED ${module}_iis7_module.cc ${imodule}.h iis7_mod.i)
		target_link_libraries(${module}_IIS7Module rt_iis7)
		set_property(TARGET ${module}_IIS7Module PROPERTY FOLDER "examples/IIS7")
    ENDIF()

     IF(USE_APACHE)
	       CONFIGURE_FILE(mod.i ${CMAKE_CURRENT_BINARY_DIR}/mod_${module}.cc )
	       add_library(mod_${module} SHARED mod_${module}.cc ${imodule}.h mod.i)
	       set_target_properties(mod_${module} PROPERTIES PREFIX "")
	       target_link_libraries(mod_${module} rt_apache)
	       set_property(TARGET mod_${module} PROPERTY FOLDER "examples/apache")
     ENDIF()

     IF(USE_CEF)
	       CONFIGURE_FILE(main_cef.i ${CMAKE_CURRENT_BINARY_DIR}/main_cef_${module}.cc )	
	       add_executable(${module}_cef main_cef_${module}.cc ${imodule}.h)
	       target_link_libraries(${module}_cef rt_cef Ws2_32)
	       SET_TARGET_PROPERTIES(${module}_cef PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS") 
	       SET_TARGET_PROPERTIES(${module}_cef PROPERTIES COMPILE_FLAGS "/D _UNICODE /D UNICODE")
	       add_custom_command(TARGET ${module}_cef PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${THIRD_PARTY_DIR}/cef/cef3/$(Platform) $<TARGET_FILE_DIR:${module}_cef>)
	       add_custom_command(TARGET ${module}_cef PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${THIRD_PARTY_DIR}/cef/cef3/Resources $<TARGET_FILE_DIR:${module}_cef>)
	       set_property(TARGET ${module}_cef PROPERTY FOLDER "examples/cef")
     ENDIF()
endmacro(add_module )

macro(add_source_directory srcdir) 
	file (GLOB_RECURSE  _INCLUDES ../include/rikitiki/${srcdir}/*.tcc ../include/rikitiki/${srcdir}/*.h )

	file (GLOB_RECURSE  _SOURCES "${srcdir}/*.cc" "${srcdir}/*.cpp" "${srcdir}/*.c")
	file (GLOB_RECURSE  _SOURCES "${srcdir}/*.c*")
        
        FOREACH(srcFile ${_SOURCES})
               get_filename_component(path ${srcFile} DIRECTORY)
               string(LENGTH "${CMAKE_CURRENT_SOURCE_DIR}/" length )
               string(SUBSTRING ${path} ${length} -1 relPath)               
               string(REPLACE "/" "\\" relPathL "${relPath}" )               
               SOURCE_GROUP("Source\\${relPathL}" FILES ${srcFile})
        ENDFOREACH(srcFile)

        FOREACH(srcFile ${_INCLUDES})
               get_filename_component(path ${srcFile} DIRECTORY)
               string(LENGTH "${CMAKE_CURRENT_SOURCE_DIR}/../include/rikitiki/" length )
               string(SUBSTRING ${path} ${length} -1 relPath)
               string(REPLACE "/" "\\" relPathL "${relPath}" )               
               SOURCE_GROUP("Include\\${relPathL}" FILES ${srcFile})
        ENDFOREACH(srcFile)

	#SOURCE_GROUP("Include\\${srcdir}" FILES ${_INCLUDES})
	set(RT_SRC ${RT_SRC} ${_INCLUDES} ${_SOURCES})
endmacro(add_source_directory)

macro(add_server_directory srcdir) 
	file (GLOB _INCLUDES ../include/rikitiki/${srcdir}/*.tcc ../include/rikitiki/${srcdir}/*.h )
	file (GLOB _SOURCES "${srcdir}/*.c*")

	SOURCE_GROUP("Source" FILES ${_SOURCES})
	SOURCE_GROUP("Include" FILES ${_INCLUDES})
	add_rikitiki_library(rt_${srcdir} ${_INCLUDES} ${_SOURCES})
        target_link_libraries(rt_${srcdir} rikitiki)
        set_property(TARGET rt_${srcdir} PROPERTY FOLDER "servers")

endmacro(add_server_directory)


#include_directories(${THIRD_PARTY_DIR}/curl-7.28.1/include)

link_directories(
#		${THIRD_PARTY_DIR}/curl-7.28.1/bin/lib
		${THIRD_PARTY_DIR}/ctemplate-2.2 
		${THIRD_PARTY_DIR}/cef/cef3/Release
)

file (GLOB RT_INC ../include/rikitiki/*.h ../include/rikitiki/*.tcc)
file (GLOB RT_SRC "*.cc")
set(RT_SRC ${RT_INC} ${RT_SRC})
SOURCE_GROUP("Source\\Base"  FILES ${RT_SRC})
SOURCE_GROUP("Include\\Base" FILES ${RT_INC})

add_source_directory(utils)	
add_source_directory(http)	

IF(${WIN32})
        add_source_directory(os/windows)
	set (RT_LIBS mxcomp)
ELSE()
        add_source_directory(os/linux)
	set (RT_LIBS pthread mxcomp event event_pthreads)
ENDIF()

IF(USE_WEBSOCKET)
	add_source_directory(websocket)	
	add_subdirectory(websocket)		
ENDIF()

IF(USE_STENCET)
	add_subdirectory(stencet)
ENDIF()

IF(USE_CTEMPLATE)
	include_directories(${THIRD_PARTY_DIR}/ctemplate-2.2/src)
	add_subdirectory(ctemplate)
ENDIF()

IF(USE_JSONCPP)
	add_subdirectory(jsoncpp)
ENDIF()

IF(USE_CONFIGURATION)
	add_source_directory(configuration)  
	add_subdirectory(configuration)
ENDIF()

CONFIGURE_FILE(config.h.in ${CMAKE_SOURCE_DIR}/include/rikitiki/config.h)	

add_rikitiki_library(rikitiki ${RT_SRC})
target_link_libraries(rikitiki ${RT_LIBS})

#add_cppcheck(rikitiki STYLE POSSIBLE_ERROR FAIL_ON_WARNINGS) 

IF(USE_MONGOOSE)
	add_server_directory(mongoose)
		target_link_libraries(rt_mongoose mongoose)
	include_directories(${THIRD_PARTY_DIR}/mongoose)
        set(rikitiki_INCLUDE_DIRS "${rikitiki_INCLUDE_DIRS};${THIRD_PARTY_DIR}/mongoose")         
	#set(RT_LIBS ${RT_LIBS} mongoose )
	#add_subdirectory(mongoose)	
ENDIF()

IF(USE_APACHE)
	include_directories(/usr/include/apr-1.0/)	
	add_server_directory(apache)
ENDIF()

IF(USE_IIS7)	
	add_server_directory(iis7)
ENDIF()

IF(USE_CEF)
	add_server_directory(cef)
        target_link_libraries(rt_cef libcef libcef_dll_wrapper )		     
        set(rikitiki_INCLUDE_DIRS "${rikitiki_INCLUDE_DIRS};${THIRD_PARTY_DIR}/cef/cef3/")         
	include_directories(${THIRD_PARTY_DIR}/cef/cef3/)
ENDIF()

if(MAKE_EXAMPLES)
	add_subdirectory(examples)
ENDIF()

if(${MAKE_TESTS})
	add_subdirectory(tests)
ENDIF()

set(rikitiki_INCLUDE_DIRS ${rikitiki_INCLUDE_DIRS} PARENT_SCOPE)

