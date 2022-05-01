/*
OmniMIDI, a fork of BASSMIDI Driver - Declarations

Thank you Kode54 for allowing me to fork your awesome driver.

KDMAPI_OMONLY = Used internally by OmniMIDI
KDMAPI_ONLYSTRUCTS = Used by MIDI apps who want to use the KDMAPI functions
*/
#pragma once

#include <wtypes.h>

#define KDMAPI(f) WINAPI f

// Return the KDMAPI version from OmniMIDI as the following output: Major.Minor.Build Rev. Revision (eg. 1.30.0 Rev. 51).
BOOL KDMAPI(ReturnKDMAPIVer)(LPDWORD Major, LPDWORD Minor, LPDWORD Build, LPDWORD Revision);

// Checks if KDMAPI is available. You can ignore the output if you want, but you should give the user the choice between WinMM and KDMAPI.
BOOL KDMAPI(IsKDMAPIAvailable)();

// Initializes OmniMIDI through KDMAPI. (Like midiOutOpen)
BOOL KDMAPI(InitializeKDMAPIStream)();

// Closes OmniMIDI through KDMAPI. (Like midiOutClose)
BOOL KDMAPI(TerminateKDMAPIStream)();

// Resets OmniMIDI and all its MIDI channels through KDMAPI. (Like midiOutReset)
VOID KDMAPI(ResetKDMAPIStream)();

// Send short messages through KDMAPI. (Like midiOutShortMsg)
UINT KDMAPI(SendCustomEvent)(DWORD eventtype, DWORD chan, DWORD param);

// Send short messages through KDMAPI. (Like midiOutShortMsg)
VOID KDMAPI(SendDirectData)(DWORD dwMsg);

// Send short messages through KDMAPI like SendDirectData, but bypasses the buffer. (Like midiOutShortMsg)
VOID KDMAPI(SendDirectDataNoBuf)(DWORD dwMsg);

// Send long messages through KDMAPI. (Like midiOutLongMsg)
UINT KDMAPI(SendDirectLongData)(MIDIHDR* IIMidiHdr, UINT IIMidiHdrSize);

// Send long messages through KDMAPI like SendDirectLongData, but bypasses the buffer. (Like midiOutLongMsg)
UINT KDMAPI(SendDirectLongDataNoBuf)(LPSTR MidiHdrData, DWORD MidiHdrDataLen);

// Prepares the long data, and locks its memory to prevent apps from writing to it.
UINT KDMAPI(PrepareLongData)(MIDIHDR* IIMidiHdr, UINT IIMidiHdrSize);

// Unlocks the memory, and unprepares the long data.
UINT KDMAPI(UnprepareLongData)(MIDIHDR * IIMidiHdr, UINT IIMidiHdrSize);

// Get or set the current settings for the driver.
BOOL KDMAPI(DriverSettings)(DWORD Setting, DWORD Mode, LPVOID Value, UINT cbValue);

// timeGetTime, but 64-bit
DWORD64 KDMAPI(timeGetTime64)();

// modMessage
MMRESULT KDMAPI(modMessage)(UINT uDeviceID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dwParam1, DWORD_PTR dwParam2);

// INT
BOOL KDMAPI(InitializeCallbackFeatures)(HMIDI OMHM, DWORD_PTR OMCB, DWORD_PTR OMI, DWORD_PTR OMU, DWORD OMCM);
VOID KDMAPI(RunCallbackFunction)(DWORD Msg, DWORD_PTR P1, DWORD_PTR P2);