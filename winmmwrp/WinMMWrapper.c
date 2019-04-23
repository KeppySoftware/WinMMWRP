#include <Windows.h>
#include <stdio.h> 
#include "mmddk.h"
#include <time.h>

#define KDMAPI_UDID			0		// KDMAPI default uDeviceID
#define KDMAPI_NOTLOADED	0		// KDMAPI is not loaded
#define KDMAPI_LOADED		1		// KDMAPI is loaded

#define LONGMSG_MAXSIZE		65535	// Maximum size for a long message/MIDIHDR

#define NT_SUCCESS(StatCode) ((NTSTATUS)(StatCode) == 0)

typedef unsigned __int64 QWORD;
typedef long NTSTATUS;

// Required KDMAPI version
#define REQ_MAJOR	1
#define REQ_MINOR	51
#define REQ_BUILD	0
#define REQ_REV		0


// KDMAPI version from library
static DWORD DrvMajor = 0, DrvMinor = 0, DrvBuild = 0, DrvRevision = 0;

// OM funcs
static HMODULE OM = NULL;																				// OM lib
static DWORD_PTR OMUser;																				// Dummy pointer, used for KDMAPI Output
static HMIDI OMDummy = 0x10001;																			// Dummy pointer, used for KDMAPI Output
static DWORD64(WINAPI*TGT64)() = 0;																		// timeGetTime64
static VOID(WINAPI*SCE)(DWORD, DWORD, DWORD) = 0;														// SendCustomEvent
static MMRESULT(WINAPI*SDD)(DWORD) = 0;																	// SendDirectData
static MMRESULT(WINAPI*SDLD)(LPMIDIHDR) = 0;															// SendDirectLongData
static MMRESULT(WINAPI*PLD)(LPMIDIHDR) = 0;																// PrepareLongData
static MMRESULT(WINAPI*UPLD)(LPMIDIHDR) = 0;															// UnprepareLongData
static MMRESULT(WINAPI*mM)(UINT, UINT, DWORD_PTR, DWORD_PTR, DWORD_PTR) = 0;							// modMessage
static BOOL(WINAPI*RKDMAPIV)(LPDWORD, LPDWORD, LPDWORD, LPDWORD) = 0;									// ReturnKDMAPIVer
static BOOL(WINAPI*IOMS)(VOID) = 0;																		// InitializeKDMAPIStream
static BOOL(WINAPI*TOMS)(VOID) = 0;																		// TerminateKDMAPIStream
static VOID(WINAPI*ROMS)(VOID) = 0;																		// ResetKDMAPIStream
static BOOL(WINAPI*IKDMAPIA)(VOID) = 0;																	// IsKDMAPIAvailable

// WinNT Kernel funcs
static HMODULE NTDLL = NULL;
static ULONGLONG TickStart = 0;																			// For TGT64
static NTSTATUS(NTAPI*NQST)(QWORD*) = 0;															// NtQuerySystemTime

// Callback, used for old apps that require one
static DWORD_PTR WMMCI;
static VOID(CALLBACK*WMMC)(HMIDIOUT, DWORD, DWORD_PTR, DWORD_PTR, DWORD_PTR) = 0;

// Stock WinMM funcs
#include "WinMM.h"

BOOL CheckIfKDMAPIIsUpToDate() {
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
			"KDMAPI ERROR",
			MB_ICONERROR | MB_OK | MB_SYSTEMMODAL
		);

		return FALSE;
	}

	return TRUE;
}

BOOL InitializeOMDirectAPI() {
	wchar_t OMDir[MAX_PATH];

	// Clear memory
	memset(OMDir, 0, sizeof(OMDir));

	// Get system directory
	GetSystemDirectoryW(OMDir, MAX_PATH);

	// Append system directory to OMDir and NTDLLDir, with their respective targets
	wcscat(OMDir, L"\\OmniMIDI\\OmniMIDI.dll");

	// Load the default DLL from the app's directory
	OM = LoadLibraryW(L"OmniMIDI.dll");
	if (!OM) {
		// Failed, try loading it from the system directory
		OM = LoadLibraryW(OMDir);
		if (!OM) {
			// Failed, OmniMIDI is not available, exit the program
			MessageBox(
				NULL,
				"LoadLibraryW for OmniMIDI.dll failed. Can not load OmniMIDI to memory!\nThe wrapper requires OmniMIDI to be installed, or its runtime to be extracted in the same directory as the wrapper itself.\n\nPress OK close this app.",
				"KDMAPI ERROR",
				MB_ICONERROR | MB_OK | MB_SYSTEMMODAL
			);

			return FALSE;
		}
	}

	SCE = (void*)GetProcAddress(OM, "SendCustomEvent");									// Send custom messages to KDMAPI
	TGT64 = (void*)GetProcAddress(OM, "timeGetTime64");									// timeGetTime but 64-bit, from KDMAPI
	SDD = (MMRESULT*)GetProcAddress(OM, "SendDirectData");								// Send short messages to KDMAPI
	SDLD = (MMRESULT*)GetProcAddress(OM, "SendDirectLongData");							// Send long messages to KDMAPI
	PLD = (MMRESULT*)GetProcAddress(OM, "PrepareLongData");								// Prepare long message with KDMAPI
	UPLD = (MMRESULT*)GetProcAddress(OM, "UnprepareLongData");							// Unprepare long message with KDMAPI
	mM = (MMRESULT*)GetProcAddress(OM, "modMessage");									// Other stuff from the driver
	RKDMAPIV = (BOOL*)GetProcAddress(OM, "ReturnKDMAPIVer");							// Used to check KDMAPI
	IOMS = (void*)GetProcAddress(OM, "InitializeKDMAPIStream");							// Initialize the audio output and the threads
	TOMS = (void*)GetProcAddress(OM, "TerminateKDMAPIStream");							// Terminate the audio output and the threads
	ROMS = (void*)GetProcAddress(OM, "ResetKDMAPIStream");								// Reset the audio output and the MIDI channels
	IKDMAPIA = (BOOL*)GetProcAddress(OM, "IsKDMAPIAvailable");							// Dummy, used to enable the KDMAPI flag in the debug window
	NQST = (NTSTATUS*)GetProcAddress(GetModuleHandleA("ntdll"), "NtQuerySystemTime");	// Required for TGT

	if (!SCE || !TGT64 || !SDD || !SDLD || !PLD ||
		!UPLD || !mM || !RKDMAPIV || !IOMS || !TOMS ||
		!ROMS || !IKDMAPIA || !NQST) {
		// One of the functions failed to load, exit
		MessageBox(
			NULL,
			"Failed to initialize KDMAPI!\n\nPress OK to quit.",
			"KDMAPI ERROR",
			MB_ICONERROR | MB_OK | MB_SYSTEMMODAL
		);

		return FALSE;
	}

	if (!NT_SUCCESS(NQST(&TickStart))) {
		MessageBoxA(NULL, "Failed to parse starting tick through NtQuerySystemTime!\nPress OK to stop the loading process of OmniMIDI.", 
			"KDMAPI ERROR", MB_ICONERROR | MB_SYSTEMMODAL);
		return FALSE;
	}

	BOOL Valid = CheckIfKDMAPIIsUpToDate();
	if (Valid) IKDMAPIA();

	return Valid;
}

BOOL DllMain(HINSTANCE hInstDLL, DWORD fdwReason, LPVOID fImpLoad) {
	if (fdwReason == DLL_PROCESS_ATTACH) {
		if (!InitializeWinMM() || !InitializeOMDirectAPI()) return FALSE;
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
	if (lpCaps == NULL) return MMSYSERR_INVALPARAM;

#ifdef _DAWRELEASE
	switch (uDeviceID) {
	case MIDI_MAPPER:
		return MMmidiOutGetDevCapsW(uDeviceID, lpCaps, uSize);

		case KDMAPI_UDID :
			break;

		default:
			return MMmidiOutGetDevCapsW(uDeviceID - 1, lpCaps, uSize);
	}
#else
	if (uDeviceID > 0) return MMSYSERR_BADDEVICEID;
#endif
	// Parse settings from OmniMIDI
	ret = mM(0, MODM_GETDEVCAPS, OMUser, &myCapsWTMP, sizeof(MIDIOUTCAPSW));

	// Assign values
	myCapsW.wMid = myCapsWTMP.wMid;
	myCapsW.wPid = myCapsWTMP.wPid;
#ifdef _DAWRELEASE
	wcsncpy(myCapsW.szPname, L"Keppy's Direct MIDI API", MAXPNAMELEN);
#else
	wcsncpy(myCapsW.szPname, myCapsWTMP.szPname, MAXPNAMELEN);
#endif
	myCapsW.wVoices = myCapsWTMP.wVoices;
	myCapsW.wNotes = myCapsWTMP.wNotes;
	myCapsW.wTechnology = myCapsWTMP.wTechnology;
	myCapsW.wChannelMask = myCapsWTMP.wChannelMask;
	myCapsW.dwSupport = myCapsWTMP.dwSupport;

	// Copy values to pointer, and return 0
	memcpy(lpCaps, &myCapsW, min(uSize, sizeof(myCapsW)));

	return MMSYSERR_NOERROR;
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
	if (hMidiOut != OMDummy) return MMmidiOutShortMsg(hMidiOut, dwMsg);
#endif
	return SDD(dwMsg);
}

MMRESULT WINAPI KDMAPI_midiOutOpen(LPHMIDIOUT lphmo, UINT uDeviceID, DWORD_PTR dwCallback, DWORD_PTR dwCallbackInstance, DWORD dwFlags) {
#ifdef _DAWRELEASE
	// If above this device ID, return an error
	if (uDeviceID > 0xFFFFFFFE) return MMSYSERR_BADDEVICEID;

	MMRESULT retval = MMSYSERR_INVALPARAM;

	switch (uDeviceID) {
		// If it's requesting for the MIDI mapper, forward the call to WinMM/MIDIMAP
	case MIDI_MAPPER:
		retval = MMmidiOutOpen(lphmo, uDeviceID, dwCallback, dwCallbackInstance, dwFlags);
		return retval;

		// If it's asking for OmniMIDI, do everything in-house
		case KDMAPI_UDID :
			// Continue initializing KDMAPI
			break;

			// Else, just forward the call to WinMM
		default:
			retval = MMmidiOutOpen(lphmo, uDeviceID - 1, dwCallback, dwCallbackInstance, dwFlags);
			return retval;
	}
#endif
	// Close any stream, just to be safe
	TOMS();

	// Initialize a dummy out device
	*lphmo = OMDummy;

	// Initialize MIDI out
	if (!IOMS())
		return MMSYSERR_ALLOCATED;

	// Setup the Callback (If there's one) - NEEDED FOR VANBASCO!
	// If dwflags is CALLBACK_EVENT, then skip, since it's not needed. (Java pls)
	if ((dwFlags != CALLBACK_NULL) && (dwFlags != CALLBACK_EVENT)) {
		WMMC = (void*)dwCallback;
		WMMCI = dwCallbackInstance;

		if (WMMC) WMMC((*lphmo), MM_MOM_OPEN, WMMCI, 0, 0);
	}

	return MMSYSERR_NOERROR;
}

MMRESULT WINAPI KDMAPI_midiOutClose(HMIDIOUT hMidiOut) {
#ifdef _DAWRELEASE
	if (hMidiOut != OMDummy) return MMmidiOutClose(hMidiOut);
#endif
	// Close OM
	if (!TOMS()) return MMSYSERR_NOMEM;
	if (WMMC) WMMC(hMidiOut, MM_MOM_OPEN, WMMCI, 0, 0);
	hMidiOut = (HMIDI)0;
	return MMSYSERR_NOERROR;
}

MMRESULT WINAPI KDMAPI_midiOutReset(HMIDIOUT hMidiOut) {
#ifdef _DAWRELEASE
	if (hMidiOut != OMDummy) return MMmidiOutReset(hMidiOut);
#endif
	// Reset the MIDI channels
	ROMS();
	return MMSYSERR_NOERROR;
}

MMRESULT WINAPI KDMAPI_midiOutPrepareHeader(HMIDIOUT hMidiOut, MIDIHDR* lpMidiOutHdr, UINT uSize) {
#ifdef _DAWRELEASE
	if (hMidiOut != OMDummy) return MMmidiOutPrepareHeader(hMidiOut, lpMidiOutHdr, uSize);
#endif
	return PLD(lpMidiOutHdr);
}

MMRESULT WINAPI KDMAPI_midiOutUnprepareHeader(HMIDIOUT hMidiOut, MIDIHDR* lpMidiOutHdr, UINT uSize) {
#ifdef _DAWRELEASE
	if (hMidiOut != OMDummy) return MMmidiOutUnprepareHeader(hMidiOut, lpMidiOutHdr, uSize);
#endif
	return UPLD(lpMidiOutHdr);
}

MMRESULT WINAPI KDMAPI_midiOutLongMsg(HMIDIOUT hMidiOut, MIDIHDR* lpMidiOutHdr, UINT uSize) {
#ifdef _DAWRELEASE
	if (hMidiOut != OMDummy) return MMmidiOutLongMsg(hMidiOut, lpMidiOutHdr, uSize);
#endif
	// Forward the buffer to KDMAPI
	MMRESULT Ret = SDLD(lpMidiOutHdr);

	// Inform the app that the driver successfully received the long message (Required for vanBasco to work), and return the MMRESULT
	if (WMMC) WMMC(hMidiOut, MM_MOM_DONE, WMMCI, lpMidiOutHdr, lpMidiOutHdr->dwBufferLength);

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
	return mM(0, MODM_SETVOLUME, OMUser, dwVolume, 0);
}

MMRESULT WINAPI KDMAPI_midiOutGetVolume(HMIDIOUT hMidiOut, LPDWORD lpdwVolume) {
#ifdef _DAWRELEASE
	if (hMidiOut != OMDummy) return MMmidiOutGetVolume(hMidiOut, lpdwVolume);
#endif
	// Get the volume
	return mM(0, MODM_GETVOLUME, OMUser, lpdwVolume, 0);
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
	return 0x0502U;
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
	*lphStream = OMDummy;

	MIDIOUTCAPSW myCapsW = { 0 };
	UINT myCapsWSize = sizeof(MIDIOUTCAPSW);
	retval = mM(0, MODM_GETDEVCAPS, OMUser, &myCapsW, myCapsWSize);

	MIDIOPENDESC OpenDesc = { 0 };
	OpenDesc.hMidi = lphStream;
	OpenDesc.dwCallback = dwCallback;
	OpenDesc.dwInstance = dwCallbackInstance;

	// Call modMessage
	retval = mM(0, MODM_OPEN, &OMUser, &OpenDesc, fdwOpen | 0x00000002L);
	IKDMAPIA();

	// Everything is oki-doki
	return retval;
}

MMRESULT WINAPI KDMAPI_midiStreamClose(HMIDISTRM hStream) {
#ifdef _DAWRELEASE
	if (hStream != OMDummy) return MMmidiStreamClose(hStream);
#endif
	// Terminate CookedPlayer, free up hStream and return 0
	MMRESULT retval = mM(0, MODM_CLOSE, OMUser, 0, 0);
	if (!retval)
		hStream = (HMIDI)0;
	return retval;
}

MMRESULT WINAPI KDMAPI_midiStreamOut(HMIDISTRM hStream, LPMIDIHDR lpMidiOutHdr, UINT uSize) {
#ifdef _DAWRELEASE
	if (hStream != OMDummy) return MMmidiStreamOut(hStream, lpMidiOutHdr, uSize);
#endif
	// Give stream data to CookedPlayer
	return mM(0, MODM_STRMDATA, OMUser, lpMidiOutHdr, uSize);
}

MMRESULT WINAPI KDMAPI_midiStreamPause(HMIDISTRM hStream) {
#ifdef _DAWRELEASE
	if (hStream != OMDummy) return MMmidiStreamPause(hStream);
#endif
	// Pause CookedPlayer
	return mM(0, MODM_PAUSE, OMUser, 0, 0);
}

MMRESULT WINAPI KDMAPI_midiStreamRestart(HMIDISTRM hStream) {
#ifdef _DAWRELEASE
	if (hStream != OMDummy) return MMmidiStreamRestart(hStream);
#endif
	// Play CookedPlayer
	return mM(0, MODM_RESTART, OMUser, 0, 0);
}

MMRESULT WINAPI KDMAPI_midiStreamStop(HMIDISTRM hStream) {
#ifdef _DAWRELEASE
	if (hStream != OMDummy) return MMmidiStreamStop(hStream);
#endif
	// Stop CookedPlayer
	return mM(0, MODM_STOP, OMUser, 0, 0);
}

MMRESULT WINAPI KDMAPI_midiStreamProperty(HMIDISTRM hStream, LPBYTE lppropdata, DWORD dwProperty) {
#ifdef _DAWRELEASE
	if (hStream != OMDummy) return MMmidiStreamProperty(hStream, lppropdata, dwProperty);
#endif
	// Pass the prop. data to modMessage
	return mM(0, MODM_PROPERTIES, OMUser, lppropdata, dwProperty);
}

MMRESULT WINAPI KDMAPI_midiStreamPosition(HMIDISTRM hStream, LPMMTIME pmmt, UINT cbmmt) {
#ifdef _DAWRELEASE
	if (hStream != OMDummy) return MMmidiStreamPosition(hStream, pmmt, cbmmt);
#endif
	// Give CookedPlayer position to MIDI app
	return mM(0, MODM_GETPOS, OMUser, pmmt, cbmmt);
}

VOID WINAPI KDMAPI_poweredByKeppy() {
	MessageBox(NULL, "With love by Keppy.", "Windows Multimedia Wrapper", MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL);
}

DWORD WINAPI KDMAPI_timeGetTime() {
	ULONGLONG CurrentTime;
	NQST(&CurrentTime);
	return (DWORD)((CurrentTime - TickStart) * (1.0 / 10000.0));
}

DWORD64 WINAPI KDMAPI_timeGetTime64() {
	return TGT64();
}