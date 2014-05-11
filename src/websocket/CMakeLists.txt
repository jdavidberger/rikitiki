IF(NOT ${WIN32}) 	   
	set(RT_LIBS ${RT_LIBS} 		   
		dl
	PARENT_SCOPE)
ENDIF()
