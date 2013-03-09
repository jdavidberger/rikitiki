/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */
#pragma once
#include "server.h"
#include "connContext.h"
#include "register.h"

/**\page apache Apache Integration
   
Apache integration works via apaches module system. You can create an apache module from a rikitiki module by using the RegisterApacheModule macro. Put this after your module definition:

~~~~~~
RegisterApacheHandler(<module-name>, <module-type>);
~~~~~~

The shared object you compile to should be named mod_<module-name>; although this is more of an apache naming convention than a requirement. This will expose a '<module-name>_module' to apache. Supposing we compiled a 'mod_helloWorld.so' shared object, we need to take that file and copy or link it to apaches modules directory. On most linux systems, that is going to be /etc/httpd/modules. 

Then we need to add the following line to the httpd.conf file (usually at /etc/httpd/conf/httpd.conf on linux systems):

~~~~~~
LoadModule <module-name>_module modules/mod_<module-name>.so
~~~~~~

Note that if you load in multiple rikitiki modules, they do not share any of the same static members. For performance reasons, it is often better to load up multiple rikitiki modules into a single apache module. 

\sa register.h
 */
