#include <Windows.h>
#include <string.h>
#include <stdio.h>
#include <locale.h>
#include "mmddk.h"
#include <time.h>

#define KDMAPI_UDID			0		// KDMAPI default uDeviceID
#define KDMAPI_NOTLOADED	0		// KDMAPI is not loaded
#define KDMAPI_LOADED		4		// KDMAPI is loaded

#define LONGMSG_MAXSIZE		65535	// Maximum size for a long message/MIDIHDR

#define LOCK_VM_IN_WORKING_SET 1
#define LOCK_VM_IN_RAM 2

// We don't need MMDDK lel
#define MODM_GETVOLUME		10
#define MODM_SETVOLUME		11

#pragma once

// Required KDMAPI version
#define REQ_MAJOR	1
#define REQ_MINOR	30
#define REQ_BUILD	0
#define REQ_REV		51

// OM funcs
static BOOL IsCallbackWindow = FALSE;																					// WMMC

// Dummy pointer, used for KDMAPI Output
static HMIDIOUT OMDummy1 = 0x10001;	
static HMIDIOUT OMDummy2 = 0x10002;	
static HMIDIOUT OMDummy3 = 0x10003;	
static HMIDIOUT OMDummy4 = 0x10004;

static MMRESULT(WINAPI* mM)(UINT uDeviceID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dwParam1, DWORD_PTR dwParam2) = 0;	// modMessage

typedef unsigned __int64 QWORD;
typedef long NTSTATUS;

#ifdef _DAWRELEASE
// WinMM funcs, just replace MM with "midiOut" to get the real version
static UINT(WINAPI* MMOutGetNumDevs)(void) = 0;
static MMRESULT(WINAPI* MMOutGetDevCapsW)(UINT_PTR uDeviceID, LPMIDIOUTCAPSW lpCaps, UINT uSize) = 0;
static MMRESULT(WINAPI* MMOutOpen)(LPHMIDIOUT lphMidiOut, UINT uDeviceID, DWORD_PTR dwCallback, DWORD_PTR dwCallbackInstance, DWORD dwFlags) = 0;
static MMRESULT(WINAPI* MMOutClose)(HMIDIOUT hMidiOut) = 0;
static MMRESULT(WINAPI* MMOutReset)(HMIDIOUT hMidiOut) = 0;
static MMRESULT(WINAPI* MMOutShortMsg)(HMIDIOUT hMidiOut, DWORD dwMsg) = 0;
static MMRESULT(WINAPI* MMOutPrepareHeader)(HMIDIOUT hMidiOut, MIDIHDR* lpMidiOutHdr, UINT uSize) = 0;
static MMRESULT(WINAPI* MMOutUnprepareHeader)(HMIDIOUT hMidiOut, MIDIHDR* lpMidiOutHdr, UINT uSize) = 0;
static MMRESULT(WINAPI* MMOutLongMsg)(HMIDIOUT hMidiOut, MIDIHDR* lpMidiOutHdr, UINT uSize) = 0;
static MMRESULT(WINAPI* MMOutCachePatches)(HMIDIOUT hMidiOut, UINT wPatch, WORD* lpPatchArray, UINT wFlags) = 0;
static MMRESULT(WINAPI* MMOutCacheDrumPatches)(HMIDIOUT hMidiOut, UINT wPatch, WORD* lpKeyArray, UINT wFlags) = 0;
static MMRESULT(WINAPI* MMOutMessage)(HMIDIOUT hMidiOut, UINT uMsg, DWORD_PTR dw1, DWORD_PTR dw2) = 0;
static MMRESULT(WINAPI* MMOutSetVolume)(HMIDIOUT hMidiOut, DWORD dwVolume) = 0;
static MMRESULT(WINAPI* MMOutGetVolume)(HMIDIOUT hMidiOut, LPDWORD lpdwVolume) = 0;
static MMRESULT(WINAPI* MMOutGetID)(HMIDIOUT hMidiOut, LPUINT puDeviceID) = 0;
#endif

// Callback, used for old apps that require one
static DWORD_PTR WMMCI;
static void(CALLBACK* WMMC)(HMIDIOUT hmo, DWORD wMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2) = 0;

static DWORD OMDI = KDMAPI_NOTLOADED;
static HMODULE OM = NULL;

// WinNT Kernel funcs
DWORD(WINAPI* TGT)() = 0;																		// timeGetTime
HMODULE NTDLL = NULL;
ULONGLONG TickStart = 0;																		// For TGT64
NTSTATUS(NTAPI* NQST)(QWORD*) = 0;																// NtQuerySystemTime

// Stock WinMM funcs
#include "WinMM.h"

BOOL InitializeOMDirectAPI() {
	OM = LoadLibrary("Shakra.dll");
	if (OM) {
		mM = (MMRESULT*)GetProcAddress(OM, "modMessage");				// Other stuff from the driver

		if (!mM) {
			// One of the functions failed to load, exit
			MessageBox(
				NULL,
				"Failed to initialize Shakra's modMessage function!\nPress OK to quit.",
				"Keppy's Direct MIDI API",
				MB_ICONERROR | MB_OK | MB_SYSTEMMODAL
			);
			exit(0x57);
		}

		TGT = WINMM_timeGetTime;
		return TRUE;
	}

	// Failed, OmniMIDI is not available, exit the program
	MessageBox(
		NULL,
		"Failed to initialize Shakra!\n\nThis patch will not work with other MIDI drivers.\n\nPress OK to exit.",
		"KDMAPI",
		MB_ICONERROR | MB_OK | MB_SYSTEMMODAL
	);

	return FALSE;
}

BOOL WINAPI DllMain(HINSTANCE hInstDLL, DWORD fdwReason, LPVOID fImpLoad) {
	if (fdwReason == DLL_PROCESS_ATTACH) {
		if (InitializeWinMM())
			if (InitializeOMDirectAPI())
				return TRUE;

		return FALSE;
	}
	return TRUE;
}


UINT WINAPI KDMAPI_midiOutGetNumDevs(void) {
#ifdef _DAWRELEASE
	// Return the first port which is KDMAPI, plus the others
	return MMOutGetNumDevs() + 1;
#else
	return KDMAPI_LOADED;
#endif
}

MMRESULT WINAPI KDMAPI_midiOutGetDevCapsW(UINT_PTR uDeviceID, LPMIDIOUTCAPSW lpCaps, UINT uSize) {
	MIDIOUTCAPSW myCapsW;
	DWORD CapsSupport = MIDICAPS_VOLUME;

	wchar_t SynthName[MAXPNAMELEN];
	WORD VID = 0xFFFF;
	WORD PID = 0x000A;
	DWORD DM = MOD_SWSYNTH;

	// Read key
	HKEY hKey;
	long lResult;
	DWORD dwType;
	DWORD dwSize = sizeof(DWORD);
	DWORD dwSizeW = sizeof(SynthName);

	// Return the output device, but Unicode (UTF-8)
	if (lpCaps == NULL) return MMSYSERR_INVALPARAM;

#ifdef _DAWRELEASE
	switch (uDeviceID) {
	case MIDI_MAPPER:
		return MMOutGetDevCapsW(uDeviceID, lpCaps, uSize);

	case KDMAPI_UDID:
		memset(SynthName, 0, MAXPNAMELEN);
		swprintf_s(SynthName, MAXPNAMELEN, L"Shakra Alpha D%d (DAW)\0", uDeviceID);
		DM = MOD_WAVETABLE;

		// ChecOM done, assign device type
		DM = SNT[ST];

		// Assign values
		myCapsW.wMid = VID;
		myCapsW.wPid = PID;
		memcpy(myCapsW.szPname, SynthName, sizeof(SynthName));
		myCapsW.wVoices = 0xFFFF;
		myCapsW.wNotes = 0x0000;
		myCapsW.wTechnology = DM;
		myCapsW.wChannelMask = 0xFFFF;
		myCapsW.dwSupport = MIDICAPS_VOLUME;

		// Copy values to pointer, and return 0
		memcpy(lpCaps, &myCapsW, min(uSize, sizeof(myCapsW)));

		return MMSYSERR_NOERROR;

	default:
		return MMOutGetDevCapsW(uDeviceID - 1, lpCaps, uSize);
	}
#else
	if (uDeviceID > 3) return MMSYSERR_BADDEVICEID;

	memset(SynthName, 0, MAXPNAMELEN);
	swprintf_s(SynthName, MAXPNAMELEN, L"Shakra Alpha D%d (MM)\0", uDeviceID);
	DM = MOD_WAVETABLE;

	// Assign values
	myCapsW.wMid = VID;
	myCapsW.wPid = PID;
	memcpy(myCapsW.szPname, SynthName, sizeof(SynthName));
	myCapsW.wVoices = 0xFFFF;
	myCapsW.wNotes = 0x0000;
	myCapsW.wTechnology = DM;
	myCapsW.wChannelMask = 0xFFFF;
	myCapsW.dwSupport = MIDICAPS_VOLUME;

	// Copy values to pointer, and return 0
	memcpy(lpCaps, &myCapsW, min(uSize, sizeof(myCapsW)));

	return MMSYSERR_NOERROR;
#endif
}

MMRESULT WINAPI KDMAPI_midiOutGetDevCapsA(UINT_PTR uDeviceID, LPMIDIOUTCAPSA lpCaps, UINT uSize) {
	// Return the output device, but ASCII/Multibyte
	if (lpCaps == NULL) return MMSYSERR_INVALPARAM;

	// Parse info in Unicode
	MIDIOUTCAPSW myCapsW;
	UINT ret;

#ifdef _DAWRELEASE
	switch (uDeviceID) {
	case MIDI_MAPPER:
		ret = MMOutGetDevCapsW(uDeviceID, &myCapsW, sizeof(myCapsW));
	case KDMAPI_UDID:
		ret = KDMAPI_midiOutGetDevCapsW(KDMAPI_UDID, &myCapsW, sizeof(myCapsW));
	default:
		ret = MMOutGetDevCapsW(uDeviceID - 1, &myCapsW, sizeof(myCapsW));
	}

	// Translate them to ASCII/Multibyte
	if (ret == MMSYSERR_NOERROR) {
		MIDIOUTCAPSA myCapsA;
		myCapsA.wMid = myCapsW.wMid;
		myCapsA.wPid = myCapsW.wPid;
		myCapsA.vDriverVersion = myCapsW.vDriverVersion;
		WideCharToMultiByte(CP_ACP, 0, myCapsW.szPname, -1, myCapsA.szPname,
			sizeof(myCapsA.szPname), NULL, NULL);
		myCapsA.wTechnology = myCapsW.wTechnology;
		myCapsA.wVoices = myCapsW.wVoices;
		myCapsA.wNotes = myCapsW.wNotes;
		myCapsA.wChannelMask = myCapsW.wChannelMask;
		myCapsA.dwSupport = myCapsW.dwSupport;
		memcpy(lpCaps, &myCapsA, min(uSize, sizeof(myCapsA)));
	}
	return ret;
#else
	ret = KDMAPI_midiOutGetDevCapsW(uDeviceID, &myCapsW, sizeof(myCapsW));

	// Translate them to ASCII/Multibyte
	if (ret == MMSYSERR_NOERROR) {
		MIDIOUTCAPSA myCapsA;
		myCapsA.wMid = myCapsW.wMid;
		myCapsA.wPid = myCapsW.wPid;
		myCapsA.vDriverVersion = myCapsW.vDriverVersion;
		WideCharToMultiByte(CP_ACP, 0, myCapsW.szPname, -1, myCapsA.szPname,
			sizeof(myCapsA.szPname), NULL, NULL);
		myCapsA.wTechnology = myCapsW.wTechnology;
		myCapsA.wVoices = myCapsW.wVoices;
		myCapsA.wNotes = myCapsW.wNotes;
		myCapsA.wChannelMask = myCapsW.wChannelMask;
		myCapsA.dwSupport = myCapsW.dwSupport;
		memcpy(lpCaps, &myCapsA, min(uSize, sizeof(myCapsA)));
	}
	return ret;
#endif
}

#ifdef _DAWRELEASE
MMRESULT WINAPI STOCK_midiOutShortMsg(HMIDIOUT hMidiOut, DWORD dwMsg) {
	MMRESULT ret = MMOutShortMsg(hMidiOut, dwMsg);
	return ret;
}
#endif

MMRESULT WINAPI KDMAPI_midiOutShortMsg(HMIDIOUT hMidiOut, DWORD dwMsg) {
	if (hMidiOut == OMDummy1) {
		// Reset the MIDI channels
		return mM(0, MODM_DATA, 0, dwMsg, 0);
	}
	else if (hMidiOut == OMDummy2) {
		// Reset the MIDI channels
		return mM(1, MODM_DATA, 0, dwMsg, 0);
	}
	else if (hMidiOut == OMDummy3) {
		// Reset the MIDI channels
		return mM(2, MODM_DATA, 0, dwMsg, 0);
	}
	else if (hMidiOut == OMDummy4) {
		// Reset the MIDI channels
		return mM(3, MODM_DATA, 0, dwMsg, 0);
	}
#ifdef _DAWRELEASE
	else return MMOutShortMsg(hMidiOut, dwMsg);
#endif
	else return MMSYSERR_BADDEVICEID;
}

MMRESULT WINAPI KDMAPI_midiOutOpen(LPHMIDIOUT lphmo, UINT_PTR uDeviceID, DWORD_PTR dwCallback, DWORD_PTR dwCallbackInstance, DWORD dwFlags) {
	MMRESULT retval = MMSYSERR_INVALPARAM;

#ifdef _DAWRELEASE
	// If above this device ID, return an error
	if (uDeviceID > 0xFFFFFFFE) return MMSYSERR_BADDEVICEID;

	switch (uDeviceID) {
		// If it's requesting for the MIDI mapper, forward the call to WinMM/MIDIMAP
	case MIDI_MAPPER:
		retval = MMOutOpen(lphmo, uDeviceID, dwCallback, dwCallbackInstance, dwFlags);
		return retval;

		// If it's asking for OmniMIDI, do everything in-house
	case KDMAPI_UDID:
		// Initialize the API
		InitializeOMDirectAPI();

		// Initialize a dummy out device
		*lphmo = OMDummy;

		// Setup the Callback (If there's one) - NEEDED FOR VANBASCO!
		// If dwflags is CALLBACK_EVENT, then skip, since it's not needed. (Java pls)
		if ((dwFlags != CALLBACK_NULL) && (dwFlags != CALLBACK_EVENT)) {
			WMMC = (void*)dwCallback;
			WMMCI = dwCallbackInstance;

			IsCallbackWindow = (dwFlags == CALLBACK_WINDOW);
			if (WMMC) WMMC((*lphmo), IsCallbackWindow ? MM_MOM_OPEN : MOM_OPEN, WMMCI, 0, 0);
		}

		return MMSYSERR_NOERROR;

		// Else, just forward the call to WinMM
	default:
		retval = MMOutOpen(lphmo, uDeviceID - 1, dwCallback, dwCallbackInstance, dwFlags);
		return retval;
	}
#else
	MIDIOPENDESC desc;

	// Initialize a dummy out device
	switch (uDeviceID) {
	case 0:
		*lphmo = (HMIDI)OMDummy1;
		desc.hMidi = lphmo;
		break;
	case 1:
		*lphmo = (HMIDI)OMDummy2;
		desc.hMidi = lphmo;
		break;
	case 2:
		*lphmo = (HMIDI)OMDummy3;
		desc.hMidi = lphmo;
		break;
	case 3:
		*lphmo = (HMIDI)OMDummy4;
		desc.hMidi = lphmo;
		break;
	}

	desc.dwCallback = dwCallback;
	desc.dwInstance = dwCallbackInstance;
	desc.dnDevNode = NULL;
	desc.cIds = NULL;

	return mM(uDeviceID, 3, 0, &desc, (DWORD_PTR)dwFlags);
#endif
}

MMRESULT WINAPI KDMAPI_midiOutClose(HMIDIOUT hMidiOut) {
	if (hMidiOut == OMDummy1) {
		// Reset the MIDI channels
		return mM(0, 4, 0, 0, 0);
	}
	else if (hMidiOut == OMDummy2) {
		// Reset the MIDI channels
		return mM(1, 4, 0, 0, 0);
	}
	else if (hMidiOut == OMDummy3) {
		// Reset the MIDI channels
		return mM(2, 4, 0, 0, 0);
	}
	else if (hMidiOut == OMDummy4) {
		// Reset the MIDI channels
		return mM(3, 4, 0, 0, 0);
	}
#ifdef _DAWRELEASE
	else return MMOutClose(hMidiOut);
#else
	else return MMSYSERR_NOERROR;
#endif
}

MMRESULT WINAPI KDMAPI_midiOutReset(HMIDIOUT hMidiOut) {
	if (hMidiOut == OMDummy1) {
		// Reset the MIDI channels
		return mM(0, 9, 0, 0, 0);
	}
	else if (hMidiOut == OMDummy2) {
		// Reset the MIDI channels
		return mM(1, 9, 0, 0, 0);
	}
	else if (hMidiOut == OMDummy3) {
		// Reset the MIDI channels
		return mM(2, 9, 0, 0, 0);
	}
	else if (hMidiOut == OMDummy4) {
		// Reset the MIDI channels
		return mM(3, 9, 0, 0, 0);
	}
#ifdef _DAWRELEASE
	else return MMOutReset(hMidiOut);
#else
	else return MMSYSERR_NOERROR;
#endif
}

MMRESULT WINAPI KDMAPI_midiOutPrepareHeader(HMIDIOUT hMidiOut, MIDIHDR* lpMidiOutHdr, UINT uSize) {
	if (!hMidiOut) return MMSYSERR_INVALHANDLE;															// The device doesn't exist, invalid handle
	if (!lpMidiOutHdr || sizeof(lpMidiOutHdr->lpData) > LONGMSG_MAXSIZE) return MMSYSERR_INVALPARAM;	// The buffer doesn't exist or is too big, invalid parameter

	// Mark the buffer as prepared, and return MMSYSERR_NOERROR
	lpMidiOutHdr->dwFlags |= MHDR_PREPARED;

	return MMSYSERR_NOERROR;
}

MMRESULT WINAPI KDMAPI_midiOutUnprepareHeader(HMIDIOUT hMidiOut, MIDIHDR* lpMidiOutHdr, UINT uSize) {
	if (!hMidiOut) return MMSYSERR_INVALHANDLE;									// The device doesn't exist, invalid handle
	if (!lpMidiOutHdr) return MMSYSERR_INVALPARAM;								// The buffer doesn't exist, invalid parameter
	if (!(lpMidiOutHdr->dwFlags & MHDR_PREPARED)) return MMSYSERR_NOERROR;		// Already unprepared, everything is fine
	if (lpMidiOutHdr->dwFlags & MHDR_INQUEUE) return MIDIERR_STILLPLAYING;		// The buffer is currently being played from the driver, cannot unprepare

	// Mark the buffer as unprepared
	lpMidiOutHdr->dwFlags &= ~MHDR_PREPARED;

	return MMSYSERR_NOERROR;
}

MMRESULT WINAPI KDMAPI_midiOutLongMsg(HMIDIOUT hMidiOut, MIDIHDR* lpMidiOutHdr, UINT uSize) {
	MMRESULT Ret = MMSYSERR_NOERROR;

	// Check if the MIDIHDR buffer is valid	
	if (!hMidiOut) return MMSYSERR_INVALHANDLE;										// The device doesn't exist, invalid handle

	if (!lpMidiOutHdr) return MMSYSERR_INVALPARAM;									// The buffer doesn't exist, invalid parameter
	if (!(lpMidiOutHdr->dwFlags & MHDR_PREPARED)) return MIDIERR_UNPREPARED;		// The buffer is not prepared
	if (lpMidiOutHdr->dwFlags & MHDR_INQUEUE) return MIDIERR_NOTREADY;				// Another buffer is being played at the moment, the driver is not ready

	if (hMidiOut == OMDummy1) {
		// Reset the MIDI channels
		return mM(0, 8, 0, (DWORD_PTR)lpMidiOutHdr, (DWORD_PTR)(lpMidiOutHdr->dwBufferLength));
	}
	else if (hMidiOut == OMDummy2) {
		// Reset the MIDI channels
		return mM(1, 8, 0, (DWORD_PTR)lpMidiOutHdr, (DWORD_PTR)(lpMidiOutHdr->dwBufferLength));
	}
	else if (hMidiOut == OMDummy3) {
		// Reset the MIDI channels
		return mM(2, 8, 0, (DWORD_PTR)lpMidiOutHdr, (DWORD_PTR)(lpMidiOutHdr->dwBufferLength));
	}
	else if (hMidiOut == OMDummy4) {
		// Reset the MIDI channels
		return mM(3, 8, 0, (DWORD_PTR)lpMidiOutHdr, (DWORD_PTR)(lpMidiOutHdr->dwBufferLength));
	}
#ifdef _DAWRELEASE
	else return MMOutLongMsg(hMidiOut, lpMidiOutHdr, uSize);
#else
	else return MMSYSERR_NOERROR;
#endif
}

MMRESULT WINAPI KDMAPI_midiOutCachePatches(HMIDIOUT hMidiOut, UINT wPatch, WORD* lpPatchArray, UINT wFlags) {
#ifdef _DAWRELEASE
	if (hMidiOut != OMDummy1 || hMidiOut != OMDummy2 || hMidiOut != OMDummy3 || hMidiOut != OMDummy4) 
		return MMOutCachePatches(hMidiOut, wPatch, lpPatchArray, wFlags);
#endif
	// Dummy, not needed
	return MMSYSERR_NOTSUPPORTED;
}

MMRESULT WINAPI KDMAPI_midiOutCacheDrumPatches(HMIDIOUT hMidiOut, UINT wPatch, WORD* lpKeyArray, UINT wFlags) {
#ifdef _DAWRELEASE
	if (hMidiOut != OMDummy1 || hMidiOut != OMDummy2 || hMidiOut != OMDummy3 || hMidiOut != OMDummy4)
		return MMOutCacheDrumPatches(hMidiOut, wPatch, lpKeyArray, wFlags);
#endif
	// Dummy, not needed
	return MMSYSERR_NOTSUPPORTED;
}

MMRESULT WINAPI KDMAPI_midiOutMessage(HMIDIOUT hMidiOut, UINT uMsg, DWORD_PTR dw1, DWORD_PTR dw2) {
#ifdef _DAWRELEASE
	if (hMidiOut != OMDummy1 || hMidiOut != OMDummy2 || hMidiOut != OMDummy3 || hMidiOut != OMDummy4)
		return MMOutMessage(hMidiOut, uMsg, dw1, dw2);
#endif
	// Dummy, not needed
	return MMSYSERR_NOTSUPPORTED;
}

MMRESULT WINAPI KDMAPI_midiOutSetVolume(HMIDIOUT hMidiOut, DWORD dwVolume) {
#ifdef _DAWRELEASE
	if (hMidiOut != OMDummy1 || hMidiOut != OMDummy2 || hMidiOut != OMDummy3 || hMidiOut != OMDummy4)
		return MMOutSetVolume(hMidiOut, dwVolume);
#endif
	// Set the volume, even though it won't work lol
	return mM(0, MODM_SETVOLUME, dwVolume, 0, 0);
}

MMRESULT WINAPI KDMAPI_midiOutGetVolume(HMIDIOUT hMidiOut, LPDWORD lpdwVolume) {
#ifdef _DAWRELEASE
	if (hMidiOut != OMDummy1 || hMidiOut != OMDummy2 || hMidiOut != OMDummy3 || hMidiOut != OMDummy4)
		return MMOutGetVolume(hMidiOut, lpdwVolume);
#endif
	// Get the volume
	return mM(0, MODM_GETVOLUME, (DWORD_PTR)lpdwVolume, 0, 0);
}

MMRESULT WINAPI KDMAPI_midiOutGetID(HMIDIOUT hMidiOut, LPUINT puDeviceID) {
#ifdef _DAWRELEASE
	if (hMidiOut != OMDummy1 || hMidiOut != OMDummy2 || hMidiOut != OMDummy3 || hMidiOut != OMDummy4) {
		UINT Dummy = 0;
		MMRESULT ret = MMOutGetID(hMidiOut, &Dummy);
		if (ret == MMSYSERR_NOERROR) puDeviceID = (Dummy == MIDI_MAPPER) ? MIDI_MAPPER : Dummy - 1;
		return ret;
	}
#endif
	// Dummy, device is always 0
	puDeviceID = 0;
	return MMSYSERR_NOERROR;
}

MMRESULT WINAPI KDMAPI_midiStreamOpen(LPHMIDISTRM lphStream, LPUINT puDeviceID, DWORD cMidi, DWORD_PTR dwCallback, DWORD_PTR dwCallbackInstance, DWORD fdwOpen) {
	return MMSYSERR_NOTSUPPORTED;
}

MMRESULT WINAPI KDMAPI_midiStreamClose(HMIDISTRM hStream) {
	return MMSYSERR_NOTSUPPORTED;
}

MMRESULT WINAPI KDMAPI_midiStreamOut(HMIDISTRM hStream, LPMIDIHDR lpMidiOutHdr, UINT uSize) {
	return MMSYSERR_NOTSUPPORTED;
}

MMRESULT WINAPI KDMAPI_midiStreamPause(HMIDISTRM hStream) {
	return MMSYSERR_NOTSUPPORTED;
}

MMRESULT WINAPI KDMAPI_midiStreamRestart(HMIDISTRM hStream) {
	return MMSYSERR_NOTSUPPORTED;
}

MMRESULT WINAPI KDMAPI_midiStreamStop(HMIDISTRM hStream) {
	return MMSYSERR_NOTSUPPORTED;
}

MMRESULT WINAPI KDMAPI_midiStreamProperty(HMIDISTRM hStream, LPBYTE lppropdata, DWORD dwProperty) {
	return MMSYSERR_NOTSUPPORTED;
}

MMRESULT WINAPI KDMAPI_midiStreamPosition(HMIDISTRM hStream, LPMMTIME pmmt, UINT cbmmt) {
	return MMSYSERR_NOTSUPPORTED;
}

UINT WINAPI KDMAPI_mmsystemGetVersion() {
	// Dummy, not needed
	return 0x0502U;
}

VOID WINAPI KDMAPI_poweredByKeppy() {
	MessageBox(NULL, "Haha got u c:", "Windows Multimedia Wrapper", MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL);
}

DWORD WINAPI FINAL_timeGetTime() {
	return TGT();
}