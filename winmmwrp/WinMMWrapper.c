#include <Windows.h>

#define KDMAPI_UDID			0		// KDMAPI default uDeviceID
#define KDMAPI_NOTLOADED	0		// KDMAPI is not loaded
#define KDMAPI_LOADED		1		// KDMAPI is loaded

#define LONGMSG_MAXSIZE		65535	// Maximum size for a long message/MIDIHDR

#define LOCK_VM_IN_WORKING_SET 1
#define LOCK_VM_IN_RAM 2

// We don't need MMDDK lel
#define MODM_GETVOLUME		10
#define MODM_SETVOLUME		11

#pragma once

// Required KDMAPI version
#define REQ_MAJOR	1
#define REQ_MINOR	47
#define REQ_BUILD	2
#define REQ_REV		21

// KDMAPI version from library
static DWORD DrvMajor = 0, DrvMinor = 0, DrvBuild = 0, DrvRevision = 0;

// OM funcs
static HMODULE OM = NULL;
static BOOL IsCallbackWindow = FALSE;																					// WMMC
static HMIDIOUT OMDummy = 0x10001;																						// Dummy pointer, used for KDMAPI Output
static VOID(WINAPI*SCE)(DWORD, DWORD, DWORD) = 0;																		// SendCustomEvent
static MMRESULT(WINAPI*SDD)(DWORD) = 0;																					// SendDirectData
static MMRESULT(WINAPI*SDLD)(LPMIDIHDR) = 0;																			// SendDirectLongData
static MMRESULT(WINAPI*mM)(UINT, UINT, DWORD_PTR, DWORD_PTR, DWORD_PTR) = 0;											// modMessage
static BOOL(WINAPI*RKDMAPIV)(LPDWORD, LPDWORD, LPDWORD, LPDWORD) = 0;													// ReturnKDMAPIVer
static BOOL(WINAPI*IOMS)(VOID) = 0;																						// InitializeKDMAPIStream
static BOOL(WINAPI*TOMS)(VOID) = 0;																						// TerminateKDMAPIStream
static VOID(WINAPI*ROMS)(VOID) = 0;																						// ResetKDMAPIStream
static BOOL(WINAPI*IKDMAPIA)(VOID) = 0;																					// IsKDMAPIAvailable

// NTDLL funcs
static NTSTATUS(NTAPI*NtQuerySystemTime)(OUT PLARGE_INTEGER);
static NTSTATUS(NTAPI*NtLockVirtualMemory)(IN HANDLE, IN OUT void**, IN OUT ULONG*, IN ULONG);
static NTSTATUS(NTAPI*NtUnlockVirtualMemory)(IN HANDLE, IN OUT void**, IN OUT ULONG*, IN ULONG);

// WinMM funcs, just replace MM with "midiOut" to get the real version
static HMODULE OWINMM = NULL;
#ifdef _DAWRELEASE

static UINT(WINAPI*MMOutGetNumDevs)(VOID) = 0;
static MMRESULT(WINAPI*MMOutGetDevCapsW)(UINT_PTR, LPMIDIOUTCAPSW, UINT) = 0;
static MMRESULT(WINAPI*MMOutOpen)(LPHMIDIOUT, UINT, DWORD_PTR, DWORD_PTR, DWORD) = 0;
static MMRESULT(WINAPI*MMOutClose)(HMIDIOUT) = 0;
static MMRESULT(WINAPI*MMOutReset)(HMIDIOUT) = 0;
static MMRESULT(WINAPI*MMOutShortMsg)(HMIDIOUT, DWORD) = 0;
static MMRESULT(WINAPI*MMOutPrepareHeader)(HMIDIOUT, LPMIDIHDR, UINT) = 0;
static MMRESULT(WINAPI*MMOutUnprepareHeader)(HMIDIOUT, LPMIDIHDR, UINT) = 0;
static MMRESULT(WINAPI*MMOutLongMsg)(HMIDIOUT, LPMIDIHDR, UINT) = 0;
static MMRESULT(WINAPI*MMOutCachePatches)(HMIDIOUT, UINT, LPWORD, UINT) = 0;
static MMRESULT(WINAPI*MMOutCacheDrumPatches)(HMIDIOUT, UINT, LPWORD, UINT) = 0;
static MMRESULT(WINAPI*MMOutMessage)(HMIDIOUT, UINT, DWORD_PTR, DWORD_PTR) = 0;
static MMRESULT(WINAPI*MMOutSetVolume)(HMIDIOUT, DWORD) = 0;
static MMRESULT(WINAPI*MMOutGetVolume)(HMIDIOUT, LPDWORD) = 0;
static MMRESULT(WINAPI*MMOutGetID)(HMIDIOUT, LPUINT) = 0;
#endif
static MMRESULT(WINAPI*MMtimeGetDevCaps)(LPTIMECAPS, UINT) = 0;
static MMRESULT(WINAPI*MMtimeSetEvent)(UINT, UINT, LPTIMECALLBACK, DWORD_PTR, UINT) = 0;
static MMRESULT(WINAPI*MMtimeKillEvent)(UINT) = 0;
static DWORD(WINAPI*MMtimeGetTime)(VOID) = 0;
static MMRESULT(WINAPI*MMtimeGetSystemTime)(LPMMTIME, UINT) = 0;
static MMRESULT(WINAPI*MMtimeBeginPeriod)(UINT) = 0;
static MMRESULT(WINAPI*MMtimeEndPeriod)(UINT) = 0;

// Callback, used for old apps that require one
static DWORD_PTR WMMCI;
static VOID(CALLBACK*WMMC)(HMIDIOUT, DWORD, DWORD_PTR, DWORD_PTR, DWORD_PTR) = 0;

void CheckIfKDMAPIIsUpToDate() {
	BOOL GoAhead = FALSE;
	RKDMAPIV(&DrvMajor, &DrvMinor, &DrvBuild, &DrvRevision);

	if (REQ_MAJOR <= DrvMajor) {
		if (REQ_MAJOR < DrvMajor) GoAhead = TRUE;
		else {
			if (REQ_MINOR <= DrvMinor) {
				if (REQ_MINOR < DrvMinor) GoAhead = TRUE;
				else {
					if (REQ_BUILD <= DrvBuild) {
						if (REQ_BUILD < DrvBuild) GoAhead = TRUE;
						else {
							if (REQ_REV <= DrvRevision) GoAhead = TRUE;
						}			
					}
				}
			}
		}
	}

	if (!GoAhead) {
		TCHAR ErrorBuf[1024];

		sprintf_s(
			ErrorBuf, 
			1024, 
			"This patch requires KDMAPI v%d.%d.%d.%d, but OmniMIDI is reporting KDMAPI v%d.%d.%d.%d.\nThis patch won't work with the current version of OmniMIDI installed, please update it.\n\nPress OK to quit.",
			REQ_MAJOR, REQ_MINOR, REQ_BUILD, REQ_REV, DrvMajor, DrvMinor, DrvBuild, DrvRevision);

		MessageBox(
			NULL,
			ErrorBuf,
			"Windows Multimedia Wrapper",
			MB_ICONERROR | MB_OK | MB_SYSTEMMODAL
		);

		exit(0x0A);
	}
}

void InitializeOMDirectAPI() {
	wchar_t SystemDirectory[MAX_PATH];
	GetSystemDirectoryW(SystemDirectory, MAX_PATH);
	wcscat(SystemDirectory, L"\\OmniMIDI\\OmniMIDI.dll");

	// Load the default DLL from the system directory
	OM = LoadLibraryW(SystemDirectory);
	if (!OM) {
		// Failed, try loading it from the app's directory
		OM = LoadLibraryW("OmniMIDI.dll");
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

	SCE = (void*)GetProcAddress(OM, "SendCustomEvent");				// Send custom messages to KDMAPI
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

	CheckIfKDMAPIIsUpToDate();	// Check if it's up to date before initializing
	IKDMAPIA();					// Initialize the audio stream
}

void InitializeNTDLL() {
	NtQuerySystemTime = (NTSTATUS*)GetProcAddress(GetModuleHandle("ntdll"), "NtQuerySystemTime");
	NtLockVirtualMemory = (NTSTATUS*)GetProcAddress(GetModuleHandle("ntdll"), "NtLockVirtualMemory");
	NtUnlockVirtualMemory = (NTSTATUS*)GetProcAddress(GetModuleHandle("ntdll"), "NtUnlockVirtualMemory");
	if (!NtQuerySystemTime || !NtLockVirtualMemory || !NtUnlockVirtualMemory) {
		MessageBox(NULL, "Failed to parse functions from NTDLL!\nPress OK to quit.", "OmniMIDI - FATAL ERROR", MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);
		exit(0x0A);
	}
}

void InitializeWinMM() {
	// Load WinMM
	wchar_t SystemDirectory[MAX_PATH];
	GetSystemDirectoryW(SystemDirectory, MAX_PATH);
	wcscat(SystemDirectory, L"\\winmm.dll");

	OWINMM = LoadLibraryW(SystemDirectory);
	if (!OWINMM) {
		MessageBox(
			NULL,
			"Failed to initialize the Windows Multimedia API!\nThe interpreter won't work if Windows Multimedia isn't present.\n(P.S. WINE isn't supported.)\n\nPress OK to exit.",
			"OMDirect API",
			MB_ICONERROR | MB_OK | MB_SYSTEMMODAL
		);
		exit(0x0A);
	}

#ifdef _DAWRELEASE
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
#endif
	MMtimeGetDevCaps = (MMRESULT*)GetProcAddress(OWINMM, "timeGetDevCaps");
	MMtimeSetEvent = (MMRESULT*)GetProcAddress(OWINMM, "timeSetEvent");
	MMtimeKillEvent = (MMRESULT*)GetProcAddress(OWINMM, "timeKillEvent");
	MMtimeGetTime = (DWORD*)GetProcAddress(OWINMM, "timeGetTime");
	MMtimeGetSystemTime = (MMRESULT*)GetProcAddress(OWINMM, "timeGetSystemTime");
	MMtimeBeginPeriod = (MMRESULT*)GetProcAddress(OWINMM, "timeBeginPeriod");
	MMtimeEndPeriod = (MMRESULT*)GetProcAddress(OWINMM, "timeEndPeriod");
}

BOOL WINAPI DllMain(HINSTANCE hInstDLL, DWORD fdwReason, LPVOID fImpLoad) {
	if (fdwReason == DLL_PROCESS_ATTACH) {
		InitializeNTDLL();
		InitializeWinMM();
		InitializeOMDirectAPI();
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

	// Return the output device, but Unicode (UTF-8)
	if (lpCaps == NULL) return MMSYSERR_INVALPARAM;

#ifdef _DAWRELEASE
	switch (uDeviceID) {
	case MIDI_MAPPER:
		return MMOutGetDevCapsW(uDeviceID, lpCaps, uSize);

	case KDMAPI_UDID:
		// Assign values
		myCapsW.wMid = 0xFFFF;
		myCapsW.wPid = 0x000A;
		wcsncpy(myCapsW.szPname, L"KDMAPI Output to OmniMIDI", MAXPNAMELEN);
		myCapsW.wVoices = 0xFFFF;
		myCapsW.wNotes = 0x0000;
		myCapsW.wTechnology = MOD_MIDIPORT;
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

	// Assign values
	myCapsW.wMid = 0xFFFF;
	myCapsW.wPid = 0x000A;
	wcsncpy(myCapsW.szPname, L"KDMAPI Output to OmniMIDI", MAXPNAMELEN);
	myCapsW.wVoices = 0xFFFF;
	myCapsW.wNotes = 0x0000;
	myCapsW.wTechnology = MOD_MIDIPORT;
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
	if (hMidiOut != OMDummy)
		return MMOutShortMsg(hMidiOut, dwMsg);
#endif
	return SDD(dwMsg);
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
		// Close any stream, just to be safe
		if (!TOMS()) return MMSYSERR_INVALPARAM;

		// Initialize MIDI out
		if (!IOMS()) return MMSYSERR_ALLOCATED;

		// Initialize a dummy out device
		*lphmo = OMDummy;

		// Setup the Callback (If there's one) - NEEDED FOR VANBASCO!
		// If dwflags is CALLBACK_EVENT, then skip, since it's not needed. (Java pls)
		if ((dwFlags != CALLBACK_NULL) && (dwFlags != CALLBACK_EVENT)) {
			WMMC = (VOID*)dwCallback;
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
	// Close any stream, just to be safe
	if (!TOMS()) return MMSYSERR_INVALPARAM;

	// Initialize MIDI out
	if (!IOMS()) return MMSYSERR_ALLOCATED;

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
		if (!TOMS()) return MMSYSERR_INVALHANDLE;
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

	void* Mem = lpMidiOutHdr->lpData;
	unsigned long Size = sizeof(lpMidiOutHdr->lpData);

	// Lock the MIDIHDR buffer, to prevent the MIDI app from accidentally writing to it
	if (!NtLockVirtualMemory(GetCurrentProcess(), &Mem, &Size, LOCK_VM_IN_WORKING_SET | LOCK_VM_IN_RAM))
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

	void* Mem = lpMidiOutHdr->lpData;
	unsigned long Size = sizeof(lpMidiOutHdr->lpData);

	// Mark the buffer as unprepared
	lpMidiOutHdr->dwFlags &= ~MHDR_PREPARED;

	// Unlock the buffer, and return MMSYSERR_NOERROR
	NtUnlockVirtualMemory(GetCurrentProcess(), &Mem, &Size, LOCK_VM_IN_WORKING_SET | LOCK_VM_IN_RAM);
	RtlSecureZeroMemory(lpMidiOutHdr->lpData, sizeof(lpMidiOutHdr->lpData));

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

MMRESULT WINAPI KDMAPI_timeGetDevCaps(LPTIMECAPS ptc, UINT cbtc) {
	return MMtimeGetDevCaps(ptc, cbtc);
}

MMRESULT WINAPI KDMAPI_timeSetEvent(UINT uDelay, UINT uResolution, LPTIMECALLBACK lpTimeProc, DWORD_PTR dwUser, UINT fuEvent) {
	return MMtimeSetEvent(uDelay, uResolution, lpTimeProc, dwUser, fuEvent);
}

MMRESULT WINAPI KDMAPI_timeKillEvent(UINT uTimerID) {
	return MMtimeKillEvent(uTimerID);
}

DWORD64 WINAPI KDMAPI_timeGetTime64() {
	LARGE_INTEGER Time;
	NtQuerySystemTime(&Time);
	return Time.QuadPart / 10000;
}

DWORD WINAPI KDMAPI_timeGetTime() {
	return (DWORD)KDMAPI_timeGetTime64();
}

MMRESULT WINAPI KDMAPI_timeGetSystemTime(LPMMTIME pmmt, UINT cbmmt) {
	return MMtimeGetSystemTime(pmmt, cbmmt);
}

MMRESULT WINAPI KDMAPI_timeBeginPeriod(UINT uPeriod) {
	return TIMERR_NOERROR;
}

MMRESULT WINAPI KDMAPI_timeEndPeriod(UINT uPeriod) {
	return TIMERR_NOERROR;
}

UINT WINAPI KDMAPI_mmsystemGetVersion(void) {
	// Dummy, not needed
	return 0x0502U;
}

VOID WINAPI KDMAPI_poweredByKeppy() {
	MessageBox(NULL, "Haha got u c:", "Windows Multimedia Wrapper", MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL);
}