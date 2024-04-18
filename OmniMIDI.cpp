/*

	OmniMIDI v15+ (Rewrite) for Windows NT

	This file contains the required code to run the driver under Windows 7 SP1 and later.
	This file is useful only if you want to compile the driver under Windows, it's not needed for Linux/macOS porting.

*/

#include "OmniMIDI.hpp"

int (WINAPI* IsKDMAPIAvailable)() = 0;
int (WINAPI* InitializeKDMAPIStream)() = 0;
int (WINAPI* TerminateKDMAPIStream)() = 0;
void (WINAPI* ResetKDMAPIStream)() = 0;
void (WINAPI* SendDirectData)(unsigned int) = 0;
void (WINAPI* SendDirectDataNoBuf)(unsigned int) = 0;
unsigned int (WINAPI* SendDirectLongData)(void*, unsigned int) = 0;
unsigned int (WINAPI* SendDirectLongDataNoBuf)(void*, unsigned int) = 0;
unsigned int (WINAPI* PrepareLongData)(MIDIHDR*, unsigned int) = 0;
unsigned int (WINAPI* UnprepareLongData)(MIDIHDR*, unsigned int) = 0;
int (WINAPI* InitializeCallbackFeatures)(HMIDI*, DWORD_PTR, DWORD_PTR, DWORD_PTR, DWORD) = 0;
void (WINAPI* RunCallbackFunction)(DWORD, DWORD_PTR, DWORD_PTR) = 0;
int (WINAPI* SendCustomEvent)(unsigned int, unsigned int, unsigned int) = 0;
int (WINAPI* DriverSettings)(unsigned int, unsigned int, void*, unsigned int) = 0;
unsigned long long (WINAPI* timeGetTime64)() = 0;
unsigned int (WINAPI* modMessage)(unsigned int, unsigned int, DWORD_PTR, DWORD_PTR, DWORD_PTR) = 0;