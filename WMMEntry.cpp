// WINMMWRP

#include <windows.h>
#include "Loader.hpp"

#define OMID					0
#define DEVICEOVERRIDE			uDeviceID != -1 ? uDeviceID - 1 : uDeviceID
#define OMCHECK					hMidiOut == (HMIDIOUT)OMDummy
#define OMCHECKS				hStream == (HMIDISTRM)OMDummy

typedef unsigned int (*mM)(unsigned int, unsigned int, DWORD_PTR, DWORD_PTR, DWORD_PTR);

static OmniMIDI::Loader* ldr =	nullptr;
static DWORD_PTR OMUser =		0;
static HMIDI OMDummy =			(HMIDI)0x10001;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
        if (ldr == nullptr)
        {
            ldr = new OmniMIDI::Loader;

            if (ldr->LoadWinMMModule()) {
				return TRUE;
            }

        }
        break;

    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }

    return FALSE;
}

LRESULT WINAPI WRP_DefDriverProc(DWORD_PTR dwDId, HDRVR drv, UINT msg, LONG lP1, LONG lP2) {
	return MMDefDriverProc(dwDId, drv, msg, lP1, lP2);
}

UINT WINAPI WRP_midiOutGetNumDevs() {
	auto n = MMmidiOutGetNumDevs() + 1;
	return n;
}

MMRESULT WINAPI WRP_midiOutGetDevCapsW(UINT_PTR uDeviceID, LPMIDIOUTCAPSW lpCaps, UINT uSize) {
	memset(lpCaps, 0, min(uSize, sizeof(MIDIOUTCAPSW)));

	if (uDeviceID == OMID) {
		MIDIOUTCAPSW myCaps;

		if (!lpCaps || uSize != sizeof(MIDIOUTCAPSW))
			return MMSYSERR_INVALPARAM;

		// Assign values
		myCaps.wMid = 0xFFFF;
		myCaps.wPid = 0xFFFF;

		wcsncpy(myCaps.szPname, L"KDMAPI", MAXPNAMELEN);

		myCaps.wVoices = 0xFFFF;
		myCaps.wNotes = 0xFFFF;
		myCaps.wTechnology = MOD_SYNTH;
		myCaps.wChannelMask = 0;

		// Add MIDICAPS_STREAM, since we won't be able to let WinMM handle the stream functions
		myCaps.dwSupport = MIDICAPS_STREAM;

		// Copy values to pointer, and return 0
		memcpy(lpCaps, &myCaps, min(uSize, sizeof(myCaps)));

		return MMSYSERR_NOERROR;
	}

	auto v = DEVICEOVERRIDE;
	auto r = MMmidiOutGetDevCapsW(v, lpCaps, uSize);
	return r;
}

MMRESULT WINAPI WRP_midiOutGetDevCapsA(UINT_PTR uDeviceID, LPMIDIOUTCAPSA lpCaps, UINT uSize) {
	if (uDeviceID == OMID) {
		MIDIOUTCAPSA myCaps;
		MIDIOUTCAPSW myCapsW;
		UINT ret;

		if (!lpCaps || uSize != sizeof(MIDIOUTCAPSA))
			return MMSYSERR_INVALPARAM;

		// Parse settings from OmniMIDI
		ret = WRP_midiOutGetDevCapsW(0, &myCapsW, sizeof(MIDIOUTCAPSW));

		if (ret == MMSYSERR_NOERROR) {
			// Assign values
			myCaps.wMid = myCapsW.wMid;
			myCaps.wPid = myCapsW.wPid;
			myCaps.wVoices = myCapsW.wVoices;
			myCaps.wNotes = myCapsW.wNotes;
			myCaps.wTechnology = myCapsW.wTechnology;
			myCaps.wChannelMask = myCapsW.wChannelMask;
			myCaps.dwSupport = myCapsW.dwSupport;

			// Convert wchar_t* to char*
			wcstombs(myCaps.szPname, myCapsW.szPname, MAXPNAMELEN);

			// Copy values to pointer, and return 0
			memcpy(lpCaps, &myCaps, min(uSize, sizeof(myCaps)));
		}

		return ret;
	}

	return MMmidiOutGetDevCapsA(DEVICEOVERRIDE, lpCaps, uSize);
}

MMRESULT WINAPI WRP_midiOutShortMsg(HMIDIOUT hMidiOut, DWORD dwMsg) {
	if ((HMIDI)hMidiOut != OMDummy)
		return MMmidiOutShortMsg(hMidiOut, dwMsg);

	SendDirectData(dwMsg);
	return MMSYSERR_NOERROR;
}

MMRESULT WINAPI WRP_midiOutOpen(LPHMIDIOUT lphmo, UINT uDeviceID, DWORD_PTR dwCallback, DWORD_PTR dwCallbackInstance, DWORD dwFlags) {
	if (uDeviceID == OMID) {
		if (ldr->LoadKDMAPIModule()) {
			if (*lphmo == (HMIDIOUT)OMDummy)
				return MMSYSERR_ALLOCATED;

			*lphmo = (HMIDIOUT)OMDummy;

			// Setup the Callback (If there's one) - NEEDED FOR VANBASCO!
			if (!InitializeCallbackFeatures((HMIDI*)(*lphmo), dwCallback, dwCallbackInstance, reinterpret_cast<DWORD_PTR>(&OMUser), dwFlags))
			{
				MessageBoxA(NULL, "ICF failed!", "KDMAPI ERROR", MB_SYSTEMMODAL | MB_ICONERROR);
				return MMSYSERR_INVALPARAM;
			}

			// Close any stream, just to be safe
			TerminateKDMAPIStream();

			// Initialize MIDI out
			if (!InitializeKDMAPIStream())
				return MMSYSERR_ERROR;

			DriverSettings(0xFFFFF, NULL, NULL, NULL);

			RunCallbackFunction(MM_MOM_OPEN, 0, 0);

			return MMSYSERR_NOERROR;
		}
		else return MMSYSERR_NOMEM;
	}

	return MMmidiOutOpen(lphmo, DEVICEOVERRIDE, dwCallback, dwCallbackInstance, dwFlags);
}

MMRESULT WINAPI WRP_midiOutClose(HMIDIOUT hMidiOut) {
	if (OMCHECK) {
		if (ldr->IsKDMAPILoaded()) {
			if (!TerminateKDMAPIStream()) return MMSYSERR_NOMEM;

			DriverSettings(0xFFFFE, NULL, NULL, NULL);

			RunCallbackFunction(MM_MOM_CLOSE, 0, 0);

			hMidiOut = (HMIDIOUT)0;

			if (!ldr->FreeKDMAPIModule())
			{
				throw;
				return MMSYSERR_NOMEM;
			}
		}

		return MMSYSERR_NOERROR;
	}

	return MMmidiOutClose(hMidiOut);
}

MMRESULT WINAPI WRP_midiOutReset(HMIDIOUT hMidiOut) {
	if (OMCHECK) {
		ResetKDMAPIStream();
		return MMSYSERR_NOERROR;
	}

	return MMmidiOutReset(hMidiOut);
}

MMRESULT WINAPI WRP_midiOutPrepareHeader(HMIDIOUT hMidiOut, LPMIDIHDR lpMidiOutHdr, UINT uSize) {
	if (OMCHECK)
		PrepareLongData(lpMidiOutHdr, uSize);

	return MMmidiOutPrepareHeader(hMidiOut, lpMidiOutHdr, uSize);
}

MMRESULT WINAPI WRP_midiOutUnprepareHeader(HMIDIOUT hMidiOut, LPMIDIHDR lpMidiOutHdr, UINT uSize) {
	if (OMCHECK)
		UnprepareLongData(lpMidiOutHdr, uSize);

	return MMmidiOutUnprepareHeader(hMidiOut, lpMidiOutHdr, uSize);
}

MMRESULT WINAPI WRP_midiOutLongMsg(HMIDIOUT hMidiOut, LPMIDIHDR lpMidiOutHdr, UINT uSize) {
	if (OMCHECK) {
		// Forward the buffer to KDMAPI
		MMRESULT Ret = SendDirectLongData(lpMidiOutHdr, uSize);

		// Inform the app that the driver successfully received the long message (Required for vanBasco to work), and return the MMRESULT
		RunCallbackFunction(MM_MOM_DONE, (DWORD_PTR)hMidiOut, (DWORD_PTR)lpMidiOutHdr);

		return Ret;
	}

	return MMmidiOutLongMsg(hMidiOut, lpMidiOutHdr, uSize);
}

MMRESULT WINAPI WRP_midiOutMessage(HMIDIOUT hMidiOut, UINT uMsg, DWORD_PTR dw1, DWORD_PTR dw2) {
	if (OMCHECK)
		return MMSYSERR_NOERROR;

	return MMmidiOutMessage(hMidiOut, uMsg, dw1, dw2);
}

MMRESULT WINAPI WRP_midiOutSetVolume(HMIDIOUT hMidiOut, DWORD dwVolume) {
	if (OMCHECK)
		return modMessage(0, MODM_SETVOLUME, OMUser, dwVolume, 0);

	return MMmidiOutSetVolume(hMidiOut, dwVolume);
}

MMRESULT WINAPI WRP_midiOutGetVolume(HMIDIOUT hMidiOut, LPDWORD lpdwVolume) {
	if (OMCHECK)
		return modMessage(0, MODM_GETVOLUME, OMUser, (DWORD_PTR)lpdwVolume, 0);

	return MMmidiOutGetVolume(hMidiOut, lpdwVolume);
}

MMRESULT WINAPI WRP_midiOutGetID(HMIDIOUT hMidiOut, LPUINT puDeviceID) {
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

MMRESULT WINAPI WRP_midiOutCachePatches(HMIDIOUT hMidiOut, UINT wPatch, LPWORD lpPatchArray, UINT wFlags) {
	if (OMCHECK)
		return MMSYSERR_NOERROR;

	return MMmidiOutCachePatches(hMidiOut, wPatch, lpPatchArray, wFlags);
}

MMRESULT WINAPI WRP_midiOutCacheDrumPatches(HMIDIOUT hMidiOut, UINT wPatch, LPWORD lpKeyArray, UINT wFlags) {
	if (OMCHECK)
		return MMSYSERR_NOERROR;

	return MMmidiOutCacheDrumPatches(hMidiOut, wPatch, lpKeyArray, wFlags);
}

MMRESULT WINAPI WRP_midiStreamOpen(LPHMIDISTRM lphStream, LPUINT puDeviceID, DWORD cMidi, DWORD_PTR dwCallback, DWORD_PTR dwCallbackInstance, DWORD fdwOpen) {
	if (puDeviceID == OMID) {
		if (*lphStream == (HMIDISTRM)OMDummy)
			return MMSYSERR_ALLOCATED;

		*lphStream = (HMIDISTRM)OMDummy;

		// Setup the Callback (If there's one) - NEEDED FOR VANBASCO!
		if (!InitializeCallbackFeatures((HMIDI*)(*lphStream), dwCallback, dwCallbackInstance, reinterpret_cast<DWORD_PTR>(&OMUser), fdwOpen | MIDI_IO_COOKED))
		{
			MessageBoxA(NULL, "ICF failed!", "KDMAPI ERROR", MB_SYSTEMMODAL | MB_ICONERROR);
			return MMSYSERR_INVALPARAM;
		}

		// Close any stream, just to be safe
		TerminateKDMAPIStream();

		// Initialize MIDI out
		if (!InitializeKDMAPIStream())
			return MMSYSERR_ERROR;

		RunCallbackFunction(MM_MOM_OPEN, 0, 0);

		return MMSYSERR_NOERROR;
	}

	return MMmidiStreamOpen(lphStream, puDeviceID, cMidi, dwCallback, dwCallbackInstance, fdwOpen);
}

MMRESULT WINAPI WRP_midiStreamClose(HMIDISTRM hStream) {
	if (OMCHECKS) {
		if (!TerminateKDMAPIStream()) 
			return MMSYSERR_NOMEM;

		RunCallbackFunction(MM_MOM_CLOSE, 0, 0);
		hStream = (HMIDISTRM)0;

		return MMSYSERR_NOERROR;
	}

	return MMmidiStreamClose(hStream);
}

MMRESULT WINAPI WRP_midiStreamOut(HMIDISTRM hStream, LPMIDIHDR lpMidiOutHdr, UINT uSize) {
	if (OMCHECKS)
		return modMessage(0, MODM_STRMDATA, OMUser, (DWORD_PTR)lpMidiOutHdr, (DWORD_PTR)uSize);

	return MMmidiStreamOut(hStream, lpMidiOutHdr, uSize);
}

MMRESULT WINAPI WRP_midiStreamPause(HMIDISTRM hStream) {
	if (OMCHECKS)
		return modMessage(0, MODM_PAUSE, OMUser, 0, 0);

	return MMmidiStreamPause(hStream);
}

MMRESULT WINAPI WRP_midiStreamRestart(HMIDISTRM hStream) {
	if (OMCHECKS)
		return modMessage(0, MODM_RESTART, OMUser, 0, 0);

	return MMmidiStreamRestart(hStream);
}

MMRESULT WINAPI WRP_midiStreamStop(HMIDISTRM hStream) {
	if (OMCHECKS)
		return modMessage(0, MODM_STOP, OMUser, 0, 0);

	return MMmidiStreamStop(hStream);
}

MMRESULT WINAPI WRP_midiStreamProperty(HMIDISTRM hStream, LPBYTE lppropdata, DWORD dwProperty) {
	if (OMCHECKS)
		return modMessage(0, MODM_PROPERTIES, OMUser, (DWORD_PTR)lppropdata, (DWORD)dwProperty);

	return MMmidiStreamProperty(hStream, lppropdata, dwProperty);
}

MMRESULT WINAPI WRP_midiStreamPosition(HMIDISTRM hStream, LPMMTIME pmmt, UINT cbmmt) {
	if (OMCHECKS)
		return modMessage(0, MODM_GETPOS, OMUser, (DWORD_PTR)pmmt, (DWORD)cbmmt);

	return MMmidiStreamPosition(hStream, pmmt, cbmmt);
}