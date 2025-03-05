// WINMMWRP

#include "Common.hpp"
#include "Loader.hpp"

#define OMID 0
#define DEVICEOVERRIDE uDeviceID != MIDI_MAPPER &&uDeviceID != 0 ? uDeviceID - 1 : uDeviceID
#define OMCHECK hMidiOut == (HMIDIOUT)OMDummy
#define OMCHECKS hStream == (HMIDISTRM)OMDummy

typedef unsigned int (*mM)(unsigned int, unsigned int, DWORD_PTR, DWORD_PTR, DWORD_PTR);

static OmniMIDI::Loader *ldr = nullptr;
static DWORD_PTR OMUser = 0;
static HMIDI OMDummy = (HMIDI)0x10001;

extern "C" {
	EXPORT BOOL APIENTRY DllMain(HMODULE hModule,
								 DWORD ul_reason_for_call,
								 LPVOID lpReserved)
	{
		switch (ul_reason_for_call)
		{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
			if (ldr == nullptr)
			{
				ldr = new OmniMIDI::Loader;

				if (ldr->LoadWinMMModule())
				{
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

	EXPORT LRESULT WINAPI WRP_DefDriverProc(DWORD_PTR dwDId, HDRVR drv, UINT msg, LONG lP1, LONG lP2)
	{
		return MMDefDriverProc(dwDId, drv, msg, lP1, lP2);
	}

	EXPORT UINT WINAPI WRP_midiOutGetNumDevs()
	{
#ifndef PURE_WRAPPER
		auto n = MMmidiOutGetNumDevs() + 1;
		return n;
#else
		return MMmidiOutGetNumDevs();
#endif
	}

	EXPORT MMRESULT WINAPI WRP_midiOutGetDevCapsW(UINT_PTR uDeviceID, LPMIDIOUTCAPSW lpCaps, UINT uSize)
	{
#ifndef PURE_WRAPPER
		memset(lpCaps, 0, fmin(uSize, sizeof(MIDIOUTCAPSW)));

		if (uDeviceID == OMID)
		{
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
			memcpy(lpCaps, &myCaps, fmin(uSize, sizeof(myCaps)));

			return MMSYSERR_NOERROR;
		}

		auto v = DEVICEOVERRIDE;
#else
		auto v = uDeviceID;
#endif

		auto r = MMmidiOutGetDevCapsW(v, lpCaps, uSize);
		return r;
	}

	EXPORT MMRESULT WINAPI WRP_midiOutGetDevCapsA(UINT_PTR uDeviceID, LPMIDIOUTCAPSA lpCaps, UINT uSize)
	{
		MIDIOUTCAPSA myCaps;
		MIDIOUTCAPSW myCapsW;
		UINT ret;

		if (!lpCaps || uSize != sizeof(MIDIOUTCAPSA))
			return MMSYSERR_INVALPARAM;

		// Parse settings from OmniMIDI
		ret = WRP_midiOutGetDevCapsW(uDeviceID, &myCapsW, sizeof(MIDIOUTCAPSW));

		if (ret == MMSYSERR_NOERROR)
		{
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
			memcpy(lpCaps, &myCaps, fmin(uSize, sizeof(myCaps)));
		}

		return ret;
	}

	EXPORT MMRESULT WINAPI WRP_midiOutOpen(LPHMIDIOUT lphmo, UINT uDeviceID, DWORD_PTR dwCallback, DWORD_PTR dwCallbackInstance, DWORD dwFlags)
	{
#ifndef PURE_WRAPPER
		if (uDeviceID == OMID)
		{
			if (ldr->LoadKDMAPIModule())
			{					
				MessageBoxA(NULL, "Success.", "KDMAPI", MB_SYSTEMMODAL | MB_ICONWARNING);

				if (*lphmo == (HMIDIOUT)OMDummy)
					return MMSYSERR_ALLOCATED;

				*lphmo = (HMIDIOUT)OMDummy;

				// Setup the Callback (If there's one) - NEEDED FOR VANBASCO!
				if (!InitializeCallbackFeatures((HMIDI *)(*lphmo), dwCallback, dwCallbackInstance, reinterpret_cast<DWORD_PTR>(&OMUser), dwFlags))
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
			else
				return MMSYSERR_NOMEM;
		}

		return MMmidiOutOpen(lphmo, DEVICEOVERRIDE, dwCallback, dwCallbackInstance, dwFlags);
#else
		return MMmidiOutOpen(lphmo, uDeviceID, dwCallback, dwCallbackInstance, dwFlags);
#endif
	}

	EXPORT MMRESULT WINAPI WRP_midiOutClose(HMIDIOUT hMidiOut)
	{
#ifndef PURE_WRAPPER
		if (OMCHECK)
		{
			if (ldr->IsKDMAPILoaded())
			{
				if (!TerminateKDMAPIStream())
					return MMSYSERR_NOMEM;

				DriverSettings(0xFFFFE, NULL, NULL, NULL);

				RunCallbackFunction(MM_MOM_CLOSE, 0, 0);

				hMidiOut = (HMIDIOUT)0;

				if (!ldr->FreeKDMAPIModule())
				{
#ifdef _DEBUG
					throw;
#endif

					return MMSYSERR_NOMEM;
				}
			}

			return MMSYSERR_NOERROR;
		}
#endif

		return MMmidiOutClose(hMidiOut);
	}

	EXPORT MMRESULT WINAPI WRP_midiOutReset(HMIDIOUT hMidiOut)
	{
#ifndef PURE_WRAPPER
		if (OMCHECK)
		{
			ResetKDMAPIStream();
			return MMSYSERR_NOERROR;
		}
#endif

		return MMmidiOutReset(hMidiOut);
	}

	EXPORT MMRESULT WINAPI WRP_midiOutPrepareHeader(HMIDIOUT hMidiOut, LPMIDIHDR lpMidiOutHdr, UINT uSize)
	{
#ifndef PURE_WRAPPER
		if (OMCHECK)
			PrepareLongData(lpMidiOutHdr, uSize);
#endif

		return MMmidiOutPrepareHeader(hMidiOut, lpMidiOutHdr, uSize);
	}

	EXPORT MMRESULT WINAPI WRP_midiOutUnprepareHeader(HMIDIOUT hMidiOut, LPMIDIHDR lpMidiOutHdr, UINT uSize)
	{
#ifndef PURE_WRAPPER
		if (OMCHECK)
			UnprepareLongData(lpMidiOutHdr, uSize);
#endif

		return MMmidiOutUnprepareHeader(hMidiOut, lpMidiOutHdr, uSize);
	}

	EXPORT MMRESULT WINAPI WRP_midiOutShortMsg(HMIDIOUT hMidiOut, DWORD dwMsg)
	{
#ifndef PURE_WRAPPER
		if (OMCHECK)
		{
			SendDirectData(dwMsg);
			return MMSYSERR_NOERROR;
		}
#endif

		auto dev = (midi_device_internal_t *)hMidiOut;
		return dev->vtbl->modMessage(dev->id, MODM_DATA, dev->user_ptr, (DWORD_PTR)dwMsg, 0);
	}

	EXPORT MMRESULT WINAPI WRP_midiOutLongMsg(HMIDIOUT hMidiOut, LPMIDIHDR lpMidiOutHdr, UINT uSize)
	{
#ifndef PURE_WRAPPER
		if (OMCHECK)
		{
			// Forward the buffer to KDMAPI
			MMRESULT Ret = SendDirectLongData(lpMidiOutHdr, uSize);

			// Inform the app that the driver successfully received the long message (Required for vanBasco to work), and return the MMRESULT
			RunCallbackFunction(MM_MOM_DONE, (DWORD_PTR)hMidiOut, (DWORD_PTR)lpMidiOutHdr);

			return Ret;
		}
#endif

		return MMmidiOutLongMsg(hMidiOut, lpMidiOutHdr, uSize);
	}

	EXPORT MMRESULT WINAPI WRP_midiOutMessage(HMIDIOUT hMidiOut, UINT uMsg, DWORD_PTR dw1, DWORD_PTR dw2)
	{
#ifndef PURE_WRAPPER
		if (OMCHECK)
			return MMSYSERR_NOERROR;
#endif

		return MMmidiOutMessage(hMidiOut, uMsg, dw1, dw2);
	}

	EXPORT MMRESULT WINAPI WRP_midiOutSetVolume(HMIDIOUT hMidiOut, DWORD dwVolume)
	{
#ifndef PURE_WRAPPER
		if (OMCHECK)
			return modMessage(0, MODM_SETVOLUME, OMUser, dwVolume, 0);
#endif

		return MMmidiOutSetVolume(hMidiOut, dwVolume);
	}

	EXPORT MMRESULT WINAPI WRP_midiOutGetVolume(HMIDIOUT hMidiOut, LPDWORD lpdwVolume)
	{
#ifndef PURE_WRAPPER
		if (OMCHECK)
			return modMessage(0, MODM_GETVOLUME, OMUser, (DWORD_PTR)lpdwVolume, 0);
#endif

		return MMmidiOutGetVolume(hMidiOut, lpdwVolume);
	}

	EXPORT MMRESULT WINAPI WRP_midiOutGetID(HMIDIOUT hMidiOut, LPUINT puDeviceID)
	{
#ifndef PURE_WRAPPER
		UINT Dummy = 0;
		MMRESULT ret = MMmidiOutGetID(hMidiOut, &Dummy);
		if (ret == MMSYSERR_NOERROR)
		{
			switch (Dummy)
			{

			case MIDI_MAPPER:
				*puDeviceID = MIDI_MAPPER;
				break;
			default:
				*puDeviceID = Dummy - 1;
				break;
			}
		}
		return ret;
#else
		return MMmidiOutGetID(hMidiOut, puDeviceID);
#endif
	}

	EXPORT MMRESULT WINAPI WRP_midiOutCachePatches(HMIDIOUT hMidiOut, UINT wPatch, LPWORD lpPatchArray, UINT wFlags)
	{
#ifndef PURE_WRAPPER
		if (OMCHECK)
			return MMSYSERR_NOERROR;
#endif

		return MMmidiOutCachePatches(hMidiOut, wPatch, lpPatchArray, wFlags);
	}

	EXPORT MMRESULT WINAPI WRP_midiOutCacheDrumPatches(HMIDIOUT hMidiOut, UINT wPatch, LPWORD lpKeyArray, UINT wFlags)
	{
#ifndef PURE_WRAPPER
		if (OMCHECK)
			return MMSYSERR_NOERROR;
#endif

		return MMmidiOutCacheDrumPatches(hMidiOut, wPatch, lpKeyArray, wFlags);
	}

	EXPORT MMRESULT WINAPI WRP_midiStreamOpen(LPHMIDISTRM lphStream, LPUINT puDeviceID, DWORD cMidi, DWORD_PTR dwCallback, DWORD_PTR dwCallbackInstance, DWORD fdwOpen)
	{
#ifndef PURE_WRAPPER
		if (puDeviceID == OMID)
		{
			if (*lphStream == (HMIDISTRM)OMDummy)
				return MMSYSERR_ALLOCATED;

			*lphStream = (HMIDISTRM)OMDummy;

			// Setup the Callback (If there's one) - NEEDED FOR VANBASCO!
			if (!InitializeCallbackFeatures((HMIDI *)(*lphStream), dwCallback, dwCallbackInstance, reinterpret_cast<DWORD_PTR>(&OMUser), fdwOpen | MIDI_IO_COOKED))
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
#endif

		return MMmidiStreamOpen(lphStream, puDeviceID, cMidi, dwCallback, dwCallbackInstance, fdwOpen);
	}

	EXPORT MMRESULT WINAPI WRP_midiStreamClose(HMIDISTRM hStream)
	{
#ifndef PURE_WRAPPER
		if (OMCHECKS)
		{
			if (!TerminateKDMAPIStream())
				return MMSYSERR_NOMEM;

			RunCallbackFunction(MM_MOM_CLOSE, 0, 0);
			hStream = (HMIDISTRM)0;

			return MMSYSERR_NOERROR;
		}
#endif

		return MMmidiStreamClose(hStream);
	}

	EXPORT MMRESULT WINAPI WRP_midiStreamOut(HMIDISTRM hStream, LPMIDIHDR lpMidiOutHdr, UINT uSize)
	{
#ifndef PURE_WRAPPER
		if (OMCHECKS)
			return modMessage(0, MODM_STRMDATA, OMUser, (DWORD_PTR)lpMidiOutHdr, (DWORD_PTR)uSize);
#endif

		return MMmidiStreamOut(hStream, lpMidiOutHdr, uSize);
	}

	EXPORT MMRESULT WINAPI WRP_midiStreamPause(HMIDISTRM hStream)
	{
#ifndef PURE_WRAPPER
		if (OMCHECKS)
			return modMessage(0, MODM_PAUSE, OMUser, 0, 0);
#endif

		return MMmidiStreamPause(hStream);
	}

	EXPORT MMRESULT WINAPI WRP_midiStreamRestart(HMIDISTRM hStream)
	{
#ifndef PURE_WRAPPER
		if (OMCHECKS)
			return modMessage(0, MODM_RESTART, OMUser, 0, 0);
#endif

		return MMmidiStreamRestart(hStream);
	}

	EXPORT MMRESULT WINAPI WRP_midiStreamStop(HMIDISTRM hStream)
	{
#ifndef PURE_WRAPPER
		if (OMCHECKS)
			return modMessage(0, MODM_STOP, OMUser, 0, 0);
#endif

		return MMmidiStreamStop(hStream);
	}

	EXPORT MMRESULT WINAPI WRP_midiStreamProperty(HMIDISTRM hStream, LPBYTE lppropdata, DWORD dwProperty)
	{
#ifndef PURE_WRAPPER
		if (OMCHECKS)
			return modMessage(0, MODM_PROPERTIES, OMUser, (DWORD_PTR)lppropdata, (DWORD)dwProperty);
#endif

		return MMmidiStreamProperty(hStream, lppropdata, dwProperty);
	}

	EXPORT MMRESULT WINAPI WRP_midiStreamPosition(HMIDISTRM hStream, LPMMTIME pmmt, UINT cbmmt)
	{
#ifndef PURE_WRAPPER
		if (OMCHECKS)
			return modMessage(0, MODM_GETPOS, OMUser, (DWORD_PTR)pmmt, (DWORD)cbmmt);
#endif

		return MMmidiStreamPosition(hStream, pmmt, cbmmt);
	}

	EXPORT DWORD WINAPI WRP_mixerGetNumDevs()
	{
#ifdef CODPATCH
		DWORD ret = MMmixerGetNumDevs();
		return ret > 15 ? 15 : ret;
#else
		return MMmixerGetNumDevs();
#endif
	}

	EXPORT DWORD WINAPI WRP_waveInGetNumDevs()
	{
#ifdef CODPATCH
		DWORD ret = MMwaveInGetNumDevs();
		return ret > 15 ? 15 : ret;
#else
		return MMwaveInGetNumDevs();
#endif
	}
}