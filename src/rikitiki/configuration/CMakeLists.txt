set(RT_SRC ${RT_SRC} 		   
	   configuration/configuration.cc
	   configuration/log.cc
	   configuration/ctemplate.cc
PARENT_SCOPE)

set(RT_LIBS ${RT_LIBS} 		   
	    config++
PARENT_SCOPE)
