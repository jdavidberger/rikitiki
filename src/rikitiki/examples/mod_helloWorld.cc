#include "helloWorld.cc"
#include <rikitiki/apache/register.h>

RegisterApacheHandler(helloWorld, HelloWorldModule);	
