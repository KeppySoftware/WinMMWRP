/*

	OmniMIDI v15+ (Rewrite) for Windows NT

	This file contains the required code to run the driver under Windows 7 SP1 and later.
	This file is useful only if you want to compile the driver under Windows, it's not needed for Linux/macOS porting.

*/

#ifndef _OMNIMIDI_HPP
#define _OMNIMIDI_HPP

#pragma once

#include <windows.h>

extern int (WINAPI* IsKDMAPIAvailable)();
extern int (WINAPI* InitializeKDMAPIStream)();
extern int (WINAPI* TerminateKDMAPIStream)();
extern void (WINAPI* ResetKDMAPIStream)();
extern void (WINAPI* SendDirectData)(unsigned int);
extern void (WINAPI* SendDirectDataNoBuf)(unsigned int);
extern unsigned int (WINAPI* SendDirectLongData)(void*, unsigned int);
extern unsigned int (WINAPI* SendDirectLongDataNoBuf)(void*, unsigned int);
extern unsigned int (WINAPI* PrepareLongData)(MIDIHDR*, unsigned int);
extern unsigned int (WINAPI* UnprepareLongData)(MIDIHDR*, unsigned int);
extern int (WINAPI* InitializeCallbackFeatures)(HMIDI*, DWORD_PTR, DWORD_PTR, DWORD_PTR, DWORD);
extern void (WINAPI* RunCallbackFunction)(DWORD, DWORD_PTR, DWORD_PTR);
extern int (WINAPI* SendCustomEvent)(unsigned int, unsigned int, unsigned int);
extern int (WINAPI* DriverSettings)(unsigned int, unsigned int, void*, unsigned int);
extern unsigned long long (WINAPI* timeGetTime64)();
extern unsigned int (WINAPI* modMessage)(unsigned int, unsigned int, DWORD_PTR, DWORD_PTR, DWORD_PTR);

#endif