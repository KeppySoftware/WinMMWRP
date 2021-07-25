#include <Windows.h>
#include <string.h>
#include <stdio.h>
#include <locale.h>
#include "mmddk.h"
#include "OmniMIDI.h"
#include <time.h>

#define KDMAPI_UDID			0		// KDMAPI default uDeviceID
#define KDMAPI_NOTLOADED	0		// KDMAPI is not loaded
#define KDMAPI_LOADED		1		// KDMAPI is loaded

#define LONGMSG_MAXSIZE		65535	// Maximum size for a long message/MIDIHDR

#define NT_SUCCESS(StatCode) ((NTSTATUS)(StatCode) == 0)

typedef unsigned __int64 QWORD;
typedef long NTSTATUS;

// Required KDMAPI version
#define REQ_MAJOR	4
#define REQ_MINOR	0
#define REQ_BUILD	0
#define REQ_REV		0

// KDMAPI version from library
DWORD DrvMajor = 0, DrvMinor = 0, DrvBuild = 0, DrvRevision = 0;

// OM funcs
BOOL OMAlreadyInit = FALSE;																		// To check if OM has already been initialized
HMODULE OM = NULL;																				// OM lib
DWORD_PTR OMUser;																				// Dummy pointer, used for KDMAPI Output
HMIDI OMDummy = 0x10001;																		// Dummy pointer, used for KDMAPI Output
DWORD(WINAPI* TGT)() = 0;																		// TGT
BOOL(WINAPI* ICF)(HMIDI, DWORD_PTR, DWORD_PTR, DWORD_PTR, DWORD) = 0;							// InitializeCallbackFeatures
VOID(WINAPI* RCF)(DWORD, DWORD_PTR, DWORD_PTR) = 0;												// RunCallbackFunction

// WinNT Kernel funcs
HMODULE NTDLL = NULL;
DOUBLE SpeedHack = 1.0;																			// TGT speedhack
DWORD TickStart = 0;																			// For TGT
QWORD UTickStart = 0;																			// For TGT64
NTSTATUS(NTAPI* NQST)(QWORD*) = 0;																// NtQuerySystemTime

// Stock WinMM funcs
#include "WinMM.h"

BOOL CheckIfKDMAPIIsUpToDate() {
	BOOL GoAhead = FALSE;
	ReturnKDMAPIVer(&DrvMajor, &DrvMinor, &DrvBuild, &DrvRevision);

	// WinMMWRP and KDMAPI **MUST** match
	if (REQ_MAJOR == DrvMajor && REQ_MINOR == DrvMinor && REQ_BUILD <= DrvBuild && REQ_REV <= DrvRevision)
		GoAhead = TRUE;

	if (!GoAhead) {
		TCHAR ErrorBuf[1024];

		sprintf_s(
			ErrorBuf,
			1024,
			"This patch is meant for KDMAPI v%d.%d.%d.%d, but OmniMIDI is reporting KDMAPI v%d.%d.%d.%d.\nThis patch won't work with the current version of OmniMIDI installed, please update it.\n\nPress OK to quit.",
			REQ_MAJOR, REQ_MINOR, REQ_BUILD, REQ_REV, DrvMajor, DrvMinor, DrvBuild, DrvRevision);

		MessageBox(
			NULL,
			ErrorBuf,
			"KDMAPI ERROR",
			MB_ICONERROR | MB_OK | MB_SYSTEMMODAL
		);

		return FALSE;
	}

	return TRUE;
}

BOOL InitializeOMDirectAPI() {
	if (OM)
		return TRUE;

	HMODULE NTDLL;
	HKEY RegKey;
	BOOL WinMMLegacyMode = FALSE;
	DWORD dwType = REG_DWORD, dwSize = sizeof(DWORD);

	// Get OM's handle and load the secret sauce functions
	OM = GetModuleHandleA("OmniMIDI");
	NTDLL = GetModuleHandleA("ntdll");

	if (OM != NULL) {
		ICF = (BOOL*)GetProcAddress(OM, "int_ICF");											// Used internally by the wrapper for callback functions
		RCF = (BOOL*)GetProcAddress(OM, "int_RCF");											// Used internally by the wrapper for callback functions
	}
	else {
		MessageBox(
			NULL,
			"Something went wrong while getting a module handle from OmniMIDI!\n\nPress OK to quit.",
			"KDMAPI ERROR",
			MB_ICONERROR | MB_OK | MB_SYSTEMMODAL
		);

		return FALSE;
	}

	if (NTDLL != NULL) {
		NQST = (NTSTATUS*)GetProcAddress(NTDLL, "NtQuerySystemTime");						// Required for TGT
	}
	else {
		MessageBox(
			NULL,
			"Something went wrong while getting a module handle from the NT Layer DLL!\n\nPress OK to quit.",
			"KDMAPI ERROR",
			MB_ICONERROR | MB_OK | MB_SYSTEMMODAL
		);

		return FALSE;
	}

	if (!ICF || !RCF) {
		// One of the functions failed to load, exit
		MessageBox(
			NULL,
			"Failed to initialize KDMAPI!\n\nPress OK to quit.",
			"KDMAPI ERROR",
			MB_ICONERROR | MB_OK | MB_SYSTEMMODAL
		);

		return FALSE;
	}

	LSTATUS ROKE = RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\OmniMIDI\\Configuration", 0, KEY_READ, &RegKey);
	if (!ROKE) {
		RegQueryValueExW(RegKey, L"StockWinMM", NULL, &dwType, (LPBYTE)&WinMMLegacyMode, &dwSize);
	}
	RegCloseKey(&RegKey);

	if (WinMMLegacyMode == TRUE) {
		OutputDebugStringW(L"The user requested the legacy WinMM timeGetTime implementation.");
		TGT = WINMM_timeGetTime;
		GetSpeedHack();
	}
	else {
		OutputDebugStringW(L"Checking NtQuerySystemTime...");

		if (!NQST || !NT_SUCCESS(NQST(&TickStart))) {
			OutputDebugStringW(L"Something went wrong while using NtQuerySystemTime. Falling back to stock timeGetTime...");
			TGT = WINMM_timeGetTime;
			GetSpeedHack();
		}
		else {
			TGT = timeGetTime64;
			OutputDebugStringW(L"NtQuerySystemTime set...");
		}
	}

	if (!CheckIfKDMAPIIsUpToDate())
		return FALSE;

	IsKDMAPIAvailable();

	return TRUE;
}

BOOL DllMain(HINSTANCE hInstDLL, DWORD fdwReason, LPVOID fImpLoad) {
	switch (fdwReason) {

	case DLL_PROCESS_ATTACH:
	{
		if (InitializeWinMM())
			if (InitializeOMDirectAPI())
				return TRUE;

		return FALSE;
	}

	case DLL_PROCESS_DETACH:
	{
		if (OM)
			FreeLibrary(OM);

		if (OWINMM) {
			for (int i = 0; i < sizeof(MMImports) / sizeof(MMImports[0]); i++)
				*(MMImports[i].ptr) = 0;

			FreeLibrary(OWINMM);
		}

		return TRUE;
	}

	}

	return TRUE;
}

UINT WINAPI KDMAPI_midiOutGetNumDevs(void) {
#ifdef _DAWRELEASE
	// Return the first port which is KDMAPI, plus the others
	return MMmidiOutGetNumDevs() + 1;
#else
	return KDMAPI_LOADED;
#endif
}

MMRESULT WINAPI KDMAPI_midiOutGetDevCapsW(UINT_PTR uDeviceID, LPMIDIOUTCAPSW lpCaps, UINT uSize) {
	MIDIOUTCAPSW myCapsW;
	MIDIOUTCAPSW myCapsWTMP;
	UINT ret;

	// Return the output device, but Unicode (UTF-8)
	if (lpCaps == NULL || uSize != sizeof(MIDIOUTCAPSW)) return MMSYSERR_INVALPARAM;

#ifdef _DAWRELEASE
	switch (uDeviceID) {
		case MIDI_MAPPER:
			return MMmidiOutGetDevCapsW(uDeviceID, lpCaps, uSize);

		case KDMAPI_UDID:
			break;

		default:
			return MMmidiOutGetDevCapsW(uDeviceID - 1, lpCaps, uSize);
	}
#else
	if (uDeviceID > 0) return MMSYSERR_BADDEVICEID;
#endif

	// Parse settings from OmniMIDI
	ret = modMessage(0, MODM_GETDEVCAPS, OMUser, &myCapsWTMP, sizeof(MIDIOUTCAPSW));

	// Assign values
	myCapsW.wMid = myCapsWTMP.wMid;
	myCapsW.wPid = myCapsWTMP.wPid;

	wcsncpy(myCapsW.szPname, L"Keppy's Direct MIDI API", MAXPNAMELEN);

	myCapsW.wVoices = myCapsWTMP.wVoices;
	myCapsW.wNotes = myCapsWTMP.wNotes;
	myCapsW.wTechnology = myCapsWTMP.wTechnology;
	myCapsW.wChannelMask = myCapsWTMP.wChannelMask;

	// Add MIDICAPS_STREAM, since we won't be able to let WinMM handle the stream functions
	myCapsW.dwSupport = myCapsWTMP.dwSupport | MIDICAPS_STREAM;

	// Copy values to pointer, and return 0
	memcpy(lpCaps, &myCapsW, min(uSize, sizeof(myCapsW)));

	return MMSYSERR_NOERROR;
}

MMRESULT WINAPI KDMAPI_midiOutGetDevCapsA(UINT_PTR uDeviceID, LPMIDIOUTCAPSA lpCaps, UINT uSize) {
	// Return the output device, but ASCII/Multibyte
	if (lpCaps == NULL || uSize != sizeof(MIDIOUTCAPSA)) return MMSYSERR_INVALPARAM;

	// Parse info in Unicode
	MIDIOUTCAPSW myCapsW;
	UINT ret;

#ifdef _DAWRELEASE
	ret = KDMAPI_midiOutGetDevCapsW(uDeviceID, &myCapsW, sizeof(myCapsW));
#else
	ret = KDMAPI_midiOutGetDevCapsW(0, &myCapsW, sizeof(myCapsW));
#endif

	// Translate them to ASCII/Multibyte
	if (ret == MMSYSERR_NOERROR) {
		MIDIOUTCAPSA myCapsA;
		myCapsA.wMid = myCapsW.wMid;
		myCapsA.wPid = myCapsW.wPid;
		myCapsA.vDriverVersion = myCapsW.vDriverVersion;
		wcstombs(myCapsA.szPname, myCapsW.szPname, wcslen(myCapsW.szPname) + 1);
		myCapsA.wTechnology = myCapsW.wTechnology;
		myCapsA.wVoices = myCapsW.wVoices;
		myCapsA.wNotes = myCapsW.wNotes;
		myCapsA.wChannelMask = myCapsW.wChannelMask;
		myCapsA.dwSupport = myCapsW.dwSupport;
		memcpy(lpCaps, &myCapsA, min(uSize, sizeof(myCapsA)));
	}
	return ret;
}

MMRESULT WINAPI KDMAPI_midiOutShortMsg(HMIDIOUT hMidiOut, DWORD dwMsg) {
#ifdef _DAWRELEASE
	// If not OM, forward to WinMM
	if (hMidiOut != OMDummy) return MMmidiOutShortMsg(hMidiOut, dwMsg);
#endif
	return SendDirectData(dwMsg);
}

MMRESULT WINAPI KDMAPI_midiOutOpen(LPHMIDIOUT lphmo, UINT uDeviceID, DWORD_PTR dwCallback, DWORD_PTR dwCallbackInstance, DWORD dwFlags) {
	GetSpeedHack();

#ifdef _DAWRELEASE
	// If above this device ID, return an error
	if (uDeviceID > 0xFFFFFFFE) return MMSYSERR_BADDEVICEID;

	switch (uDeviceID) {
		// If it's requesting for the MIDI mapper, forward the call to WinMM/MIDIMAP
		case MIDI_MAPPER:
			return MMmidiOutOpen(lphmo, uDeviceID, dwCallback, dwCallbackInstance, dwFlags);

		// If it's asking for OmniMIDI, do everything in-house
		case KDMAPI_UDID :
			// Continue initializing KDMAPI
			break;

			// Else, just forward the call to WinMM
		default:
			return MMmidiOutOpen(lphmo, uDeviceID - 1, dwCallback, dwCallbackInstance, dwFlags);
	}
#endif

	if (!OMAlreadyInit) {
		// Setup the Callback (If there's one) - NEEDED FOR VANBASCO!
		// If dwflags is CALLBACK_EVENT, then skip, since it's not needed. (Java pls)
		if (!ICF((*lphmo), dwCallback, dwCallbackInstance, &OMUser, dwFlags))
		{
			MessageBox(NULL, "ICF failed!", "KDMAPI ERROR", MB_SYSTEMMODAL | MB_ICONERROR);
			return MMSYSERR_INVALPARAM;
		}
		
		// Close any stream, just to be safe
		TerminateKDMAPIStream();

		// Initialize a dummy out device
		*lphmo = OMDummy;

		// Initialize MIDI out
		if (!InitializeKDMAPIStream())
			return MMSYSERR_ALLOCATED;

		DriverSettings(0xFFFFF, NULL, NULL, NULL);

		RCF(MM_MOM_OPEN, 0, 0);

		OMAlreadyInit = TRUE;
		return MMSYSERR_NOERROR;
	}
	else MessageBox(NULL, "OmniMIDI has been already initialized via KDMAPI! Can not initialize it again!", "KDMAPI ERROR", MB_SYSTEMMODAL | MB_ICONERROR);

	return MMSYSERR_ALLOCATED;
}

MMRESULT WINAPI KDMAPI_midiOutClose(HMIDIOUT hMidiOut) {
	GetSpeedHack();

#ifdef _DAWRELEASE
	if (hMidiOut != OMDummy) return MMmidiOutClose(hMidiOut);
#endif
	// Close OM
	if (OMAlreadyInit) {
		if (!TerminateKDMAPIStream()) return MMSYSERR_NOMEM;

		DriverSettings(0xFFFFE, NULL, NULL, NULL);

		RCF(MM_MOM_CLOSE, 0, 0);

		hMidiOut = (HMIDI)0;
		OMAlreadyInit = FALSE;
	}

	return MMSYSERR_NOERROR;
}

MMRESULT WINAPI KDMAPI_midiOutReset(HMIDIOUT hMidiOut) {
#ifdef _DAWRELEASE
	if (hMidiOut != OMDummy) return MMmidiOutReset(hMidiOut);
#endif
	// Reset the MIDI channels
	ResetKDMAPIStream();
	return MMSYSERR_NOERROR;
}

MMRESULT WINAPI KDMAPI_midiOutPrepareHeader(HMIDIOUT hMidiOut, MIDIHDR* lpMidiOutHdr, UINT uSize) {
#ifdef _DAWRELEASE
	if (hMidiOut != OMDummy) return MMmidiOutPrepareHeader(hMidiOut, lpMidiOutHdr, uSize);
#endif
	return PrepareLongData(lpMidiOutHdr, uSize);
}

MMRESULT WINAPI KDMAPI_midiOutUnprepareHeader(HMIDIOUT hMidiOut, MIDIHDR* lpMidiOutHdr, UINT uSize) {
#ifdef _DAWRELEASE
	if (hMidiOut != OMDummy) return MMmidiOutUnprepareHeader(hMidiOut, lpMidiOutHdr, uSize);
#endif
	return UnprepareLongData(lpMidiOutHdr, uSize);
}

MMRESULT WINAPI KDMAPI_midiOutLongMsg(HMIDIOUT hMidiOut, MIDIHDR* lpMidiOutHdr, UINT uSize) {
#ifdef _DAWRELEASE
	if (hMidiOut != OMDummy) return MMmidiOutLongMsg(hMidiOut, lpMidiOutHdr, uSize);
#endif
	// Forward the buffer to KDMAPI
	MMRESULT Ret = SendDirectLongData(lpMidiOutHdr, uSize);

	// Inform the app that the driver successfully received the long message (Required for vanBasco to work), and return the MMRESULT
	RCF(MM_MOM_DONE, hMidiOut, lpMidiOutHdr);

	return Ret;
}

MMRESULT WINAPI KDMAPI_midiOutCachePatches(HMIDIOUT hMidiOut, UINT wPatch, LPWORD lpPatchArray, UINT wFlags) {
#ifdef _DAWRELEASE
	if (hMidiOut != OMDummy) return MMmidiOutCachePatches(hMidiOut, wPatch, lpPatchArray, wFlags);
#endif
	// Dummy, OmniMIDI uses SoundFonts
	return MMSYSERR_NOERROR;
}

MMRESULT WINAPI KDMAPI_midiOutCacheDrumPatches(HMIDIOUT hMidiOut, UINT wPatch, LPWORD lpKeyArray, UINT wFlags) {
#ifdef _DAWRELEASE
	if (hMidiOut != OMDummy) return MMmidiOutCacheDrumPatches(hMidiOut, wPatch, lpKeyArray, wFlags);
#endif
	// Dummy, OmniMIDI uses SoundFonts
	return MMSYSERR_NOERROR;
}

MMRESULT WINAPI KDMAPI_midiOutMessage(HMIDIOUT hMidiOut, UINT uMsg, DWORD_PTR dw1, DWORD_PTR dw2) {
#ifdef _DAWRELEASE
	if (hMidiOut != OMDummy) return MMmidiOutMessage(hMidiOut, uMsg, dw1, dw2);
#endif
	// Dummy, OmniMIDI uses SoundFonts
	return MMSYSERR_NOERROR;
}

MMRESULT WINAPI KDMAPI_midiOutSetVolume(HMIDIOUT hMidiOut, DWORD dwVolume) {
#ifdef _DAWRELEASE
	if (hMidiOut != OMDummy) return MMmidiOutSetVolume(hMidiOut, dwVolume);
#endif
	// Set the volume, even though it won't work lol
	return modMessage(0, MODM_SETVOLUME, OMUser, dwVolume, 0);
}

MMRESULT WINAPI KDMAPI_midiOutGetVolume(HMIDIOUT hMidiOut, LPDWORD lpdwVolume) {
#ifdef _DAWRELEASE
	if (hMidiOut != OMDummy) return MMmidiOutGetVolume(hMidiOut, lpdwVolume);
#endif
	// Get the volume
	return modMessage(0, MODM_GETVOLUME, OMUser, lpdwVolume, 0);
}

MMRESULT WINAPI KDMAPI_midiOutGetID(HMIDIOUT hMidiOut, LPUINT puDeviceID) {
#ifdef _DAWRELEASE
	if (hMidiOut != OMDummy) {
		UINT Dummy = 0;
		MMRESULT ret = MMmidiOutGetID(hMidiOut, &Dummy);
		if (ret == MMSYSERR_NOERROR) {
			switch (Dummy) {
			case MIDI_MAPPER:
				*puDeviceID = MIDI_MAPPER;
				break;
			default:
				*puDeviceID = Dummy - 1;
				break;
			}
		}
		return ret;
	}
#endif
	// Dummy, device is always 0
	*puDeviceID = 0;
	return MMSYSERR_NOERROR;
}

UINT WINAPI KDMAPI_mmsystemGetVersion(void) {
	// Dummy, not needed
	return 0x0600U;
}

MMRESULT WINAPI KDMAPI_midiStreamOpen(LPHMIDISTRM lphStream, LPUINT puDeviceID, DWORD cMidi, DWORD_PTR dwCallback, DWORD_PTR dwCallbackInstance, DWORD fdwOpen) {
	MMRESULT retval = MMSYSERR_NOERROR;

	if (cMidi != 1 || lphStream == NULL || puDeviceID == NULL) return MMSYSERR_INVALPARAM;

#ifdef _DAWRELEASE
	switch (*puDeviceID) {
		// The app requested KDMAPI, break and initialize it outside of the switch
		case KDMAPI_UDID :
			break;

			// Else, just forward the call to WinMM
		default: {
			UINT devid = *puDeviceID - 1;
			retval = MMmidiStreamOpen(lphStream, &devid, cMidi, dwCallback, dwCallbackInstance, fdwOpen);
			return retval;
		}
	}
#endif
	if (!OMAlreadyInit) {
		// Setup the Callback
		if (!ICF((*lphStream), dwCallback, dwCallbackInstance, &OMUser, fdwOpen | 0x00000002L))
		{
			MessageBox(NULL, "ICF failed!", "KDMAPI ERROR", MB_SYSTEMMODAL | MB_ICONERROR);
			return MMSYSERR_INVALPARAM;
		}

		// Close any stream, just to be safe
		TerminateKDMAPIStream();

		// Initialize a dummy out device
		*lphStream = OMDummy;

		// Initialize MIDI out
		if (!InitializeKDMAPIStream())
			return MMSYSERR_ALLOCATED;

		DriverSettings(0xFFFFF, NULL, NULL, NULL);

		RCF(MM_MOM_OPEN, *lphStream, 0);

		OMAlreadyInit = TRUE;
		return MMSYSERR_NOERROR;
	}
	else MessageBox(NULL, "OmniMIDI has been already initialized via KDMAPI! Can not initialize it again!", "KDMAPI ERROR", MB_SYSTEMMODAL | MB_ICONERROR);

	return MMSYSERR_ALLOCATED;
}

MMRESULT WINAPI KDMAPI_midiStreamClose(HMIDISTRM hStream) {
#ifdef _DAWRELEASE
	if (hStream != OMDummy) return MMmidiStreamClose(hStream);
#endif
	// Terminate CookedPlayer, free up hStream and return 0
	if (OMAlreadyInit) {
		if (TerminateKDMAPIStream())
			RCF(MM_MOM_CLOSE, hStream, 0);

		OMAlreadyInit = FALSE;
		return MMSYSERR_NOERROR;
	}

	return MMSYSERR_INVALHANDLE;
}

MMRESULT WINAPI KDMAPI_midiStreamOut(HMIDISTRM hStream, LPMIDIHDR lpMidiOutHdr, UINT uSize) {
#ifdef _DAWRELEASE
	if (hStream != OMDummy) return MMmidiStreamOut(hStream, lpMidiOutHdr, uSize);
#endif
	// Give stream data to CookedPlayer
	return modMessage(0, MODM_STRMDATA, &OMUser, lpMidiOutHdr, uSize);
}

MMRESULT WINAPI KDMAPI_midiStreamPause(HMIDISTRM hStream) {
#ifdef _DAWRELEASE
	if (hStream != OMDummy) return MMmidiStreamPause(hStream);
#endif
	// Pause CookedPlayer
	return modMessage(0, MODM_PAUSE, &OMUser, 0, 0);
}

MMRESULT WINAPI KDMAPI_midiStreamRestart(HMIDISTRM hStream) {
#ifdef _DAWRELEASE
	if (hStream != OMDummy) return MMmidiStreamRestart(hStream);
#endif
	// Play CookedPlayer
	return modMessage(0, MODM_RESTART, &OMUser, 0, 0);
}

MMRESULT WINAPI KDMAPI_midiStreamStop(HMIDISTRM hStream) {
#ifdef _DAWRELEASE
	if (hStream != OMDummy) return MMmidiStreamStop(hStream);
#endif
	// Stop CookedPlayer
	return modMessage(0, MODM_STOP, &OMUser, 0, 0);
}

MMRESULT WINAPI KDMAPI_midiStreamProperty(HMIDISTRM hStream, LPBYTE lppropdata, DWORD dwProperty) {
#ifdef _DAWRELEASE
	if (hStream != OMDummy) return MMmidiStreamProperty(hStream, lppropdata, dwProperty);
#endif
	// Pass the prop. data to modMessage
	return modMessage(0, MODM_PROPERTIES, &OMUser, lppropdata, dwProperty);
}

MMRESULT WINAPI KDMAPI_midiStreamPosition(HMIDISTRM hStream, LPMMTIME pmmt, UINT cbmmt) {
#ifdef _DAWRELEASE
	if (hStream != OMDummy) return MMmidiStreamPosition(hStream, pmmt, cbmmt);
#endif
	// Give CookedPlayer position to MIDI app
	return modMessage(0, MODM_GETPOS, &OMUser, pmmt, cbmmt);
}

VOID WINAPI KDMAPI_poweredByKeppy() {
	MessageBox(NULL, "With love by Keppy.", "Windows Multimedia Wrapper", MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL);
}

DWORD WINAPI FINAL_timeGetTime() {
	return TGT();
}