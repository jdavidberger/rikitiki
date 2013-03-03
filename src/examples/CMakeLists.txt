add_executable(helloWorld helloWorld.cc)
target_link_libraries(helloWorld rikitiki utils)

# These commands will create a module suitable to be loaded into apache
# You will still neeed to copy the resulting file to the apache modules
# folder with something like:
# 'sudo cp lib/mod_helloWorld.so /etc/httpd/modules'
# And then add the following line to /etc/httpd/conf/httpd.conf:
# LoadModule helloWorld_module modules/mod_helloWorld.so
add_library(mod_helloWorld SHARED mod_helloWorld.cc)
set_target_properties(mod_helloWorld PROPERTIES PREFIX "")
target_link_libraries(mod_helloWorld rikitiki utils)
