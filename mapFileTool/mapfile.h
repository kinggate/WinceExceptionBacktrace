
#pragma once

#include "windows.h"
#include "string.h"
#include "stdlib.h"
#include <stdio.h>

//extern "C"{
//PSTR MapfileLookupAddress(TCAHR* wszModuleName,DWORD dwAddress);
//};
//extern "C"{
PSTR
MapfileLookupAddress(
    IN  PWSTR wszModuleName,
    IN  DWORD dwAddress);
//}