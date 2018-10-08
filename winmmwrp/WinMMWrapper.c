#include <Windows.h>
#include <stdio.h>
#include <mmddk.h>
#include <Winuser.h>

#define KDMAPI_UDID			0		// KDMAPI default uDeviceID
#define KDMAPI_NOTLOADED	0		// KDMAPI is not loaded
#define KDMAPI_LOADED		1		// KDMAPI is loaded

#define LONGMSG_MAXSIZE		65535	// Maximum size for a long message/MIDIHDR

#pragma once

// Required KDMAPI version
#define REQ_MAJOR	1
#define REQ_MINOR	30
#define REQ_BUILD	0
#define REQ_REV		51

// OM funcs
static BOOL IsCallbackWindow = FALSE;																					// WMMC
static HMIDIOUT OMDummy = 0x10001;																						// Dummy pointer, used for KDMAPI Output
static MMRESULT(WINAPI*SDD)(DWORD msg) = 0;																				// SendDirectData
static MMRESULT(WINAPI*SDLD)(MIDIHDR* IIMidiHdr) = 0;																	// SendDirectLongData
static MMRESULT(WINAPI*mM)(UINT uDeviceID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dwParam1, DWORD_PTR dwParam2) = 0;	// modMessage
static BOOL(WINAPI*RKDMAPIV)(DWORD *Major, DWORD *Minor, DWORD *Build, DWORD *Revision) = 0;							// ReturnKDMAPIVer
static void(WINAPI*IOMS)(void) = 0;																						// InitializeKDMAPIStream
static void(WINAPI*TOMS)(void) = 0;																						// TerminateKDMAPIStream
static void(WINAPI*ROMS)(void) = 0;																						// ResetKDMAPIStream
static BOOL(WINAPI*IKDMAPIA)(void) = 0;																					// IsKDMAPIAvailable

#ifdef _DAWRELEASE
// WinMM funcs, just replace MM with "midiOut" to get the real version
static UINT(WINAPI*MMOutGetNumDevs)(void) = 0;
static MMRESULT(WINAPI*MMOutGetDevCapsW)(UINT_PTR uDeviceID, LPMIDIOUTCAPSW lpCaps, UINT uSize) = 0;
static MMRESULT(WINAPI*MMOutOpen)(LPHMIDIOUT lphMidiOut, UINT uDeviceID, DWORD_PTR dwCallback, DWORD_PTR dwCallbackInstance, DWORD dwFlags) = 0;
static MMRESULT(WINAPI*MMOutClose)(HMIDIOUT hMidiOut) = 0;
static MMRESULT(WINAPI*MMOutReset)(HMIDIOUT hMidiOut) = 0;
static MMRESULT(WINAPI*MMOutShortMsg)(HMIDIOUT hMidiOut, DWORD dwMsg) = 0;
static MMRESULT(WINAPI*MMOutPrepareHeader)(HMIDIOUT hMidiOut, MIDIHDR* lpMidiOutHdr, UINT uSize) = 0;
static MMRESULT(WINAPI*MMOutUnprepareHeader)(HMIDIOUT hMidiOut, MIDIHDR* lpMidiOutHdr, UINT uSize) = 0;
static MMRESULT(WINAPI*MMOutLongMsg)(HMIDIOUT hMidiOut, MIDIHDR* lpMidiOutHdr, UINT uSize) = 0;
static MMRESULT(WINAPI*MMOutCachePatches)(HMIDIOUT hMidiOut, UINT wPatch, WORD *lpPatchArray, UINT wFlags) = 0;
static MMRESULT(WINAPI*MMOutCacheDrumPatches)(HMIDIOUT hMidiOut, UINT wPatch, WORD *lpKeyArray, UINT wFlags) = 0;
static MMRESULT(WINAPI*MMOutMessage)(HMIDIOUT hMidiOut, UINT uMsg, DWORD_PTR dw1, DWORD_PTR dw2) = 0;
static MMRESULT(WINAPI*MMOutSetVolume)(HMIDIOUT hMidiOut, DWORD dwVolume) = 0;
static MMRESULT(WINAPI*MMOutGetVolume)(HMIDIOUT hMidiOut, LPDWORD lpdwVolume) = 0;
static MMRESULT(WINAPI*MMOutGetID)(HMIDIOUT hMidiOut, LPUINT puDeviceID) = 0;
#endif

// Callback, used for old apps that require one
static DWORD_PTR WMMCI;
static void(CALLBACK*WMMC)(HMIDIOUT hmo, DWORD wMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2) = 0;

static int ST = 4;
static int SNT[7] =
{
	MOD_FMSYNTH,
	MOD_SYNTH,
	MOD_WAVETABLE,
	MOD_MAPPER,
	MOD_MIDIPORT,
	MOD_SWSYNTH,
	MOD_SQSYNTH
};

static DWORD OMDI = KDMAPI_NOTLOADED;
static DWORD OWINMMI = KDMAPI_NOTLOADED;
static HMODULE OM = NULL;
static HMODULE OWINMM = NULL;

BOOL CheckIfKDMAPIIsUpToDate() {
	DWORD Major, Minor, Build, Revision;
	RKDMAPIV(&Major, &Minor, &Build, &Revision);

	if (Major < REQ_MAJOR) return FALSE;
	else {
		if (Minor < REQ_MINOR) return FALSE;
		else {
			if (Build < REQ_BUILD) return FALSE;
			else {
				if (Revision < REQ_REV) return FALSE;
			}
		}
	}

	return TRUE;
}

void InitializeOMDirectAPI() {
	if (!OMDI) {
		SDD = (MMRESULT*)GetProcAddress(OM, "SendDirectData");			// Send short messages to KDMAPI
		SDLD = (MMRESULT*)GetProcAddress(OM, "SendDirectLongData");		// Send long messages to KDMAPI
		mM = (MMRESULT*)GetProcAddress(OM, "modMessage");				// Other stuff from the driver
		RKDMAPIV = (BOOL*)GetProcAddress(OM, "ReturnKDMAPIVer");		// Used to check KDMAPI
		IOMS = (void*)GetProcAddress(OM, "InitializeKDMAPIStream");		// Initialize the audio output and the threads
		TOMS = (void*)GetProcAddress(OM, "TerminateKDMAPIStream");		// Terminate the audio output and the threads
		ROMS = (void*)GetProcAddress(OM, "ResetKDMAPIStream");			// Reset the audio output and the MIDI channels
		IKDMAPIA = (BOOL*)GetProcAddress(OM, "IsKDMAPIAvailable");		// Dummy, used to enable the KDMAPI flag in the debug window

		if (!SDD || !SDLD || !IOMS ||
			!TOMS || !ROMS || !IKDMAPIA ||
			!mM) {
			// One of the functions failed to load, exit
			MessageBox(
				NULL,
				"Failed to initialize KDMAPI!\n\nThis patch only works with KDMAPI 1.30.0 or newer!\nIt won't work with other MIDI drivers!\n\nPress OK to quit.",
				"Keppy's Direct MIDI API",
				MB_ICONERROR | MB_OK | MB_SYSTEMMODAL
			);
			exit(0x57);
		}

		if (!CheckIfKDMAPIIsUpToDate()) {
			MessageBox(
				NULL,
				"The current version of OmniMIDI is out of date, and won't work with this patch.\n\nPress OK to quit.",
				"Keppy's Direct MIDI API",
				MB_ICONERROR | MB_OK | MB_SYSTEMMODAL
			);
			exit(0x0A);
		}
		
		IKDMAPIA();				// Initialize the audio stream
		OMDI = KDMAPI_LOADED;	// The functions loaded successfully, proceed with the initialization
	}
	
	if (OMDI) IOMS();			// Enable the KDMAPI flag in the debug window
}

#ifdef _DAWRELEASE
void InitializeWinMM() {
	// Load WinMM
	OWINMM = LoadLibrary("C:\\Windows\\system32\\winmm.dll");
	if (!OWINMM) {
		MessageBox(
			NULL,
			"Failed to initialize the Windows Multimedia API!\nAre you trying to use the patch on Windows 98 or something like that?\n\nPress OK to exit.",
			"OMDirect API",
			MB_ICONERROR | MB_OK | MB_SYSTEMMODAL
		);
		exit(0x0A);
	}

	if (!OWINMMI) {
		MMOutGetNumDevs = (UINT*)GetProcAddress(OWINMM, "midiOutGetNumDevs");
		MMOutGetDevCapsW = (MMRESULT*)GetProcAddress(OWINMM, "midiOutGetDevCapsW");
		MMOutOpen = (MMRESULT*)GetProcAddress(OWINMM, "midiOutOpen");
		MMOutClose = (MMRESULT*)GetProcAddress(OWINMM, "midiOutClose");
		MMOutReset = (MMRESULT*)GetProcAddress(OWINMM, "midiOutReset");
		MMOutShortMsg = (MMRESULT*)GetProcAddress(OWINMM, "midiOutShortMsg");
		MMOutPrepareHeader = (MMRESULT*)GetProcAddress(OWINMM, "midiOutPrepareHeader");
		MMOutUnprepareHeader = (MMRESULT*)GetProcAddress(OWINMM, "midiOutUnprepareHeader");
		MMOutLongMsg = (MMRESULT*)GetProcAddress(OWINMM, "midiOutLongMsg");
		MMOutCachePatches = (MMRESULT*)GetProcAddress(OWINMM, "midiOutCachePatches");
		MMOutCacheDrumPatches = (MMRESULT*)GetProcAddress(OWINMM, "midiOutCacheDrumPatches");
		MMOutMessage = (MMRESULT*)GetProcAddress(OWINMM, "midiOutMessage");
		MMOutSetVolume = (MMRESULT*)GetProcAddress(OWINMM, "midiOutSetVolume");
		MMOutGetVolume = (MMRESULT*)GetProcAddress(OWINMM, "midiOutGetVolume");
		MMOutGetID = (MMRESULT*)GetProcAddress(OWINMM, "midiOutGetID");
		OWINMMI = KDMAPI_LOADED;
	}
}
#endif

BOOL WINAPI DllMain(HINSTANCE hInstDLL, DWORD fdwReason, LPVOID fImpLoad) {
	if (fdwReason == DLL_PROCESS_ATTACH) {
#ifdef _DAWRELEASE
		InitializeWinMM();
#endif
		// Load the default DLL from the system directory
		OM = LoadLibrary("C:\\Windows\\system32\\OmniMIDI\\OmniMIDI.dll");
		if (!OM) {
			// Failed, try loading it from the app's directory
			OM = LoadLibrary("OmniMIDI.dll");
			if (!OM) {
				// Failed, OmniMIDI is not available, exit the program
				MessageBox(
					NULL,
					"Failed to initialize OmniMIDI!\n\nThis patch will not work with other MIDI drivers.\n\nPress OK to exit.",
					"Keppy's Direct MIDI API",
					MB_ICONERROR | MB_OK | MB_SYSTEMMODAL
				);
				exit(-1);
			}
		}
	}
	else if (fdwReason == DLL_PROCESS_DETACH)
	{
		TOMS();
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
		wcsncpy(SynthName, L"KDMAPI Output\0", MAXPNAMELEN);
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
	if (uDeviceID > 0) return MMSYSERR_BADDEVICEID;

	LSTATUS RegKey = RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\OmniMIDI\\Configuration", 0, KEY_ALL_ACCESS, &hKey);
	if (RegKey == ERROR_SUCCESS) {
		dwType = REG_DWORD;
		RegQueryValueEx(hKey, "SynthType", NULL, &dwType, (LPBYTE)&ST, &dwSize);
		RegQueryValueEx(hKey, "VID", NULL, &dwType, (LPBYTE)&VID, &dwSize);
		RegQueryValueEx(hKey, "PID", NULL, &dwType, (LPBYTE)&PID, &dwSize);

		dwType = REG_SZ;
		lResult = RegQueryValueExW(hKey, L"SynthName", NULL, &dwType, (LPBYTE)&SynthName, &dwSizeW);
		RegCloseKey(hKey);
		// Done reading, close key
	}

	// Some checks
	if (ST < 0 || ST > 6) ST = 4;
	else ST = MOD_SWSYNTH;

	if (wcslen(SynthName) < 1 || iswspace(SynthName[0]) || lResult != ERROR_SUCCESS) {
		ZeroMemory(SynthName, MAXPNAMELEN);
		wcsncpy(SynthName, L"KDMAPI Output\0", MAXPNAMELEN);
	}

	// Checks done, assign device type
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
	ret = KDMAPI_midiOutGetDevCapsW(0, &myCapsW, sizeof(myCapsW));

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
#ifdef _DAWRELEASE
	// If not OM, forward to WinMM
	if (hMidiOut != OMDummy)
		return MMOutShortMsg(hMidiOut, dwMsg);
#endif
	// Else, forward to KDMAPI
	SDD(dwMsg);
}

MMRESULT WINAPI KDMAPI_midiOutOpen(LPHMIDIOUT lphmo, UINT_PTR uDeviceID, DWORD_PTR dwCallback, DWORD_PTR dwCallbackInstance, DWORD dwFlags) {
#ifdef _DAWRELEASE
	// If above this device ID, return an error
	if (uDeviceID > 0xFFFFFFFE) return MMSYSERR_BADDEVICEID;

	MMRESULT retval = MMSYSERR_INVALPARAM;

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
	// Initialize the API
	InitializeOMDirectAPI();

	// Initialize a dummy out device
	*lphmo = (HMIDI)0x10001;

	// Setup the Callback (If there's one) - NEEDED FOR VANBASCO!
	// If dwflags is CALLBACK_EVENT, then skip, since it's not needed. (Java pls)
	if ((dwFlags != CALLBACK_NULL) && (dwFlags != CALLBACK_EVENT)) {
		WMMC = (void*)dwCallback;
		WMMCI = dwCallbackInstance;

		IsCallbackWindow = (dwFlags == CALLBACK_WINDOW);
		if (WMMC) WMMC((*lphmo), IsCallbackWindow ? MM_MOM_OPEN : MOM_OPEN, WMMCI, 0, 0);
	}

	return MMSYSERR_NOERROR;
#endif
}

MMRESULT WINAPI KDMAPI_midiOutClose(HMIDIOUT hMidiOut) {
#ifdef _DAWRELEASE
	if (hMidiOut == OMDummy) {
#endif
		// Close OM
		TOMS();
		if (WMMC) WMMC(hMidiOut, IsCallbackWindow ? MM_MOM_CLOSE : MOM_CLOSE, WMMCI, 0, 0);
		return MMSYSERR_NOERROR;
#ifdef _DAWRELEASE
	}
	else return MMOutClose(hMidiOut);
#endif
}

MMRESULT WINAPI KDMAPI_midiOutReset(HMIDIOUT hMidiOut) {
#ifdef _DAWRELEASE
	if (hMidiOut == OMDummy) {
#endif
		// Reset the MIDI channels
		ROMS();
		return MMSYSERR_NOERROR;
#ifdef _DAWRELEASE
	}
	else return MMOutReset(hMidiOut);
#endif
}

MMRESULT WINAPI KDMAPI_midiOutPrepareHeader(HMIDIOUT hMidiOut, MIDIHDR* lpMidiOutHdr, UINT uSize) {
	if (!hMidiOut) return MMSYSERR_INVALHANDLE;															// The device doesn't exist, invalid handle
	if (!lpMidiOutHdr || sizeof(lpMidiOutHdr->lpData) > LONGMSG_MAXSIZE) return MMSYSERR_INVALPARAM;	// The buffer doesn't exist or is too big, invalid parameter

	// Lock the MIDIHDR buffer, to prevent the MIDI app from accidentally writing to it
	if (!VirtualLock(lpMidiOutHdr->lpData, sizeof(lpMidiOutHdr->lpData))) 
		return MMSYSERR_NOMEM;																			// Failed to lock the buffer, the working set is not big enough

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

	// Unlock the buffer, and return MMSYSERR_NOERROR
	VirtualUnlock(lpMidiOutHdr->lpData, sizeof(lpMidiOutHdr->lpData));
	return MMSYSERR_NOERROR;
}

MMRESULT WINAPI KDMAPI_midiOutLongMsg(HMIDIOUT hMidiOut, MIDIHDR* lpMidiOutHdr, UINT uSize) {
	// Check if the MIDIHDR buffer is valid	
	if (!hMidiOut) return MMSYSERR_INVALHANDLE;										// The device doesn't exist, invalid handle

#ifdef _DAWRELEASE
	if (hMidiOut == OMDummy) {
#endif

		// This is what you see in both standard and DAW releases ----------------------------------------------------------------
		if (!lpMidiOutHdr) return MMSYSERR_INVALPARAM;								// The buffer doesn't exist, invalid parameter
		if (!(lpMidiOutHdr->dwFlags & MHDR_PREPARED)) return MIDIERR_UNPREPARED;	// The buffer is not prepared
		if (lpMidiOutHdr->dwFlags & MHDR_INQUEUE) return MIDIERR_NOTREADY;			// Another buffer is being played at the moment, the driver is not ready
		
		// Forward the buffer to KDMAPI
		MMRESULT Ret = SDLD(lpMidiOutHdr);

		// Inform the app that the driver successfully received the long message (Required for vanBasco to work), and return the MMRESULT
		if (WMMC) WMMC(hMidiOut, IsCallbackWindow ? MM_MOM_DONE : MOM_DONE, WMMCI, lpMidiOutHdr, lpMidiOutHdr->dwBufferLength);
		return Ret;
		// This is what you see in both standard and DAW releases ----------------------------------------------------------------

#ifdef _DAWRELEASE
	}
	else return MMOutLongMsg(hMidiOut, lpMidiOutHdr, uSize);
#endif
}

MMRESULT WINAPI KDMAPI_midiOutCachePatches(HMIDIOUT hMidiOut, UINT wPatch, WORD *lpPatchArray, UINT wFlags) {
#ifdef _DAWRELEASE
	if (hMidiOut != OMDummy) return MMOutCachePatches(hMidiOut, wPatch, lpPatchArray, wFlags);
#endif
	// Dummy, not needed
	return MMSYSERR_NOTSUPPORTED;
}

MMRESULT WINAPI KDMAPI_midiOutCacheDrumPatches(HMIDIOUT hMidiOut, UINT wPatch, WORD *lpKeyArray, UINT wFlags) {
#ifdef _DAWRELEASE
	if (hMidiOut != OMDummy) return MMOutCacheDrumPatches(hMidiOut, wPatch, lpKeyArray, wFlags);
#endif
	// Dummy, not needed
	return MMSYSERR_NOTSUPPORTED;
}

MMRESULT WINAPI KDMAPI_midiOutMessage(HMIDIOUT hMidiOut, UINT uMsg, DWORD_PTR dw1, DWORD_PTR dw2) {
#ifdef _DAWRELEASE
	if (hMidiOut != OMDummy) return MMOutMessage(hMidiOut, uMsg, dw1, dw2);
#endif
	// Dummy, not needed
	return MMSYSERR_NOTSUPPORTED;
}

MMRESULT WINAPI KDMAPI_midiOutSetVolume(HMIDIOUT hMidiOut, DWORD dwVolume) {
#ifdef _DAWRELEASE
	if (hMidiOut != OMDummy) return MMOutSetVolume(hMidiOut, dwVolume);
#endif
	// Set the volume, even though it won't work lol
	return mM(0, MODM_SETVOLUME, dwVolume, 0, 0);
}

MMRESULT WINAPI KDMAPI_midiOutGetVolume(HMIDIOUT hMidiOut, LPDWORD lpdwVolume) {
#ifdef _DAWRELEASE
	if (hMidiOut != OMDummy) return MMOutGetVolume(hMidiOut, lpdwVolume);
#endif
	// Get the volume
	return mM(0, MODM_GETVOLUME, (DWORD_PTR)lpdwVolume, 0, 0);
}

MMRESULT WINAPI KDMAPI_midiOutGetID(HMIDIOUT hMidiOut, LPUINT puDeviceID) {
#ifdef _DAWRELEASE
	if (hMidiOut != OMDummy) {
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

UINT WINAPI KDMAPI_mmsystemGetVersion() {
	// Dummy, not needed
	return 0x0502U;
}

VOID WINAPI KDMAPI_PoweredByKeppy() {
	MessageBox(NULL, "Haha got u c:", "Windows Multimedia Wrapper", MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL);
}