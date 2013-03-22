
IF(${USE_STENCET})
	FIND_LIBRARY(STENCET_LIB stencet)
	IF(NOT ${STENCET_LIB})
	       IF(NOT (IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/stencet))
	       	       MESSAGE("Stencet missing, grabbing it.")
	       	       EXEC_PROGRAM(hg ARGS clone http://hg.code.sf.net/p/stencet/code ${CMAKE_CURRENT_SOURCE_DIR}/stencet)

               ENDIF()
	       ADD_SUBDIRECTORY(stencet)
	ENDIF()
ENDIF()
