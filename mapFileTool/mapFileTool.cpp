// mapFileTool.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"

extern "C"{
#include "mapfile.h"
}



int _tmain(int argc, _TCHAR* argv[])
{
	printf("stack trace:\n");
	printf("symbol: %s\n", MapfileLookupAddress(L"C:/CEBacktrace.map", 0x11230));
	printf("symbol: %s\n", MapfileLookupAddress(L"C:/CEBacktrace.map", 0x11370));
	printf("symbol: %s\n", MapfileLookupAddress(L"C:/CEBacktrace.map", 0x11458));
	printf("symbol: %s\n", MapfileLookupAddress(L"C:/CEBacktrace.map", 0x114D8));
	printf("symbol: %s\n", MapfileLookupAddress(L"C:/CEBacktrace.map", 0x11568));
	printf("symbol: %s\n", MapfileLookupAddress(L"C:/CEBacktrace.map", 0x11590));


	return 0;
}

