set(RT_SRC ${RT_SRC} 		   
	   ctemplate/connContext_ext.cc
	   ctemplate/templatePreprocessor.cc
	   ctemplate/headerFooterPreprocessor.cc
PARENT_SCOPE)

set(RT_LIBS ${RT_LIBS} 		   
	    ctemplate
PARENT_SCOPE)
