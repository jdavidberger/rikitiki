macro( add_module module )
       SET(module ${module})
       set(imodule "rikitiki/examples/${module}")

       STRING(SUBSTRING ${module} 0 1 f)
       STRING(TOUPPER ${f} f)
       STRING(REGEX REPLACE "^.(.*)" "${f}\\1Module" modstruct "${module}")

       	IF(${USE_MONGOOSE})
		CONFIGURE_FILE(main.i ${CMAKE_CURRENT_BINARY_DIR}/main_${module}.cc )	
		add_executable(${module} main_${module}.cc)
		target_link_libraries(${module} rikitiki)
       	ENDIF()

	IF(${USE_APACHE})
		CONFIGURE_FILE(mod.i ${CMAKE_CURRENT_BINARY_DIR}/mod_${module}.cc )
		add_library(mod_${module} SHARED mod_${module}.cc)
		set_target_properties(mod_${module} PROPERTIES PREFIX "")
		target_link_libraries(mod_${module} rikitiki)
	ENDIF()
endmacro(add_module )


include_directories(${THIRD_PARTY_DIR}/curl-7.28.1/include)

link_directories(
		${THIRD_PARTY_DIR}/curl-7.28.1/bin/lib
		${THIRD_PARTY_DIR}/ctemplate-2.2 )

set (RT_SRC
    server.cc 
    connContext.cc 
    content_types.cc 
    routing.cc
    http_statuses.cc
    stream_helpers.cc
    )

set (RT_LIBS curl pthread mxcomp)

IF(${USE_MONGOOSE})
	include_directories(${THIRD_PARTY_DIR}/mongoose)
	add_subdirectory(mongoose)	
ENDIF()

IF(${USE_CTEMPLATE})
	include_directories(${THIRD_PARTY_DIR}/ctemplate-2.2/src)
	add_subdirectory(ctemplate)
ENDIF()

IF(${USE_APACHE})
	include_directories(/usr/include/apr-1/)	
	add_subdirectory(apache)
ENDIF()

IF(${USE_JSONCPP})
	add_subdirectory(jsoncpp)
ENDIF()

IF(${USE_CONFIGURATION})
	add_subdirectory(configuration)
ENDIF()

CONFIGURE_FILE(config.h.in ${CMAKE_SOURCE_DIR}/include/rikitiki/config.h)	

add_library(rikitiki ${RT_SRC})
target_link_libraries(rikitiki ${RT_LIBS})		     
install(TARGETS rikitiki DESTINATION lib)

if(${MAKE_EXAMPLES})
	add_subdirectory(examples)
ENDIF()
