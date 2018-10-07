================================================================================
    How to use the test and kmapi.dll
===============================================================================

In order to use the kmapi dll in a C++ application you will need to place the folowing files in your project root:
1: KMapi.dll
2: KMApi.lib
2. KMApi.h
3: Generaldefines.h

Point your application to the dll :
In your Projects properties, expend Linker->Input and add to "additional Depandencies" the KMApi.lib
Now you can access the dlls class, KMApi.

In your h code add the folowing lines 

#pragma comment(lib, "Kmapi.lib")
#include "KMApi.h"

This will allow you to create and use the KMApi class.
