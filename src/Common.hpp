/*

	OmniMIDI v15+ (Rewrite) for Win32/Linux

	This file contains the required code to run the driver under both Windows and Linux

*/

#ifndef _COMMON_H
#define _COMMON_H

#define MAX_PATH_LONG	32767
#define RANGE(variable, minv, maxv) ((variable) >= minv && (value) <= maxv)

#ifdef _WIN64
#define EXPORT	__declspec(dllexport) 
#define APICALL
#else
#define EXPORT
#define APICALL __stdcall
#endif

#include <windows.h>
#include <math.h>
#include <cstring>

#endif