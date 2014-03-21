set(RT_SRC ${RT_SRC} 
	   mongoose/connContext.cc 
	   mongoose/mongooseWebcontext.cc
	   ${THIRD_PARTY_DIR}/mongoose/mongoose.c 
	   mongoose/server.cc PARENT_SCOPE)

IF(NOT ${WIN32}) 	   
	set(RT_LIBS ${RT_LIBS} 		   
		dl
	PARENT_SCOPE)
ENDIF()
