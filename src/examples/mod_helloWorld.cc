#include "helloWorld.cc"
#include <apache/register.h>

RegisterApacheHandler(helloWorld, HelloWorldModule);	
