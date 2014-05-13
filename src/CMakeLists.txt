macro( add_module module )
       SET(module ${module})
       set(imodule "rikitiki/examples/${module}")

       STRING(SUBSTRING ${module} 0 1 f)
       STRING(TOUPPER ${f} f)
       STRING(REGEX REPLACE "^.(.*)" "${f}\\1Module" modstruct "${module}")

       	IF(${USE_MONGOOSE})
			CONFIGURE_FILE(main.i ${CMAKE_CURRENT_BINARY_DIR}/main_${module}.cc )	
			add_executable(${module}_mongoose main_${module}.cc)
			target_link_libraries(${module}_mongoose rikitiki)
			set_property(TARGET ${module}_mongoose PROPERTY FOLDER "examples/mongoose")
       	ENDIF()

	IF(${USE_APACHE})
		CONFIGURE_FILE(mod.i ${CMAKE_CURRENT_BINARY_DIR}/mod_${module}.cc )
		add_library(mod_${module} SHARED mod_${module}.cc)
		set_target_properties(mod_${module} PROPERTIES PREFIX "")
		target_link_libraries(mod_${module} rikitiki)
		set_property(TARGET mod_${module} PROPERTY FOLDER "examples/apache")
	ENDIF()

	IF(${USE_CEF})
		CONFIGURE_FILE(main_cef.i ${CMAKE_CURRENT_BINARY_DIR}/main_cef_${module}.cc )	
		add_executable(${module}_cef main_cef_${module}.cc)
		target_link_libraries(${module}_cef rikitiki)
		SET_TARGET_PROPERTIES(${module}_cef PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS") 
		SET_TARGET_PROPERTIES(${module}_cef PROPERTIES COMPILE_FLAGS "/D _UNICODE /D UNICODE")
		add_custom_command(TARGET ${module}_cef PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${THIRD_PARTY_DIR}/cef/cef3/Release $<TARGET_FILE_DIR:${module}_cef>)
		set_property(TARGET ${module}_cef PROPERTY FOLDER "examples/cef")
	ENDIF()

endmacro(add_module )

macro(add_source_directory srcdir) 
	file (GLOB _INCLUDES ../include/rikitiki/${srcdir}/*.tcc ../include/rikitiki/${srcdir}/*.h )
	file (GLOB _SOURCES "${srcdir}/*.c*")

	SOURCE_GROUP("Source\\${srcdir}" FILES ${_SOURCES})
	SOURCE_GROUP("Include\\${srcdir}" FILES ${_INCLUDES})
	set(RT_SRC ${RT_SRC} ${_INCLUDES} ${_SOURCES})
endmacro(add_source_directory)

include_directories(${THIRD_PARTY_DIR}/curl-7.28.1/include)

link_directories(
		${THIRD_PARTY_DIR}/curl-7.28.1/bin/lib
		${THIRD_PARTY_DIR}/ctemplate-2.2 
		${THIRD_PARTY_DIR}/cef/cef3/Release
)

file (GLOB RT_INC ../include/rikitiki/*.h ../include/rikitiki/*.tcc)
file (GLOB RT_SRC "*.cc")
set(RT_SRC ${RT_INC} ${RT_SRC})
SOURCE_GROUP("Source\\Base"  FILES ${RT_SRC})
SOURCE_GROUP("Include\\Base" FILES ${RT_INC})

IF(${WIN32})
	set (RT_LIBS mxcomp)
ELSE()
	set (RT_LIBS curl pthread mxcomp)
ENDIF()

IF(${USE_MONGOOSE})
	add_source_directory(mongoose)
	include_directories(${THIRD_PARTY_DIR}/mongoose)
	set(RT_LIBS ${RT_LIBS} mongoose )
	add_subdirectory(mongoose)	
ENDIF()

IF(${USE_WEBSOCKET})
	add_source_directory(websocket)	
	add_subdirectory(websocket)		
ENDIF()

IF(${USE_STENCET})
	add_subdirectory(stencet)
ENDIF()

IF(${USE_CTEMPLATE})
	include_directories(${THIRD_PARTY_DIR}/ctemplate-2.2/src)
	add_subdirectory(ctemplate)
ENDIF()

IF(${USE_APACHE})
	include_directories(/usr/include/apr-1/)	
	add_source_directory(apache)
ENDIF()

IF(${USE_JSONCPP})
	add_subdirectory(jsoncpp)
ENDIF()

IF(${USE_CONFIGURATION})
	add_source_directory(configuration)
	add_subdirectory(configuration)
ENDIF()

IF(${USE_CEF})
	add_source_directory(cef)
	set(RT_LIBS ${RT_LIBS} libcef libcef_dll_wrapper )
	include_directories(${THIRD_PARTY_DIR}/cef/cef3/)
ENDIF() 

CONFIGURE_FILE(config.h.in ${CMAKE_SOURCE_DIR}/include/rikitiki/config.h)	

add_library(rikitiki ${RT_SRC})
target_link_libraries(rikitiki ${RT_LIBS})		     
install(TARGETS rikitiki DESTINATION lib)

if(${MAKE_EXAMPLES})
	add_subdirectory(examples)
ENDIF()
