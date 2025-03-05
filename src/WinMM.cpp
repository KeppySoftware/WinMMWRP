/*

	OmniMIDI v15+ (Rewrite) for Windows NT

	This file contains the required code to run the driver under Windows 7 SP1 and later.
	This file is useful only if you want to compile the driver under Windows, it's not needed for Linux/macOS porting.

*/

#include "WinMM.hpp"

// MIDI out stuff
MMRESULT(WINAPI* MMmidiStreamClose)(HMIDISTRM) = 0;
MMRESULT(WINAPI* MMmidiStreamOpen)(LPHMIDISTRM, LPUINT, DWORD, DWORD_PTR, DWORD_PTR, DWORD) = 0;
MMRESULT(WINAPI* MMmidiStreamOut)(HMIDISTRM, LPMIDIHDR, UINT) = 0;
MMRESULT(WINAPI* MMmidiStreamPause)(HMIDISTRM) = 0;
MMRESULT(WINAPI* MMmidiStreamPosition)(HMIDISTRM, LPMMTIME, UINT) = 0;
MMRESULT(WINAPI* MMmidiStreamProperty)(HMIDISTRM, LPBYTE, DWORD) = 0;
MMRESULT(WINAPI* MMmidiStreamRestart)(HMIDISTRM) = 0;
MMRESULT(WINAPI* MMmidiStreamStop)(HMIDISTRM) = 0;
MMRESULT(WINAPI* MMmidiOutCacheDrumPatches)(HMIDIOUT, UINT, LPWORD, UINT) = 0;
MMRESULT(WINAPI* MMmidiOutCachePatches)(HMIDIOUT, UINT, LPWORD, UINT) = 0;
MMRESULT(WINAPI* MMmidiOutClose)(HMIDIOUT) = 0;
MMRESULT(WINAPI* MMmidiOutGetDevCapsA)(UINT_PTR, LPMIDIOUTCAPSA, UINT) = 0;
MMRESULT(WINAPI* MMmidiOutGetDevCapsW)(UINT_PTR, LPMIDIOUTCAPSW, UINT) = 0;
MMRESULT(WINAPI* MMmidiOutGetErrorTextA)(MMRESULT, LPSTR, UINT) = 0;
MMRESULT(WINAPI* MMmidiOutGetErrorTextW)(MMRESULT, LPWSTR, UINT) = 0;
MMRESULT(WINAPI* MMmidiOutGetID)(HMIDIOUT, LPUINT) = 0;
MMRESULT(WINAPI* MMmidiOutGetVolume)(HMIDIOUT, LPDWORD) = 0;
MMRESULT(WINAPI* MMmidiOutLongMsg)(HMIDIOUT, LPMIDIHDR, UINT) = 0;
MMRESULT(WINAPI* MMmidiOutMessage)(HMIDIOUT, UINT, DWORD_PTR, DWORD_PTR) = 0;
MMRESULT(WINAPI* MMmidiOutOpen)(LPHMIDIOUT, UINT, DWORD_PTR, DWORD_PTR, DWORD) = 0;
MMRESULT(WINAPI* MMmidiOutPrepareHeader)(HMIDIOUT, LPMIDIHDR, UINT) = 0;
MMRESULT(WINAPI* MMmidiOutReset)(HMIDIOUT) = 0;
MMRESULT(WINAPI* MMmidiOutSetVolume)(HMIDIOUT, DWORD) = 0;
MMRESULT(WINAPI* MMmidiOutShortMsg)(HMIDIOUT, DWORD) = 0;
MMRESULT(WINAPI* MMmidiOutUnprepareHeader)(HMIDIOUT, LPMIDIHDR, UINT) = 0;
UINT(WINAPI* MMmidiOutGetNumDevs)() = 0;
// MIDI out stuff

// MIDI in stuff
MMRESULT(WINAPI* MMmidiConnect)(HMIDI, HMIDIOUT, LPVOID) = 0;
MMRESULT(WINAPI* MMmidiDisconnect)(HMIDI, HMIDIOUT, LPVOID) = 0;
MMRESULT(WINAPI* MMmidiInAddBuffer)(HMIDIIN, LPMIDIHDR, UINT) = 0;
MMRESULT(WINAPI* MMmidiInClose)(HMIDIIN) = 0;
MMRESULT(WINAPI* MMmidiInGetDevCapsA)(UINT_PTR, LPMIDIINCAPSA, UINT) = 0;
MMRESULT(WINAPI* MMmidiInGetDevCapsW)(UINT_PTR, LPMIDIINCAPSW, UINT) = 0;
MMRESULT(WINAPI* MMmidiInGetErrorTextA)(MMRESULT, LPSTR, UINT) = 0;
MMRESULT(WINAPI* MMmidiInGetErrorTextW)(MMRESULT, LPWSTR, UINT) = 0;
MMRESULT(WINAPI* MMmidiInGetID)(HMIDIIN, LPUINT) = 0;
MMRESULT(WINAPI* MMmidiInMessage)(HMIDIIN, UINT, DWORD_PTR, DWORD_PTR) = 0;
MMRESULT(WINAPI* MMmidiInOpen)(LPHMIDIIN, UINT, DWORD_PTR, DWORD_PTR, DWORD) = 0;
MMRESULT(WINAPI* MMmidiInPrepareHeader)(HMIDIIN, LPMIDIHDR, UINT) = 0;
MMRESULT(WINAPI* MMmidiInReset)(HMIDIIN) = 0;
MMRESULT(WINAPI* MMmidiInStart)(HMIDIIN) = 0;
MMRESULT(WINAPI* MMmidiInStop)(HMIDIIN) = 0;
MMRESULT(WINAPI* MMmidiInUnprepareHeader)(HMIDIIN, LPMIDIHDR, UINT) = 0;
UINT(WINAPI* MMmidiInGetNumDevs)() = 0;
// MIDI in stuff

// Time related stuff
DWORD(WINAPI* MMtimeGetTime)() = 0;
MMRESULT(WINAPI* MMtimeBeginPeriod)(UINT) = 0;
MMRESULT(WINAPI* MMtimeEndPeriod)(UINT) = 0;
MMRESULT(WINAPI* MMtimeGetDevCaps)(LPTIMECAPS, UINT) = 0;
MMRESULT(WINAPI* MMtimeGetSystemTime)(LPMMTIME, UINT) = 0;
MMRESULT(WINAPI* MMtimeKillEvent)(UINT) = 0;
MMRESULT(WINAPI* MMtimeSetEvent)(UINT, UINT, LPTIMECALLBACK, DWORD_PTR, UINT) = 0;
// Time related stuff

// Driver stuff

HDRVR(WINAPI* MMOpenDriver)(_In_ LPCWSTR, _In_ LPCWSTR, _In_ LPARAM) = 0;
LRESULT(WINAPI* MMCloseDriver)(_In_ HDRVR, _In_ LPARAM, _In_ LPARAM) = 0;
LRESULT(WINAPI* MMSendDriverMessage)(_In_ HDRVR, _In_ UINT, _Inout_ LPARAM, _Inout_ LPARAM) = 0;
BOOL(WINAPI* MMmmDrvInstall)(LPCSTR, LPCSTR, BOOL) = 0;
DWORD(WINAPI* MMmmGetCurrentTask)() = 0;
HMODULE(WINAPI* MMDrvGetModuleHandle)(_In_ HDRVR) = 0;
HMODULE(WINAPI* MMGetDriverModuleHandle)(_In_ HDRVR) = 0;
LRESULT(WINAPI* MMDefDriverProc)(DWORD_PTR, HDRVR, UINT, LONG, LONG) = 0;
LRESULT(WINAPI* MMDriverCallback)(DWORD, DWORD, HDRVR, DWORD, DWORD, DWORD, DWORD) = 0;
// Driver stuff

// Game-related stuff
MMRESULT(WINAPI* MMjoyConfigChanged)(DWORD) = 0;
MMRESULT(WINAPI* MMjoyGetDevCapsA)(UINT, LPJOYCAPSA, UINT) = 0;
MMRESULT(WINAPI* MMjoyGetDevCapsW)(UINT, LPJOYCAPSW, UINT) = 0;
MMRESULT(WINAPI* MMjoyGetPos)(UINT, LPJOYINFO) = 0;
MMRESULT(WINAPI* MMjoyGetPosEx)(UINT, LPJOYINFOEX) = 0;
MMRESULT(WINAPI* MMjoyGetThreshold)(UINT, LPUINT) = 0;
MMRESULT(WINAPI* MMjoySetThreshold)(UINT, UINT) = 0;
MMRESULT(WINAPI* MMjoyReleaseCapture)(UINT) = 0;
MMRESULT(WINAPI* MMjoySetCapture)(HWND, UINT, UINT, BOOL) = 0;
UINT(WINAPI* MMjoyGetNumDevs)() = 0;
// Game related stuff

// MCI stuff
BOOL(WINAPI* MMmciDriverNotify)(HWND, UINT, UINT) = 0;
BOOL(WINAPI* MMmciExecute)(LPCSTR) = 0;
BOOL(WINAPI* MMmciFreeCommandResource)(UINT) = 0;
BOOL(WINAPI* MMmciGetErrorStringA)(DWORD, LPTSTR, UINT) = 0;
BOOL(WINAPI* MMmciGetErrorStringW)(DWORD, LPWSTR, UINT) = 0;
BOOL(WINAPI* MMmciSetDriverData)(UINT, DWORD) = 0;
DWORD(WINAPI* MMmciGetDriverData)(UINT) = 0;
HANDLE(WINAPI* MMmciGetCreatorTask)(MCIDEVICEID) = 0;
MCIDEVICEID(WINAPI* MMmciGetDeviceIDA)(LPCTSTR) = 0;
MCIDEVICEID(WINAPI* MMmciGetDeviceIDFromElementIDA)(DWORD, LPCTSTR) = 0;
MCIDEVICEID(WINAPI* MMmciGetDeviceIDFromElementIDW)(DWORD, LPCWSTR) = 0;
MCIDEVICEID(WINAPI* MMmciGetDeviceIDW)(LPCWSTR) = 0;
MCIERROR(WINAPI* MMmciSendCommandA)(MCIDEVICEID, UINT, DWORD_PTR, DWORD_PTR) = 0;
MCIERROR(WINAPI* MMmciSendCommandW)(MCIDEVICEID, UINT, DWORD_PTR, DWORD_PTR) = 0;
MCIERROR(WINAPI* MMmciSendStringA)(LPCTSTR, LPTSTR, UINT, HANDLE) = 0;
MCIERROR(WINAPI* MMmciSendStringW)(LPCWSTR, LPWSTR, UINT, HANDLE) = 0;
UINT(WINAPI* MMmciDriverYield)(UINT) = 0;
UINT(WINAPI* MMmciLoadCommandResource)(HINSTANCE, LPCWSTR, UINT) = 0;
UINT(WINAPI* MMmciSetYieldProc)(MCIDEVICEID, YIELDPROC, DWORD) = 0;
YIELDPROC(WINAPI* MMmciGetYieldProc)(MCIDEVICEID, LPDWORD) = 0;
// MCI stuff

// Mixer stuff
DWORD(WINAPI* MMmixerMessage)(HMIXER, UINT, DWORD_PTR, DWORD_PTR) = 0;
MMRESULT(WINAPI* MMmixerClose)(HMIXER) = 0;
MMRESULT(WINAPI* MMmixerGetControlDetailsA)(HMIXEROBJ, LPMIXERCONTROLDETAILS, DWORD) = 0;
MMRESULT(WINAPI* MMmixerGetControlDetailsW)(HMIXEROBJ, LPMIXERCONTROLDETAILS, DWORD) = 0;
MMRESULT(WINAPI* MMmixerGetDevCapsA)(UINT_PTR, LPMIXERCAPSA, UINT) = 0;
MMRESULT(WINAPI* MMmixerGetDevCapsW)(UINT_PTR, LPMIXERCAPSW, UINT) = 0;
MMRESULT(WINAPI* MMmixerGetID)(HMIXEROBJ, UINT FAR*, DWORD) = 0;
MMRESULT(WINAPI* MMmixerGetLineControlsA)(HMIXEROBJ, LPMIXERLINECONTROLSA, DWORD) = 0;
MMRESULT(WINAPI* MMmixerGetLineControlsW)(HMIXEROBJ, LPMIXERLINECONTROLSW, DWORD) = 0;
MMRESULT(WINAPI* MMmixerGetLineInfoA)(HMIXEROBJ, LPMIXERLINEA, DWORD) = 0;
MMRESULT(WINAPI* MMmixerGetLineInfoW)(HMIXEROBJ, LPMIXERLINEW, DWORD) = 0;
MMRESULT(WINAPI* MMmixerOpen)(LPHMIXER, UINT, DWORD_PTR, DWORD_PTR, DWORD) = 0;
MMRESULT(WINAPI* MMmixerSetControlDetails)(HMIXEROBJ, LPMIXERCONTROLDETAILS, DWORD) = 0;
UINT(WINAPI* MMmixerGetNumDevs)() = 0;
// Mixer stuff

// MMIO stuff
FOURCC(WINAPI* MMmmioStringToFOURCCA)(LPCTSTR, UINT) = 0;
FOURCC(WINAPI* MMmmioStringToFOURCCW)(LPCWSTR, UINT) = 0;
LONG(WINAPI* MMmmioRead)(HMMIO, HPSTR, LONG) = 0;
LONG(WINAPI* MMmmioSeek)(HMMIO, LONG, INT) = 0;
LONG(WINAPI* MMmmioWrite)(HMMIO, char _huge*, LONG) = 0;
LPMMIOPROC(WINAPI* MMmmioInstallIOProcA)(FOURCC, LPMMIOPROC, DWORD) = 0;
LPMMIOPROC(WINAPI* MMmmioInstallIOProcW)(FOURCC, LPMMIOPROC, DWORD) = 0;
LRESULT(WINAPI* MMmmioSendMessage)(HMMIO, UINT, LPARAM, LPARAM) = 0;
MMRESULT(WINAPI* MMmmioAdvance)(HMMIO, LPMMIOINFO, UINT) = 0;
MMRESULT(WINAPI* MMmmioAscend)(HMMIO, LPMMCKINFO, UINT) = 0;
MMRESULT(WINAPI* MMmmioClose)(HMMIO, UINT) = 0;
MMRESULT(WINAPI* MMmmioCreateChunk)(HMMIO, LPMMCKINFO, UINT) = 0;
MMRESULT(WINAPI* MMmmioDescend)(HMMIO, LPMMCKINFO, const MMCKINFO*, UINT) = 0;
MMRESULT(WINAPI* MMmmioFlush)(HMMIO, UINT) = 0;
MMRESULT(WINAPI* MMmmioGetInfo)(HMMIO, LPMMIOINFO, UINT) = 0;
MMRESULT(WINAPI* MMmmioOpenA)(LPTSTR, LPMMIOINFO, DWORD) = 0;
MMRESULT(WINAPI* MMmmioOpenW)(LPWSTR, LPMMIOINFO, DWORD) = 0;
MMRESULT(WINAPI* MMmmioRenameA)(LPCTSTR, LPCTSTR, const LPMMIOINFO, DWORD) = 0;
MMRESULT(WINAPI* MMmmioRenameW)(LPCWSTR, LPCWSTR, const LPMMIOINFO, DWORD) = 0;
MMRESULT(WINAPI* MMmmioSetBuffer)(HMMIO, LPSTR, LONG, UINT) = 0;
MMRESULT(WINAPI* MMmmioSetInfo)(HMMIO, LPMMIOINFO, UINT) = 0;
// MMIO stuff

// MM stuff
BOOL(WINAPI* MMmmTaskSignal)(DWORD) = 0;
UINT(WINAPI* MMmmTaskCreate)(LPTASKCALLBACK, HANDLE, DWORD_PTR) = 0;
VOID(WINAPI* MMmmTaskBlock)(DWORD) = 0;
VOID(WINAPI* MMmmTaskYield)() = 0;
// MM stuff

// PlaySound stuff
BOOL(WINAPI* MMPlaySound)(LPCSTR, HMODULE, DWORD) = 0;
BOOL(WINAPI* MMPlaySoundA)(LPCTSTR, HMODULE, DWORD) = 0;
BOOL(WINAPI* MMPlaySoundW)(LPCWSTR, HMODULE, DWORD) = 0;
BOOL(WINAPI* MMsndPlaySound)(LPCSTR, UINT) = 0;
BOOL(WINAPI* MMsndPlaySoundA)(LPCTSTR, UINT) = 0;
BOOL(WINAPI* MMsndPlaySoundW)(LPCWSTR, UINT) = 0;
// PlaySound stuff

// Aux stuff
DWORD(WINAPI* MMauxOutMessage)(UINT, UINT, DWORD_PTR, DWORD_PTR) = 0;
MMRESULT(WINAPI* MMauxGetDevCapsA)(UINT_PTR, LPAUXCAPSA, UINT) = 0;
MMRESULT(WINAPI* MMauxGetDevCapsW)(UINT_PTR, LPAUXCAPSW, UINT) = 0;
MMRESULT(WINAPI* MMauxGetVolume)(UINT, LPDWORD) = 0;
MMRESULT(WINAPI* MMauxSetVolume)(UINT, DWORD) = 0;
UINT(WINAPI* MMauxGetNumDevs)() = 0;
// Aux stuff

// Wave out stuff
DWORD(WINAPI* MMwaveOutMessage)(HWAVEOUT, UINT, DWORD_PTR, DWORD_PTR) = 0;
MMRESULT(WINAPI* MMwaveOutAddBuffer)(HWAVEOUT, LPWAVEHDR, UINT) = 0;
MMRESULT(WINAPI* MMwaveOutBreakLoop)(HWAVEOUT) = 0;
MMRESULT(WINAPI* MMwaveOutClose)(HWAVEOUT) = 0;
MMRESULT(WINAPI* MMwaveOutGetDevCapsA)(UINT_PTR, LPWAVEOUTCAPSA, UINT) = 0;
MMRESULT(WINAPI* MMwaveOutGetDevCapsW)(UINT_PTR, LPWAVEOUTCAPSW, UINT) = 0;
MMRESULT(WINAPI* MMwaveOutGetErrorTextA)(MMRESULT, LPTSTR, UINT) = 0;
MMRESULT(WINAPI* MMwaveOutGetErrorTextW)(MMRESULT, LPWSTR, UINT) = 0;
MMRESULT(WINAPI* MMwaveOutGetID)(HWAVEOUT, LPUINT) = 0;
MMRESULT(WINAPI* MMwaveOutGetPitch)(HWAVEOUT, LPDWORD) = 0;
MMRESULT(WINAPI* MMwaveOutGetPlaybackRate)(HWAVEOUT, LPDWORD) = 0;
MMRESULT(WINAPI* MMwaveOutGetPosition)(HWAVEOUT, LPMMTIME, UINT) = 0;
MMRESULT(WINAPI* MMwaveOutGetVolume)(HWAVEOUT, LPDWORD) = 0;
MMRESULT(WINAPI* MMwaveOutOpen)(LPHWAVEOUT, UINT, LPCWAVEFORMATEX, DWORD_PTR, DWORD_PTR, DWORD) = 0;
MMRESULT(WINAPI* MMwaveOutPrepareHeader)(HWAVEOUT, LPWAVEHDR, UINT) = 0;
MMRESULT(WINAPI* MMwaveOutReset)(HWAVEOUT) = 0;
MMRESULT(WINAPI* MMwaveOutRestart)(HWAVEOUT) = 0;
MMRESULT(WINAPI* MMwaveOutPause)(HWAVEOUT) = 0;
MMRESULT(WINAPI* MMwaveOutSetPitch)(HWAVEOUT, DWORD) = 0;
MMRESULT(WINAPI* MMwaveOutSetPlaybackRate)(HWAVEOUT, DWORD) = 0;
MMRESULT(WINAPI* MMwaveOutSetVolume)(HWAVEOUT, DWORD) = 0;
MMRESULT(WINAPI* MMwaveOutStart)(HWAVEOUT) = 0;
MMRESULT(WINAPI* MMwaveOutStop)(HWAVEOUT) = 0;
MMRESULT(WINAPI* MMwaveOutUnprepareHeader)(HWAVEOUT, LPWAVEHDR, UINT) = 0;
MMRESULT(WINAPI* MMwaveOutWrite)(HWAVEOUT, LPWAVEHDR, UINT) = 0;
UINT(WINAPI* MMwaveOutGetNumDevs)() = 0;
// Wave out stuff

// Wave in stuff
DWORD(WINAPI* MMwaveInMessage)(HWAVEIN, UINT, DWORD_PTR, DWORD_PTR) = 0;
MMRESULT(WINAPI* MMwaveInAddBuffer)(HWAVEIN, LPWAVEHDR, UINT) = 0;
MMRESULT(WINAPI* MMwaveInClose)(HWAVEIN) = 0;
MMRESULT(WINAPI* MMwaveInGetDevCapsA)(UINT_PTR, LPWAVEINCAPSA, UINT) = 0;
MMRESULT(WINAPI* MMwaveInGetDevCapsW)(UINT_PTR, LPWAVEINCAPSW, UINT) = 0;
MMRESULT(WINAPI* MMwaveInGetErrorTextA)(MMRESULT, LPTSTR, UINT) = 0;
MMRESULT(WINAPI* MMwaveInGetErrorTextW)(MMRESULT, LPWSTR, UINT) = 0;
MMRESULT(WINAPI* MMwaveInGetID)(HWAVEIN, LPUINT) = 0;
MMRESULT(WINAPI* MMwaveInGetPosition)(HWAVEIN, LPMMTIME, UINT) = 0;
MMRESULT(WINAPI* MMwaveInOpen)(LPHWAVEIN, UINT, LPCWAVEFORMATEX, DWORD_PTR, DWORD_PTR, DWORD) = 0;
MMRESULT(WINAPI* MMwaveInPrepareHeader)(HWAVEIN, LPWAVEHDR, UINT) = 0;
MMRESULT(WINAPI* MMwaveInReset)(HWAVEIN) = 0;
MMRESULT(WINAPI* MMwaveInStart)(HWAVEIN) = 0;
MMRESULT(WINAPI* MMwaveInStop)(HWAVEIN) = 0;
MMRESULT(WINAPI* MMwaveInUnprepareHeader)(HWAVEIN, LPWAVEHDR, UINT) = 0;
UINT(WINAPI* MMwaveInGetNumDevs)() = 0;
// Wave in stuff

#ifdef _M_IX86
// Legacy 16-bit functions
MMRESULT(WINAPI* MMaux32Message)(UINT_PTR, UINT, DWORD_PTR, DWORD_PTR, DWORD_PTR) = 0;
MMRESULT(WINAPI* MMjoy32Message)(UINT_PTR, UINT, DWORD_PTR, DWORD_PTR, DWORD_PTR) = 0;
MMRESULT(WINAPI* MMmci32Message)(UINT_PTR, UINT, DWORD_PTR, DWORD_PTR, DWORD_PTR) = 0;
MMRESULT(WINAPI* MMmid32Message)(UINT_PTR, UINT, DWORD_PTR, DWORD_PTR, DWORD_PTR) = 0;
MMRESULT(WINAPI* MMmod32Message)(UINT_PTR, UINT, DWORD_PTR, DWORD_PTR, DWORD_PTR) = 0;
MMRESULT(WINAPI* MMmxd32Message)(UINT_PTR, UINT, DWORD_PTR, DWORD_PTR, DWORD_PTR) = 0;
MMRESULT(WINAPI* MMtid32Message)(UINT_PTR, UINT, DWORD_PTR, DWORD_PTR, DWORD_PTR) = 0;
MMRESULT(WINAPI* MMwid32Message)(UINT_PTR, UINT, DWORD_PTR, DWORD_PTR, DWORD_PTR) = 0;
MMRESULT(WINAPI* MMwod32Message)(UINT_PTR, UINT, DWORD_PTR, DWORD_PTR, DWORD_PTR) = 0;
// Legacy 16-bit functions
#endif

extern "C" {
	EXPORT HDRVR WINAPI WINMM_OpenDriver(LPCWSTR lpDN, LPCWSTR lpSN, LPARAM lp) {
		return MMOpenDriver(lpDN, lpSN, lp);
	}
	
	EXPORT LRESULT WINAPI WINMM_CloseDriver(HDRVR drv, LPARAM lP1, LPARAM lP2) {
		return MMCloseDriver(drv, lP1, lP2);
	}
	
	EXPORT LRESULT WINAPI WINMM_SendDriverMessage(HDRVR drv, UINT msg, LPARAM lP1, LPARAM lP2) {
		return MMSendDriverMessage(drv, msg, lP1, lP2);
	}
	
	EXPORT DWORD WINAPI WINMM_mmGetCurrentTask() {
		return MMmmGetCurrentTask();
	}
	
	EXPORT HMODULE WINAPI WINMM_DrvGetModuleHandle(HDRVR drv) {
		return MMDrvGetModuleHandle(drv);
	}
	
	EXPORT HMODULE WINAPI WINMM_GetDriverModuleHandle(HDRVR drv) {
		return MMGetDriverModuleHandle(drv);
	}
	
	EXPORT LRESULT WINAPI WINMM_DefDriverProc(DWORD_PTR dwDId, HDRVR drv, UINT msg, LONG lP1, LONG lP2) {
		return MMDefDriverProc(dwDId, drv, msg, lP1, lP2);
	}
	
	EXPORT LRESULT WINAPI WINMM_DriverCallback(DWORD dwC, DWORD dwF, HDRVR drv, DWORD msg, DWORD dwU, DWORD dwP1, DWORD dwP2) {
		return MMDriverCallback(dwC, dwF, drv, msg, dwU, dwP1, dwP2);
	}
	
	EXPORT MMRESULT WINAPI WINMM_midiOutGetErrorTextA(MMRESULT err, LPSTR pszT, UINT cchT) {
		return MMmidiOutGetErrorTextA(err, pszT, cchT);
	}
	
	EXPORT MMRESULT WINAPI WINMM_midiOutGetErrorTextW(MMRESULT err, LPWSTR pszT, UINT cchT) {
		return MMmidiOutGetErrorTextW(err, pszT, cchT);
	}
	
	EXPORT MMRESULT WINAPI WINMM_midiConnect(HMIDI hS, HMIDIOUT hM, LPVOID lpV) {
		return MMmidiConnect(hS, hM, lpV);
	}
	
	EXPORT MMRESULT WINAPI WINMM_midiDisconnect(HMIDI hS, HMIDIOUT hM, LPVOID lpV) {
		return MMmidiDisconnect(hS, hM, lpV);
	}
	
	EXPORT MMRESULT WINAPI WINMM_midiInAddBuffer(HMIDIIN hM, LPMIDIHDR buf, UINT bufsize) {
		return MMmidiInAddBuffer(hM, buf, bufsize);
	}
	
	EXPORT MMRESULT WINAPI WINMM_midiInPrepareHeader(HMIDIIN hM, LPMIDIHDR buf, UINT bufsize) {
		return MMmidiInPrepareHeader(hM, buf, bufsize);
	}
	
	EXPORT MMRESULT WINAPI WINMM_midiInUnprepareHeader(HMIDIIN hM, LPMIDIHDR buf, UINT bufsize) {
		return MMmidiInPrepareHeader(hM, buf, bufsize);
	}
	
	EXPORT UINT WINAPI WINMM_midiOutGetNumDevs() {
		int v = MMmidiOutGetNumDevs();
		return v;
	}
	
	EXPORT MMRESULT WINAPI WINMM_midiOutGetDevCapsW(UINT_PTR uDeviceID, LPMIDIOUTCAPSW lpCaps, UINT uSize) {
		return MMmidiOutGetDevCapsW(uDeviceID, lpCaps, uSize);
	}
	
	EXPORT MMRESULT WINAPI WINMM_midiOutGetDevCapsA(UINT_PTR uDeviceID, LPMIDIOUTCAPSA lpCaps, UINT uSize) {
		return MMmidiOutGetDevCapsA(uDeviceID, lpCaps, uSize);
	}
	
	EXPORT MMRESULT WINAPI WINMM_midiOutShortMsg(HMIDIOUT hMidiOut, DWORD dwMsg) {
		return MMmidiOutShortMsg(hMidiOut, dwMsg);
	}
	
	EXPORT MMRESULT WINAPI WINMM_midiOutOpen(LPHMIDIOUT lphmo, UINT uDeviceID, DWORD_PTR dwCallback, DWORD_PTR dwCallbackInstance, DWORD dwFlags) {
		return MMmidiOutOpen(lphmo, uDeviceID, dwCallback, dwCallbackInstance, dwFlags);
	}
	
	EXPORT MMRESULT WINAPI WINMM_midiOutClose(HMIDIOUT hMidiOut) {
		return MMmidiOutClose(hMidiOut);
	}
	
	EXPORT MMRESULT WINAPI WINMM_midiOutReset(HMIDIOUT hMidiOut) {
		return MMmidiOutReset(hMidiOut);
	}
	
	EXPORT MMRESULT WINAPI WINMM_midiOutPrepareHeader(HMIDIOUT hMidiOut, LPMIDIHDR lpMidiOutHdr, UINT uSize) {
		return MMmidiOutPrepareHeader(hMidiOut, lpMidiOutHdr, uSize);
	}
	
	EXPORT MMRESULT WINAPI WINMM_midiOutUnprepareHeader(HMIDIOUT hMidiOut, LPMIDIHDR lpMidiOutHdr, UINT uSize) {
		return MMmidiOutUnprepareHeader(hMidiOut, lpMidiOutHdr, uSize);
	}
	
	EXPORT MMRESULT WINAPI WINMM_midiOutLongMsg(HMIDIOUT hMidiOut, LPMIDIHDR lpMidiOutHdr, UINT uSize) {
		return MMmidiOutLongMsg(hMidiOut, lpMidiOutHdr, uSize);
	}
	
	EXPORT MMRESULT WINAPI WINMM_midiOutMessage(HMIDIOUT hMidiOut, UINT uMsg, DWORD_PTR dw1, DWORD_PTR dw2) {
		return MMmidiOutMessage(hMidiOut, uMsg, dw1, dw2);
	}
	
	EXPORT MMRESULT WINAPI WINMM_midiOutSetVolume(HMIDIOUT hMidiOut, DWORD dwVolume) {
		return MMmidiOutSetVolume(hMidiOut, dwVolume);
	}
	
	EXPORT MMRESULT WINAPI WINMM_midiOutGetVolume(HMIDIOUT hMidiOut, LPDWORD lpdwVolume) {
		return MMmidiOutGetVolume(hMidiOut, lpdwVolume);
	}
	
	EXPORT MMRESULT WINAPI WINMM_midiOutGetID(HMIDIOUT hMidiOut, LPUINT puDeviceID) {
		return MMmidiOutGetID(hMidiOut, puDeviceID);
	}
	
	EXPORT MMRESULT WINAPI WINMM_midiStreamOpen(LPHMIDISTRM lphStream, LPUINT puDeviceID, DWORD cMidi, DWORD_PTR dwCallback, DWORD_PTR dwCallbackInstance, DWORD fdwOpen) {
		return MMmidiStreamOpen(lphStream, puDeviceID, cMidi, dwCallback, dwCallbackInstance, fdwOpen);
	}
	
	EXPORT MMRESULT WINAPI WINMM_midiStreamClose(HMIDISTRM hStream) {
		return MMmidiStreamClose(hStream);
	}
	
	EXPORT MMRESULT WINAPI WINMM_midiStreamOut(HMIDISTRM hStream, LPMIDIHDR lpMidiOutHdr, UINT uSize) {
		return MMmidiStreamOut(hStream, lpMidiOutHdr, uSize);
	}
	
	EXPORT MMRESULT WINAPI WINMM_midiStreamPause(HMIDISTRM hStream) {
		return MMmidiStreamPause(hStream);
	}
	
	EXPORT MMRESULT WINAPI WINMM_midiStreamRestart(HMIDISTRM hStream) {
		return MMmidiStreamRestart(hStream);
	}
	
	EXPORT MMRESULT WINAPI WINMM_midiStreamStop(HMIDISTRM hStream) {
		return MMmidiStreamStop(hStream);
	}
	
	EXPORT MMRESULT WINAPI WINMM_midiStreamProperty(HMIDISTRM hStream, LPBYTE lppropdata, DWORD dwProperty) {
		return MMmidiStreamProperty(hStream, lppropdata, dwProperty);
	}
	
	EXPORT MMRESULT WINAPI WINMM_midiStreamPosition(HMIDISTRM hStream, LPMMTIME pmmt, UINT cbmmt) {
		return MMmidiStreamPosition(hStream, pmmt, cbmmt);
	}
	
	EXPORT MMRESULT WINAPI WINMM_midiOutCachePatches(HMIDIOUT hMidiOut, UINT wPatch, LPWORD lpPatchArray, UINT wFlags) {
		return MMmidiOutCachePatches(hMidiOut, wPatch, lpPatchArray, wFlags);
	}
	
	EXPORT MMRESULT WINAPI WINMM_midiOutCacheDrumPatches(HMIDIOUT hMidiOut, UINT wPatch, LPWORD lpKeyArray, UINT wFlags) {
		return MMmidiOutCacheDrumPatches(hMidiOut, wPatch, lpKeyArray, wFlags);
	}
	
	EXPORT UINT WINAPI WINMM_mmsystemGetVersion(void) {
		// Dummy, not needed
		return 0x0600U;
	}
	
	EXPORT MMRESULT WINAPI WINMM_midiInOpen(LPHMIDIIN lphM, UINT uDID, DWORD_PTR dwC, DWORD_PTR dwCI, DWORD dwF) {
		return MMmidiInOpen(lphM, uDID, dwC, dwCI, dwF);
	}
	
	EXPORT MMRESULT WINAPI WINMM_midiInStart(HMIDIIN hM) {
		return MMmidiInStart(hM);
	}
	
	EXPORT MMRESULT WINAPI WINMM_midiInReset(HMIDIIN hM) {
		return MMmidiInReset(hM);
	}
	
	EXPORT MMRESULT WINAPI WINMM_midiInStop(HMIDIIN hM) {
		return MMmidiInStop(hM);
	}
	
	EXPORT MMRESULT WINAPI WINMM_midiInClose(HMIDIIN hM) {
		return MMmidiInClose(hM);
	}
	
	EXPORT MMRESULT WINAPI WINMM_midiInGetID(HMIDIIN hM, LPUINT lpU) {
		return MMmidiInGetID(hM, lpU);
	}
	
	EXPORT MMRESULT WINAPI WINMM_midiInGetDevCapsA(UINT_PTR uP, LPMIDIINCAPSA LPMIC, UINT u) {
		return MMmidiInGetDevCapsA(uP, LPMIC, u);
	}
	
	EXPORT MMRESULT WINAPI WINMM_midiInGetDevCapsW(UINT_PTR uP, LPMIDIINCAPSW LPMIC, UINT u) {
		return MMmidiInGetDevCapsW(uP, LPMIC, u);
	}
	
	EXPORT MMRESULT WINAPI WINMM_midiInGetErrorTextA(MMRESULT mmr, LPSTR str, UINT u) {
		return MMmidiInGetErrorTextA(mmr, str, u);
	}
	
	EXPORT MMRESULT WINAPI WINMM_midiInGetErrorTextW(MMRESULT mmr, LPWSTR str, UINT u) {
		return MMmidiInGetErrorTextW(mmr, str, u);
	}
	
	EXPORT MMRESULT WINAPI WINMM_midiInMessage(HMIDIIN hM, UINT u, DWORD_PTR dwP1, DWORD_PTR dwP2) {
		return MMmidiInMessage(hM, u, dwP1, dwP2);
	}
	
	EXPORT UINT WINAPI WINMM_midiInGetNumDevs() {
		return MMmidiInGetNumDevs();
	}
	
	EXPORT MMRESULT WINAPI WINMM_joyConfigChanged(DWORD dwF) {
		return MMjoyConfigChanged(dwF);
	}
	
	EXPORT MMRESULT WINAPI WINMM_joyGetDevCapsA(UINT uDID, LPJOYCAPSA LPJC, UINT size) {
		return MMjoyGetDevCapsA(uDID, LPJC, size);
	}
	
	EXPORT MMRESULT WINAPI WINMM_joyGetDevCapsW(UINT uDID, LPJOYCAPSW LPJC, UINT size) {
		return MMjoyGetDevCapsW(uDID, LPJC, size);
	}
	
	EXPORT MMRESULT WINAPI WINMM_joyGetPos(UINT uDID, LPJOYINFO LPJI) {
		return MMjoyGetPos(uDID, LPJI);
	}
	
	EXPORT MMRESULT WINAPI WINMM_joyGetPosEx(UINT uDID, LPJOYINFOEX LPJI) {
		return MMjoyGetPosEx(uDID, LPJI);
	}
	
	EXPORT MMRESULT WINAPI WINMM_joyGetThreshold(UINT uDID, LPUINT val) {
		return MMjoyGetThreshold(uDID, val);
	}
	
	EXPORT MMRESULT WINAPI WINMM_joySetThreshold(UINT uDID, UINT val) {
		return MMjoySetThreshold(uDID, val);
	}
	
	EXPORT MMRESULT WINAPI WINMM_joyReleaseCapture(UINT uDID) {
		return MMjoyReleaseCapture(uDID);
	}
	
	EXPORT MMRESULT WINAPI WINMM_joySetCapture(HWND h, UINT uDID, UINT uP, BOOL fC) {
		return MMjoySetCapture(h, uDID, uP, fC);
	}
	
	EXPORT UINT WINAPI WINMM_joyGetNumDevs() {
		return MMjoyGetNumDevs();
	}
	
	EXPORT BOOL WINAPI WINMM_mciDriverNotify(HWND hC, UINT wDID, UINT uS) {
		return MMmciDriverNotify(hC, wDID, uS);
	}
	
	EXPORT BOOL WINAPI WINMM_mciExecute(LPCSTR pC) {
		return MMmciExecute(pC);
	}
	
	EXPORT UINT WINAPI WINMM_mciLoadCommandResource(HINSTANCE hI, LPCWSTR lpRN, UINT wT) {
		return MMmciLoadCommandResource(hI, lpRN, wT);
	}
	
	EXPORT BOOL WINAPI WINMM_mciFreeCommandResource(UINT wT) {
		return MMmciFreeCommandResource(wT);
	}
	
	EXPORT BOOL WINAPI WINMM_mciGetErrorStringA(DWORD MCIE, LPTSTR pT, UINT cT) {
		return MMmciGetErrorStringA(MCIE, pT, cT);
	}
	
	EXPORT BOOL WINAPI WINMM_mciGetErrorStringW(DWORD MCIE, LPWSTR pT, UINT cT) {
		return MMmciGetErrorStringW(MCIE, pT, cT);
	}
	
	EXPORT BOOL WINAPI WINMM_mciSetDriverData(UINT wDID, DWORD dwD) {
		return MMmciSetDriverData(wDID, dwD);
	}
	
	EXPORT BOOL WINAPI WINMM_mciGetDriverData(UINT wDID) {
		return MMmciGetDriverData(wDID);
	}
	
	EXPORT HANDLE WINAPI WINMM_mciGetCreatorTask(MCIDEVICEID IDD) {
		return MMmciGetCreatorTask(IDD);
	}
	
	EXPORT MCIDEVICEID WINAPI WINMM_mciGetDeviceIDA(LPCTSTR pszD) {
		return MMmciGetDeviceIDA(pszD);
	}
	
	EXPORT MCIDEVICEID WINAPI WINMM_mciGetDeviceIDW(LPCWSTR pszD) {
		return MMmciGetDeviceIDW(pszD);
	}
	
	EXPORT MCIDEVICEID WINAPI WINMM_mciGetDeviceIDFromElementIDA(DWORD dwEID, LPCTSTR pszD) {
		return MMmciGetDeviceIDFromElementIDA(dwEID, pszD);
	}
	
	EXPORT MCIDEVICEID WINAPI WINMM_mciGetDeviceIDFromElementIDW(DWORD dwEID, LPCWSTR pszD) {
		return MMmciGetDeviceIDFromElementIDW(dwEID, pszD);
	}
	
	EXPORT MCIERROR WINAPI WINMM_mciSendCommandA(MCIDEVICEID uDID, UINT msg, DWORD_PTR dwP1, DWORD_PTR dwP2) {
		return MMmciSendCommandA(uDID, msg, dwP1, dwP2);
	}
	
	EXPORT MCIERROR WINAPI WINMM_mciSendCommandW(MCIDEVICEID uDID, UINT msg, DWORD_PTR dwP1, DWORD_PTR dwP2) {
		return MMmciSendCommandW(uDID, msg, dwP1, dwP2);
	}
	
	EXPORT MCIERROR WINAPI WINMM_mciSendStringA(LPCTSTR lpszC, LPTSTR lpszR, UINT cchR, HANDLE hC) {
		return MMmciSendStringA(lpszC, lpszR, cchR, hC);
	}
	
	EXPORT MCIERROR WINAPI WINMM_mciSendStringW(LPCWSTR lpszC, LPWSTR lpszR, UINT cchR, HANDLE hC) {
		return MMmciSendStringW(lpszC, lpszR, cchR, hC);
	}
	
	EXPORT UINT WINAPI WINMM_mciDriverYield(MCIDEVICEID wDID) {
		return MMmciDriverYield(wDID);
	}
	
	EXPORT UINT WINAPI WINMM_mciSetYieldProc(MCIDEVICEID wDID, YIELDPROC fpYP, DWORD dwYD) {
		return MMmciSetYieldProc(wDID, fpYP, dwYD);
	}
	
	EXPORT YIELDPROC WINAPI WINMM_mciGetYieldProc(MCIDEVICEID wDID, LPDWORD lpdwYD) {
		return MMmciGetYieldProc(wDID, lpdwYD);
	}
	
	EXPORT DWORD WINAPI WINMM_mixerMessage(HMIXER dID, UINT msg, DWORD_PTR dwP1, DWORD_PTR dwP2) {
		return MMmixerMessage(dID, msg, dwP1, dwP2);
	}
	
	EXPORT MMRESULT WINAPI WINMM_mixerClose(HMIXER dID) {
		return MMmixerClose(dID);
	}
	
	EXPORT MMRESULT WINAPI WINMM_mixerGetLineInfoA(HMIXEROBJ dIDO, LPMIXERLINEA LPML, DWORD size) {
		return MMmixerGetLineInfoA(dIDO, LPML, size);
	}
	
	EXPORT MMRESULT WINAPI WINMM_mixerGetLineInfoW(HMIXEROBJ dIDO, LPMIXERLINEW LPML, DWORD size) {
		return MMmixerGetLineInfoW(dIDO, LPML, size);
	}
	
	EXPORT MMRESULT WINAPI WINMM_mixerGetLineControlsA(HMIXEROBJ dIDO, LPMIXERLINECONTROLSA LPMLC, DWORD size) {
		return MMmixerGetLineControlsA(dIDO, LPMLC, size);
	}
	
	EXPORT MMRESULT WINAPI WINMM_mixerGetLineControlsW(HMIXEROBJ dIDO, LPMIXERLINECONTROLSW LPMLC, DWORD size) {
		return MMmixerGetLineControlsW(dIDO, LPMLC, size);
	}
	
	EXPORT MMRESULT WINAPI WINMM_mixerSetControlDetails(HMIXEROBJ dIDO, LPMIXERCONTROLDETAILS LPMCD, DWORD size) {
		return MMmixerSetControlDetails(dIDO, LPMCD, size);
	}
	
	EXPORT MMRESULT WINAPI WINMM_mixerGetControlDetailsA(HMIXEROBJ dIDO, LPMIXERCONTROLDETAILS LPMCD, DWORD size) {
		return MMmixerGetControlDetailsA(dIDO, LPMCD, size);
	}
	
	EXPORT MMRESULT WINAPI WINMM_mixerGetControlDetailsW(HMIXEROBJ dIDO, LPMIXERCONTROLDETAILS LPMCD, DWORD size) {
		return MMmixerGetControlDetailsW(dIDO, LPMCD, size);
	}
	
	EXPORT MMRESULT WINAPI WINMM_mixerGetDevCapsA(UINT_PTR uDID, LPMIXERCAPSA LPMC, UINT size) {
		return MMmixerGetDevCapsA(uDID, LPMC, size);
	}
	
	EXPORT MMRESULT WINAPI WINMM_mixerGetDevCapsW(UINT_PTR uDID, LPMIXERCAPSW LPMC, UINT size) {
		return MMmixerGetDevCapsW(uDID, LPMC, size);
	}
	
	EXPORT MMRESULT WINAPI WINMM_mixerGetID(HMIXEROBJ dIDO, UINT* puMxId, DWORD fdwld) {
		return MMmixerGetID(dIDO, puMxId, fdwld);
	}
	
	EXPORT MMRESULT WINAPI WINMM_mixerOpen(LPHMIXER lpdID, UINT uMxId, DWORD_PTR dwC, DWORD_PTR dwI, DWORD fdwO) {
		return MMmixerOpen(lpdID, uMxId, dwC, dwI, fdwO);
	}
	
	EXPORT UINT WINAPI WINMM_mixerGetNumDevs() {
		return MMmixerGetNumDevs();
	}
	
	EXPORT FOURCC WINAPI WINMM_mmioStringToFOURCCA(LPCTSTR sz, UINT f) {
		return MMmmioStringToFOURCCA(sz, f);
	}
	
	EXPORT FOURCC WINAPI WINMM_mmioStringToFOURCCW(LPCWSTR sz, UINT f) {
		return MMmmioStringToFOURCCW(sz, f);
	}
	
	EXPORT LONG WINAPI WINMM_mmioRead(HMMIO hm, HPSTR pch, LONG cch) {
		return MMmmioRead(hm, pch, cch);
	}
	
	EXPORT LONG WINAPI WINMM_mmioSeek(HMMIO hm, LONG lO, INT iO) {
		return MMmmioSeek(hm, lO, iO);
	}
	
	EXPORT LONG WINAPI WINMM_mmioWrite(HMMIO hm, char* pch, LONG cch) {
		return MMmmioWrite(hm, pch, cch);
	}
	
	EXPORT LPMMIOPROC WINAPI WINMM_mmioInstallIOProcA(FOURCC fccIOP, LPMMIOPROC pIOP, DWORD dwF) {
		return MMmmioInstallIOProcA(fccIOP, pIOP, dwF);
	}
	
	EXPORT LPMMIOPROC WINAPI WINMM_mmioInstallIOProcW(FOURCC fccIOP, LPMMIOPROC pIOP, DWORD dwF) {
		return MMmmioInstallIOProcW(fccIOP, pIOP, dwF);
	}
	
	EXPORT LRESULT WINAPI WINMM_mmioSendMessage(HMMIO hm, UINT msg, LPARAM lP1, LPARAM lP2) {
		return MMmmioSendMessage(hm, msg, lP1, lP2);
	}
	
	EXPORT MMRESULT WINAPI WINMM_mmioAdvance(HMMIO hm, LPMMIOINFO pmmioi, UINT fuA) {
		return MMmmioAdvance(hm, pmmioi, fuA);
	}
	
	EXPORT MMRESULT WINAPI WINMM_mmioAscend(HMMIO hm, LPMMCKINFO pmmcki, UINT fuA) {
		return MMmmioAscend(hm, pmmcki, fuA);
	}
	
	EXPORT MMRESULT WINAPI WINMM_mmioClose(HMMIO hm, UINT fuC) {
		return MMmmioClose(hm, fuC);
	}
	
	EXPORT MMRESULT WINAPI WINMM_mmioCreateChunk(HMMIO hm, LPMMCKINFO pmmcki, UINT fuC) {
		return MMmmioCreateChunk(hm, pmmcki, fuC);
	}
	
	EXPORT MMRESULT WINAPI WINMM_mmioDescend(HMMIO hm, LPMMCKINFO pmmcki, const MMCKINFO* pmmckiP, UINT fuD) {
		return MMmmioDescend(hm, pmmcki, pmmckiP, fuD);
	}
	
	EXPORT MMRESULT WINAPI WINMM_mmioFlush(HMMIO hm, UINT fuF) {
		return MMmmioFlush(hm, fuF);
	}
	
	EXPORT MMRESULT WINAPI WINMM_mmioGetInfo(HMMIO hm, LPMMIOINFO pmmioi, UINT fuI) {
		return MMmmioGetInfo(hm, pmmioi, fuI);
	}
	
	EXPORT MMRESULT WINAPI WINMM_mmioSetInfo(HMMIO hm, const LPMMIOINFO pmmioi, UINT fuI) {
		return MMmmioSetInfo(hm, pmmioi, fuI);
	}
	
	EXPORT MMRESULT WINAPI WINMM_mmioOpenA(LPTSTR pszFN, LPMMIOINFO pmmioi, DWORD fdwO) {
		return MMmmioOpenA(pszFN, pmmioi, fdwO);
	}
	
	EXPORT MMRESULT WINAPI WINMM_mmioOpenW(LPWSTR pszFN, LPMMIOINFO pmmioi, DWORD fdwO) {
		return MMmmioOpenW(pszFN, pmmioi, fdwO);
	}
	
	EXPORT MMRESULT WINAPI WINMM_mmioRenameA(LPCTSTR pszFN, LPCTSTR pszNFN, const LPMMIOINFO pmmioi, DWORD fdwR) {
		return MMmmioRenameA(pszFN, pszNFN, pmmioi, fdwR);
	}
	
	EXPORT MMRESULT WINAPI WINMM_mmioRenameW(LPCWSTR pszFN, LPCWSTR pszNFN, const LPMMIOINFO pmmioi, DWORD fdwR) {
		return MMmmioRenameW(pszFN, pszNFN, pmmioi, fdwR);
	}
	
	EXPORT MMRESULT WINAPI WINMM_mmioSetBuffer(HMMIO hm, LPSTR pchB, LONG cchB, UINT fuB) {
		return MMmmioSetBuffer(hm, pchB, cchB, fuB);
	}
	
	EXPORT BOOL WINAPI WINMM_mmTaskSignal(DWORD undef) {
		return MMmmTaskSignal(undef);
	}
	
	EXPORT UINT WINAPI WINMM_mmTaskCreate(LPTASKCALLBACK undef1, HANDLE undef2, DWORD_PTR undef3) {
		return MMmmTaskCreate(undef1, undef2, undef3);
	}
	
	EXPORT VOID WINAPI WINMM_mmTaskBlock(DWORD undef) {
		return MMmmTaskBlock(undef);
	}
	
	EXPORT VOID WINAPI WINMM_mmTaskYield() {
		MMmmTaskYield();
	}
	
	EXPORT BOOL WINAPI WINMM_PlaySound(LPCSTR pszS, HMODULE hmod, DWORD fdwS) {
		return MMPlaySound(pszS, hmod, fdwS);
	}
	
	EXPORT BOOL WINAPI WINMM_PlaySoundA(LPCTSTR pszS, HMODULE hmod, DWORD fdwS) {
		return MMPlaySoundA(pszS, hmod, fdwS);
	}
	
	EXPORT BOOL WINAPI WINMM_PlaySoundW(LPCWSTR pszS, HMODULE hmod, DWORD fdwS) {
		return MMPlaySoundW(pszS, hmod, fdwS);
	}
	
	EXPORT BOOL WINAPI WINMM_sndPlaySoundA(LPCTSTR pszS, DWORD fuS) {
		return MMsndPlaySoundA(pszS, fuS);
	}
	
	EXPORT BOOL WINAPI WINMM_sndPlaySoundW(LPCWSTR pszS, DWORD fuS) {
		return MMsndPlaySoundW(pszS, fuS);
	}
	
	EXPORT DWORD WINAPI WINMM_auxOutMessage(UINT uDID, UINT msg, DWORD_PTR dwP1, DWORD_PTR dwP2) {
		return MMauxOutMessage(uDID, msg, dwP1, dwP2);
	}
	
	EXPORT MMRESULT WINAPI WINMM_auxGetDevCapsA(UINT_PTR uDID, LPAUXCAPSA pac, UINT cbac) {
		return MMauxGetDevCapsA(uDID, pac, cbac);
	}
	
	EXPORT MMRESULT WINAPI WINMM_auxGetDevCapsW(UINT_PTR uDID, LPAUXCAPSW pac, UINT cbac) {
		return MMauxGetDevCapsW(uDID, pac, cbac);
	}
	
	EXPORT MMRESULT WINAPI WINMM_auxGetVolume(UINT uDID, LPDWORD lpV) {
		return MMauxGetVolume(uDID, lpV);
	}
	
	EXPORT MMRESULT WINAPI WINMM_auxSetVolume(UINT uDID, DWORD V) {
		return MMauxSetVolume(uDID, V);
	}
	
	EXPORT UINT WINAPI WINMM_auxGetNumDevs() {
		return MMauxGetNumDevs();
	}
	
	EXPORT DWORD WINAPI WINMM_waveOutMessage(HWAVEOUT hw, UINT msg, DWORD_PTR dwP1, DWORD_PTR dwP2) {
		return MMwaveOutMessage(hw, msg, dwP1, dwP2);
	}
	
	EXPORT MMRESULT WINAPI WINMM_waveOutAddBuffer(HWAVEOUT hw, LPWAVEHDR pch, UINT cch) {
		return MMwaveOutAddBuffer(hw, pch, cch);
	}
	
	EXPORT MMRESULT WINAPI WINMM_waveOutBreakLoop(HWAVEOUT hw) {
		return MMwaveOutBreakLoop(hw);
	}
	
	EXPORT MMRESULT WINAPI WINMM_waveOutClose(HWAVEOUT hw) {
		return MMwaveOutClose(hw);
	}
	
	EXPORT MMRESULT WINAPI WINMM_waveOutGetDevCapsA(UINT_PTR uDID, LPWAVEOUTCAPSA LPWOT, UINT size) {
		return MMwaveOutGetDevCapsA(uDID, LPWOT, size);
	}
	
	EXPORT MMRESULT WINAPI WINMM_waveOutGetDevCapsW(UINT_PTR uDID, LPWAVEOUTCAPSW LPWOT, UINT size) {
		return MMwaveOutGetDevCapsW(uDID, LPWOT, size);
	}
	
	EXPORT MMRESULT WINAPI WINMM_waveOutGetErrorTextA(MMRESULT err, LPTSTR pszT, UINT cchT) {
		return MMwaveOutGetErrorTextA(err, pszT, cchT);
	}
	
	EXPORT MMRESULT WINAPI WINMM_waveOutGetErrorTextW(MMRESULT err, LPWSTR pszT, UINT cchT) {
		return MMwaveOutGetErrorTextW(err, pszT, cchT);
	}
	
	EXPORT MMRESULT WINAPI WINMM_waveOutGetID(HWAVEOUT hw, LPUINT puDID) {
		return MMwaveOutGetID(hw, puDID);
	}
	
	EXPORT MMRESULT WINAPI WINMM_waveOutSetPitch(HWAVEOUT hw, DWORD P) {
		return MMwaveOutSetPitch(hw, P);
	}
	
	EXPORT MMRESULT WINAPI WINMM_waveOutGetPitch(HWAVEOUT hw, LPDWORD pP) {
		return MMwaveOutGetPitch(hw, pP);
	}
	
	EXPORT MMRESULT WINAPI WINMM_waveOutSetPlaybackRate(HWAVEOUT hw, DWORD PR) {
		return MMwaveOutSetPlaybackRate(hw, PR);
	}
	
	EXPORT MMRESULT WINAPI WINMM_waveOutGetPlaybackRate(HWAVEOUT hw, LPDWORD pPR) {
		return MMwaveOutGetPlaybackRate(hw, pPR);
	}
	
	EXPORT MMRESULT WINAPI WINMM_waveOutGetPosition(HWAVEOUT hw, LPMMTIME pmmt, UINT cbmmt) {
		return MMwaveOutGetPosition(hw, pmmt, cbmmt);
	}
	
	EXPORT MMRESULT WINAPI WINMM_waveOutSetVolume(HWAVEOUT hw, DWORD V) {
		return MMwaveOutSetVolume(hw, V);
	}
	
	EXPORT MMRESULT WINAPI WINMM_waveOutGetVolume(HWAVEOUT hw, LPDWORD pV) {
		return MMwaveOutGetVolume(hw, pV);
	}
	
	EXPORT MMRESULT WINAPI WINMM_waveOutOpen(LPHWAVEOUT lphw, UINT uDID, LPCWAVEFORMATEX LPWFEX, DWORD_PTR dwC, DWORD_PTR dwI, DWORD fdwO) {
		return MMwaveOutOpen(lphw, uDID, LPWFEX, dwC, dwI, fdwO);
	}
	
	EXPORT MMRESULT WINAPI WINMM_waveOutPrepareHeader(HWAVEOUT hw, LPWAVEHDR pchB, UINT cchB) {
		return MMwaveOutPrepareHeader(hw, pchB, cchB);
	}
	
	EXPORT MMRESULT WINAPI WINMM_waveOutUnprepareHeader(HWAVEOUT hw, LPWAVEHDR pchB, UINT cchB) {
		return MMwaveOutUnprepareHeader(hw, pchB, cchB);
	}
	
	EXPORT MMRESULT WINAPI WINMM_waveOutWrite(HWAVEOUT hw, LPWAVEHDR pchB, UINT cchB) {
		return MMwaveOutWrite(hw, pchB, cchB);
	}
	
	EXPORT MMRESULT WINAPI WINMM_waveOutReset(HWAVEOUT hw) {
		return MMwaveOutReset(hw);
	}
	
	EXPORT MMRESULT WINAPI WINMM_waveOutRestart(HWAVEOUT hw) {
		return MMwaveOutRestart(hw);
	}
	
	EXPORT MMRESULT WINAPI WINMM_waveOutPause(HWAVEOUT hw) {
		return MMwaveOutPause(hw);
	}
	
	EXPORT MMRESULT WINAPI WINMM_waveOutStart(HWAVEOUT hw) {
		return MMwaveOutStart(hw);
	}
	
	EXPORT MMRESULT WINAPI WINMM_waveOutStop(HWAVEOUT hw) {
		return MMwaveOutStop(hw);
	}
	
	EXPORT UINT WINAPI WINMM_waveOutGetNumDevs() {
		return MMwaveOutGetNumDevs();
	}
	
	EXPORT DWORD WINAPI WINMM_waveInMessage(HWAVEIN hw, UINT msg, DWORD_PTR dwP1, DWORD_PTR dwP2) {
		return MMwaveInMessage(hw, msg, dwP1, dwP2);
	}
	
	EXPORT MMRESULT WINAPI WINMM_waveInAddBuffer(HWAVEIN hw, LPWAVEHDR pch, UINT cch) {
		return MMwaveInAddBuffer(hw, pch, cch);
	}
	
	EXPORT MMRESULT WINAPI WINMM_waveInClose(HWAVEIN hw) {
		return MMwaveInClose(hw);
	}
	
	EXPORT MMRESULT WINAPI WINMM_waveInGetDevCapsA(UINT_PTR uDID, LPWAVEINCAPSA LPWOT, UINT size) {
		return MMwaveInGetDevCapsA(uDID, LPWOT, size);
	}
	
	EXPORT MMRESULT WINAPI WINMM_waveInGetDevCapsW(UINT_PTR uDID, LPWAVEINCAPSW LPWOT, UINT size) {
		return MMwaveInGetDevCapsW(uDID, LPWOT, size);
	}
	
	EXPORT MMRESULT WINAPI WINMM_waveInGetErrorTextA(MMRESULT err, LPTSTR pszT, UINT cchT) {
		return MMwaveInGetErrorTextA(err, pszT, cchT);
	}
	
	EXPORT MMRESULT WINAPI WINMM_waveInGetErrorTextW(MMRESULT err, LPWSTR pszT, UINT cchT) {
		return MMwaveInGetErrorTextW(err, pszT, cchT);
	}
	
	EXPORT MMRESULT WINAPI WINMM_waveInGetID(HWAVEIN hw, LPUINT puDID) {
		return MMwaveInGetID(hw, puDID);
	}
	
	EXPORT MMRESULT WINAPI WINMM_waveInGetPosition(HWAVEIN hw, LPMMTIME pmmt, UINT cbmmt) {
		return MMwaveInGetPosition(hw, pmmt, cbmmt);
	}
	
	EXPORT MMRESULT WINAPI WINMM_waveInOpen(LPHWAVEIN lphw, UINT uDID, LPCWAVEFORMATEX LPWFEX, DWORD_PTR dwC, DWORD_PTR dwI, DWORD fdwO) {
		return MMwaveInOpen(lphw, uDID, LPWFEX, dwC, dwI, fdwO);
	}
	
	EXPORT MMRESULT WINAPI WINMM_waveInPrepareHeader(HWAVEIN hw, LPWAVEHDR pchB, UINT cchB) {
		return MMwaveInPrepareHeader(hw, pchB, cchB);
	}
	
	EXPORT MMRESULT WINAPI WINMM_waveInUnprepareHeader(HWAVEIN hw, LPWAVEHDR pchB, UINT cchB) {
		return MMwaveInUnprepareHeader(hw, pchB, cchB);
	}
	
	EXPORT MMRESULT WINAPI WINMM_waveInReset(HWAVEIN hw) {
		return MMwaveInReset(hw);
	}
	
	EXPORT MMRESULT WINAPI WINMM_waveInStart(HWAVEIN hw) {
		return MMwaveInStart(hw);
	}
	
	EXPORT MMRESULT WINAPI WINMM_waveInStop(HWAVEIN hw) {
		return MMwaveInStop(hw);
	}
	
	EXPORT UINT WINAPI WINMM_waveInGetNumDevs() {
		return MMwaveInGetNumDevs();
	}
	
	EXPORT DWORD WINAPI WINMM_timeGetTime() {
		return MMtimeGetTime();
	}
	
	EXPORT MMRESULT WINAPI WINMM_timeGetDevCaps(LPTIMECAPS ptc, UINT cbtc) {
		return MMtimeGetDevCaps(ptc, cbtc);
	}
	
	EXPORT MMRESULT WINAPI WINMM_timeSetEvent(UINT uDelay, UINT uResolution, LPTIMECALLBACK lpTimeProc, DWORD_PTR dwUser, UINT fuEvent) {
		return MMtimeSetEvent(uDelay, uResolution, lpTimeProc, dwUser, fuEvent);
	}
	
	EXPORT MMRESULT WINAPI WINMM_timeKillEvent(UINT uTimerID) {
		return MMtimeKillEvent(uTimerID);
	}
	
	EXPORT MMRESULT WINAPI WINMM_timeGetSystemTime(LPMMTIME pmmt, UINT cbmmt) {
		return MMtimeGetSystemTime(pmmt, cbmmt);
	}
	
	EXPORT MMRESULT WINAPI WINMM_timeBeginPeriod(UINT uPeriod) {
		return MMtimeBeginPeriod(uPeriod);
	}
	
	EXPORT MMRESULT WINAPI WINMM_timeEndPeriod(UINT uPeriod) {
		return MMtimeEndPeriod(uPeriod);
	}
	
	#ifdef _M_IX86
	EXPORT MMRESULT WINAPI WINMM_aux32Message(UINT_PTR uDeviceID, UINT uMsg, DWORD_PTR Handle, DWORD_PTR dwParam1, DWORD_PTR dwParam2) {
		return MMaux32Message(uDeviceID, uMsg, Handle, dwParam1, dwParam2);
	}
	
	EXPORT MMRESULT WINAPI WINMM_joy32Message(UINT_PTR uDeviceID, UINT uMsg, DWORD_PTR Handle, DWORD_PTR dwParam1, DWORD_PTR dwParam2) {
		return MMjoy32Message(uDeviceID, uMsg, Handle, dwParam1, dwParam2);
	}
	
	EXPORT MMRESULT WINAPI WINMM_mci32Message(UINT_PTR uDeviceID, UINT uMsg, DWORD_PTR Handle, DWORD_PTR dwParam1, DWORD_PTR dwParam2) {
		return MMmci32Message(uDeviceID, uMsg, Handle, dwParam1, dwParam2);
	}
	
	EXPORT MMRESULT WINAPI WINMM_mid32Message(UINT_PTR uDeviceID, UINT uMsg, DWORD_PTR Handle, DWORD_PTR dwParam1, DWORD_PTR dwParam2) {
		return MMmid32Message(uDeviceID, uMsg, Handle, dwParam1, dwParam2);
	}
	
	EXPORT MMRESULT WINAPI WINMM_mod32Message(UINT_PTR uDeviceID, UINT uMsg, DWORD_PTR Handle, DWORD_PTR dwParam1, DWORD_PTR dwParam2) {
		return MMmod32Message(uDeviceID, uMsg, Handle, dwParam1, dwParam2);
	}
	
	EXPORT MMRESULT WINAPI WINMM_mxd32Message(UINT_PTR uDeviceID, UINT uMsg, DWORD_PTR Handle, DWORD_PTR dwParam1, DWORD_PTR dwParam2) {
		return MMmxd32Message(uDeviceID, uMsg, Handle, dwParam1, dwParam2);
	}
	
	EXPORT MMRESULT WINAPI WINMM_tid32Message(UINT_PTR uDeviceID, UINT uMsg, DWORD_PTR Handle, DWORD_PTR dwParam1, DWORD_PTR dwParam2) {
		return MMtid32Message(uDeviceID, uMsg, Handle, dwParam1, dwParam2);
	}
	
	EXPORT MMRESULT WINAPI WINMM_wid32Message(UINT_PTR uDeviceID, UINT uMsg, DWORD_PTR hMidi, DWORD_PTR dwParam1, DWORD_PTR dwParam2) {
		return MMwid32Message(uDeviceID, uMsg, hMidi, dwParam1, dwParam2);
	}
	
	EXPORT MMRESULT WINAPI WINMM_wod32Message(UINT_PTR uDeviceID, UINT uMsg, DWORD_PTR hMidi, DWORD_PTR dwParam1, DWORD_PTR dwParam2) {
		return MMwod32Message(uDeviceID, uMsg, hMidi, dwParam1, dwParam2);
	}
	#endif
}