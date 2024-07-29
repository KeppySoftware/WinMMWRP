#ifndef _LOADER_HPP
#define _LOADER_HPP

#pragma once

#include "OmniMIDI.hpp"
#include "WinMM.hpp"
#include "Utils.hpp"

#ifndef _M_IX86 
#define WINMMIMPORTS 177
#else
#define WINMMIMPORTS 186
#endif

#ifdef PURE_WRAPPER
typedef struct {
	char gap0[0x50];
	MMRESULT(WINAPI* modMessage)(UINT, UINT, DWORD_PTR, DWORD_PTR, DWORD_PTR);
} midi_device_internal_vtbl_t;

typedef struct {
	midi_device_internal_vtbl_t* vtbl;
	UINT id;
	char gapC[4];
	DWORD_PTR user_ptr;
} midi_device_internal_t;
#endif

namespace OmniMIDI {
	class Loader {
	protected:
#ifndef PURE_WRAPPER
		Lib* KDMAPILib = nullptr;
		LibImport OMLibImports[16] = {
			ImpFunc(modMessage),
			ImpFunc(IsKDMAPIAvailable),
			ImpFunc(InitializeKDMAPIStream),
			ImpFunc(TerminateKDMAPIStream),
			ImpFunc(ResetKDMAPIStream),
			ImpFunc(SendDirectData),
			ImpFunc(SendDirectDataNoBuf),
			ImpFunc(SendDirectLongData),
			ImpFunc(SendDirectLongDataNoBuf),
			ImpFunc(PrepareLongData),
			ImpFunc(UnprepareLongData),
			ImpFunc(InitializeCallbackFeatures),
			ImpFunc(RunCallbackFunction),
			ImpFunc(SendCustomEvent),
			ImpFunc(DriverSettings),
			ImpFunc(timeGetTime64)
		};
		size_t OMLibImportsSize = sizeof(OMLibImports) / sizeof(OMLibImports[0]);
#endif

		Lib* WMMBaseLib = nullptr;
		LibImport WMMBaseLibImps[WINMMIMPORTS] = {
			ImpMMFunc(CloseDriver),
			ImpMMFunc(DefDriverProc),
			ImpMMFunc(DriverCallback),
			ImpMMFunc(DrvGetModuleHandle),
			ImpMMFunc(GetDriverModuleHandle),
			ImpMMFunc(OpenDriver),
			ImpMMFunc(PlaySound),
			ImpMMFunc(PlaySoundA),
			ImpMMFunc(PlaySoundW),
			ImpMMFunc(SendDriverMessage),
			ImpMMFunc(auxGetDevCapsA),
			ImpMMFunc(auxGetDevCapsW),
			ImpMMFunc(auxGetNumDevs),
			ImpMMFunc(auxGetVolume),
			ImpMMFunc(auxOutMessage),
			ImpMMFunc(auxSetVolume),
			ImpMMFunc(joyConfigChanged),
			ImpMMFunc(joyGetDevCapsA),
			ImpMMFunc(joyGetDevCapsW),
			ImpMMFunc(joyGetNumDevs),
			ImpMMFunc(joyGetPos),
			ImpMMFunc(joyGetPosEx),
			ImpMMFunc(joyGetThreshold),
			ImpMMFunc(joyReleaseCapture),
			ImpMMFunc(joySetCapture),
			ImpMMFunc(joySetThreshold),
			ImpMMFunc(mciDriverNotify),
			ImpMMFunc(mciDriverYield),
			ImpMMFunc(mciExecute),
			ImpMMFunc(mciFreeCommandResource),
			ImpMMFunc(mciGetCreatorTask),
			ImpMMFunc(mciGetDeviceIDA),
			ImpMMFunc(mciGetDeviceIDFromElementIDA),
			ImpMMFunc(mciGetDeviceIDFromElementIDW),
			ImpMMFunc(mciGetDeviceIDW),
			ImpMMFunc(mciGetDriverData),
			ImpMMFunc(mciGetErrorStringA),
			ImpMMFunc(mciGetErrorStringW),
			ImpMMFunc(mciGetYieldProc),
			ImpMMFunc(mciLoadCommandResource),
			ImpMMFunc(mciSendCommandA),
			ImpMMFunc(mciSendCommandW),
			ImpMMFunc(mciSendStringA),
			ImpMMFunc(mciSendStringW),
			ImpMMFunc(mciSetDriverData),
			ImpMMFunc(mciSetYieldProc),
			ImpMMFunc(midiConnect),
			ImpMMFunc(midiDisconnect),
			ImpMMFunc(midiInAddBuffer),
			ImpMMFunc(midiInClose),
			ImpMMFunc(midiInGetDevCapsA),
			ImpMMFunc(midiInGetDevCapsW),
			ImpMMFunc(midiInGetErrorTextA),
			ImpMMFunc(midiInGetErrorTextW),
			ImpMMFunc(midiInGetID),
			ImpMMFunc(midiInGetNumDevs),
			ImpMMFunc(midiInMessage),
			ImpMMFunc(midiInOpen),
			ImpMMFunc(midiInPrepareHeader),
			ImpMMFunc(midiInReset),
			ImpMMFunc(midiInStart),
			ImpMMFunc(midiInStop),
			ImpMMFunc(midiInUnprepareHeader),
			ImpMMFunc(midiOutCacheDrumPatches),
			ImpMMFunc(midiOutCachePatches),
			ImpMMFunc(midiOutClose),
			ImpMMFunc(midiOutGetDevCapsA),
			ImpMMFunc(midiOutGetDevCapsW),
			ImpMMFunc(midiOutGetErrorTextA),
			ImpMMFunc(midiOutGetErrorTextW),
			ImpMMFunc(midiOutGetID),
			ImpMMFunc(midiOutGetNumDevs),
			ImpMMFunc(midiOutGetVolume),
			ImpMMFunc(midiOutLongMsg),
			ImpMMFunc(midiOutMessage),
			ImpMMFunc(midiOutOpen),
			ImpMMFunc(midiOutPrepareHeader),
			ImpMMFunc(midiOutReset),
			ImpMMFunc(midiOutSetVolume),
			ImpMMFunc(midiOutShortMsg),
			ImpMMFunc(midiOutUnprepareHeader),
			ImpMMFunc(midiStreamClose),
			ImpMMFunc(midiStreamOpen),
			ImpMMFunc(midiStreamOut),
			ImpMMFunc(midiStreamPause),
			ImpMMFunc(midiStreamPosition),
			ImpMMFunc(midiStreamProperty),
			ImpMMFunc(midiStreamRestart),
			ImpMMFunc(midiStreamStop),
			ImpMMFunc(mixerClose),
			ImpMMFunc(mixerGetControlDetailsA),
			ImpMMFunc(mixerGetControlDetailsW),
			ImpMMFunc(mixerGetDevCapsA),
			ImpMMFunc(mixerGetDevCapsW),
			ImpMMFunc(mixerGetID),
			ImpMMFunc(mixerGetLineControlsA),
			ImpMMFunc(mixerGetLineControlsW),
			ImpMMFunc(mixerGetLineInfoA),
			ImpMMFunc(mixerGetLineInfoW),
			ImpMMFunc(mixerGetNumDevs),
			ImpMMFunc(mixerMessage),
			ImpMMFunc(mixerOpen),
			ImpMMFunc(mixerSetControlDetails),
			ImpMMFunc(mmGetCurrentTask),
			ImpMMFunc(mmTaskBlock),
			ImpMMFunc(mmTaskCreate),
			ImpMMFunc(mmTaskSignal),
			ImpMMFunc(mmTaskYield),
			ImpMMFunc(mmioAdvance),
			ImpMMFunc(mmioAscend),
			ImpMMFunc(mmioClose),
			ImpMMFunc(mmioCreateChunk),
			ImpMMFunc(mmioDescend),
			ImpMMFunc(mmioFlush),
			ImpMMFunc(mmioGetInfo),
			ImpMMFunc(mmioInstallIOProcA),
			ImpMMFunc(mmioInstallIOProcW),
			ImpMMFunc(mmioOpenA),
			ImpMMFunc(mmioOpenW),
			ImpMMFunc(mmioRead),
			ImpMMFunc(mmioRenameA),
			ImpMMFunc(mmioRenameW),
			ImpMMFunc(mmioSeek),
			ImpMMFunc(mmioSendMessage),
			ImpMMFunc(mmioSetBuffer),
			ImpMMFunc(mmioSetInfo),
			ImpMMFunc(mmioStringToFOURCCA),
			ImpMMFunc(mmioStringToFOURCCW),
			ImpMMFunc(mmioWrite),
			ImpMMFunc(sndPlaySoundA),
			ImpMMFunc(sndPlaySoundW),
			ImpMMFunc(waveInAddBuffer),
			ImpMMFunc(waveInClose),
			ImpMMFunc(waveInGetDevCapsA),
			ImpMMFunc(waveInGetDevCapsW),
			ImpMMFunc(waveInGetErrorTextA),
			ImpMMFunc(waveInGetErrorTextW),
			ImpMMFunc(waveInGetID),
			ImpMMFunc(waveInGetNumDevs),
			ImpMMFunc(waveInGetPosition),
			ImpMMFunc(waveInMessage),
			ImpMMFunc(waveInOpen),
			ImpMMFunc(waveInPrepareHeader),
			ImpMMFunc(waveInReset),
			ImpMMFunc(waveInStart),
			ImpMMFunc(waveInStop),
			ImpMMFunc(waveInUnprepareHeader),
			ImpMMFunc(waveOutBreakLoop),
			ImpMMFunc(waveOutClose),
			ImpMMFunc(waveOutGetDevCapsA),
			ImpMMFunc(waveOutGetDevCapsW),
			ImpMMFunc(waveOutGetErrorTextA),
			ImpMMFunc(waveOutGetErrorTextW),
			ImpMMFunc(waveOutGetID),
			ImpMMFunc(waveOutGetNumDevs),
			ImpMMFunc(waveOutGetPitch),
			ImpMMFunc(waveOutGetPlaybackRate),
			ImpMMFunc(waveOutGetPosition),
			ImpMMFunc(waveOutGetVolume),
			ImpMMFunc(waveOutMessage),
			ImpMMFunc(waveOutOpen),
			ImpMMFunc(waveOutPause),
			ImpMMFunc(waveOutPrepareHeader),
			ImpMMFunc(waveOutReset),
			ImpMMFunc(waveOutRestart),
			ImpMMFunc(waveOutSetPitch),
			ImpMMFunc(waveOutSetPlaybackRate),
			ImpMMFunc(waveOutSetVolume),
			ImpMMFunc(waveOutUnprepareHeader),
			ImpMMFunc(waveOutWrite),
			ImpMMFunc(timeBeginPeriod),
			ImpMMFunc(timeEndPeriod),
			ImpMMFunc(timeGetDevCaps),
			ImpMMFunc(timeGetSystemTime),
			ImpMMFunc(timeGetTime),
			ImpMMFunc(timeKillEvent),
			ImpMMFunc(timeSetEvent),

		#ifdef _M_IX86
			ImpMMFunc(aux32Message),
			ImpMMFunc(joy32Message),
			ImpMMFunc(mci32Message),
			ImpMMFunc(mid32Message),
			ImpMMFunc(mod32Message),
			ImpMMFunc(mxd32Message),
			ImpMMFunc(tid32Message),
			ImpMMFunc(wid32Message),
			ImpMMFunc(wod32Message)
		#endif
		};
		size_t WMMBaseLibImpSize = sizeof(WMMBaseLibImps) / sizeof(WMMBaseLibImps[0]);

		Lib* WinMMLib = nullptr;
		LibImport WinMMLibImports[7] = {
			ImpMMFunc(timeBeginPeriod),
			ImpMMFunc(timeEndPeriod),
			ImpMMFunc(timeGetDevCaps),
			ImpMMFunc(timeGetSystemTime),
			ImpMMFunc(timeGetTime),
			ImpMMFunc(timeKillEvent),
			ImpMMFunc(timeSetEvent)
		};
		size_t WinMMLibImportsSize = sizeof(WinMMLibImports) / sizeof(WinMMLibImports[0]);

	public:
#ifndef PURE_WRAPPER
		bool LoadKDMAPIModule();
		bool FreeKDMAPIModule();
		bool IsKDMAPILoaded();
#endif

		bool LoadWinMMModule();
		bool FreeWinMMModule();
	};
}

#endif