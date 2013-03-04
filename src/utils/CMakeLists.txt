add_library(utils log.cc time_utils.cc config.cc )
target_link_libraries(utils pthread  config++ )

