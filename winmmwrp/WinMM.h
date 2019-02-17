// Windows Multimedia original functions, used to redirect unedited functions without relying on definition files

// WinMM funcs, just replace MM with "midiOut" to get the real version
static HMODULE OWINMM = NULL;
static DWORD(WINAPI*INtimeGetTime)() = 0;

// MIDI out stuff
static MMRESULT(WINAPI*MMmidiStreamClose)(HMIDISTRM) = 0;
static MMRESULT(WINAPI*MMmidiStreamOpen)(LPHMIDISTRM, LPUINT, DWORD, DWORD_PTR, DWORD_PTR, DWORD) = 0;
static MMRESULT(WINAPI*MMmidiStreamOut)(HMIDISTRM, LPMIDIHDR, UINT) = 0;
static MMRESULT(WINAPI*MMmidiStreamPause)(HMIDISTRM) = 0;
static MMRESULT(WINAPI*MMmidiStreamPosition)(HMIDISTRM, LPMMTIME, UINT) = 0;
static MMRESULT(WINAPI*MMmidiStreamProperty)(HMIDISTRM, LPBYTE, DWORD) = 0;
static MMRESULT(WINAPI*MMmidiStreamRestart)(HMIDISTRM) = 0;
static MMRESULT(WINAPI*MMmidiStreamStop)(HMIDISTRM) = 0;
static MMRESULT(WINAPI*MMmidiOutCacheDrumPatches)(HMIDIOUT, UINT, LPWORD, UINT) = 0;
static MMRESULT(WINAPI*MMmidiOutCachePatches)(HMIDIOUT, UINT, LPWORD, UINT) = 0;
static MMRESULT(WINAPI*MMmidiOutClose)(HMIDIOUT) = 0;
static MMRESULT(WINAPI*MMmidiOutGetDevCapsA)(UINT_PTR, LPMIDIOUTCAPSA, UINT) = 0;
static MMRESULT(WINAPI*MMmidiOutGetDevCapsW)(UINT_PTR, LPMIDIOUTCAPSW, UINT) = 0;
static MMRESULT(WINAPI*MMmidiOutGetErrorTextA)(MMRESULT, LPSTR, UINT) = 0;
static MMRESULT(WINAPI*MMmidiOutGetErrorTextW)(MMRESULT, LPWSTR, UINT) = 0;
static MMRESULT(WINAPI*MMmidiOutGetID)(HMIDIOUT, LPUINT) = 0;
static MMRESULT(WINAPI*MMmidiOutGetVolume)(HMIDIOUT, LPDWORD) = 0;
static MMRESULT(WINAPI*MMmidiOutLongMsg)(HMIDIOUT, LPMIDIHDR, UINT) = 0;
static MMRESULT(WINAPI*MMmidiOutMessage)(HMIDIOUT, UINT, DWORD_PTR, DWORD_PTR) = 0;
static MMRESULT(WINAPI*MMmidiOutOpen)(LPHMIDIOUT, UINT, DWORD_PTR, DWORD_PTR, DWORD) = 0;
static MMRESULT(WINAPI*MMmidiOutPrepareHeader)(HMIDIOUT, LPMIDIHDR, UINT) = 0;
static MMRESULT(WINAPI*MMmidiOutReset)(HMIDIOUT) = 0;
static MMRESULT(WINAPI*MMmidiOutSetVolume)(HMIDIOUT, DWORD) = 0;
static MMRESULT(WINAPI*MMmidiOutShortMsg)(HMIDIOUT, DWORD) = 0;
static MMRESULT(WINAPI*MMmidiOutUnprepareHeader)(HMIDIOUT, LPMIDIHDR, UINT) = 0;
static UINT(WINAPI*MMmidiOutGetNumDevs)() = 0;
// MIDI out stuff

// MIDI in stuff
static MMRESULT(WINAPI*MMmidiConnect)(HMIDI, HMIDIOUT, LPVOID) = 0;
static MMRESULT(WINAPI*MMmidiDisconnect)(HMIDI, HMIDIOUT, LPVOID) = 0;
static MMRESULT(WINAPI*MMmidiInAddBuffer)(HMIDIIN, LPMIDIHDR, UINT) = 0;
static MMRESULT(WINAPI*MMmidiInClose)(HMIDIIN) = 0;
static MMRESULT(WINAPI*MMmidiInGetDevCapsA)(UINT_PTR, LPMIDIINCAPSA, UINT) = 0;
static MMRESULT(WINAPI*MMmidiInGetDevCapsW)(UINT_PTR, LPMIDIINCAPSW, UINT) = 0;
static MMRESULT(WINAPI*MMmidiInGetErrorTextA)(MMRESULT, LPSTR, UINT) = 0;
static MMRESULT(WINAPI*MMmidiInGetErrorTextW)(MMRESULT, LPWSTR, UINT) = 0;
static MMRESULT(WINAPI*MMmidiInGetID)(HMIDIIN, LPUINT) = 0;
static MMRESULT(WINAPI*MMmidiInMessage)(HMIDIIN, UINT, DWORD_PTR, DWORD_PTR) = 0;
static MMRESULT(WINAPI*MMmidiInOpen)(LPHMIDIIN, UINT, DWORD_PTR, DWORD_PTR, DWORD) = 0;
static MMRESULT(WINAPI*MMmidiInPrepareHeader)(HMIDIIN, LPMIDIHDR, UINT) = 0;
static MMRESULT(WINAPI*MMmidiInReset)(HMIDIIN) = 0;
static MMRESULT(WINAPI*MMmidiInStart)(HMIDIIN) = 0;
static MMRESULT(WINAPI*MMmidiInStop)(HMIDIIN) = 0;
static MMRESULT(WINAPI*MMmidiInUnprepareHeader)(HMIDIIN, LPMIDIHDR, UINT) = 0;
static UINT(WINAPI*MMmidiInGetNumDevs)() = 0;
// MIDI in stuff

// Time related stuff
static DWORD(WINAPI*MMtimeGetTime)() = 0;
static MMRESULT(WINAPI*MMtimeBeginPeriod)(UINT) = 0;
static MMRESULT(WINAPI*MMtimeEndPeriod)(UINT) = 0;
static MMRESULT(WINAPI*MMtimeGetDevCaps)(LPTIMECAPS, UINT) = 0;
static MMRESULT(WINAPI*MMtimeGetSystemTime)(LPMMTIME, UINT) = 0;
static MMRESULT(WINAPI*MMtimeKillEvent)(UINT) = 0;
static MMRESULT(WINAPI*MMtimeSetEvent)(UINT, UINT, LPTIMECALLBACK, DWORD_PTR, UINT) = 0;
// Time related stuff

// Driver stuff

static HDRVR(WINAPI*MMOpenDriver)(_In_ LPCWSTR, _In_ LPCWSTR, _In_ LPARAM) = 0;
static LRESULT(WINAPI*MMCloseDriver)(_In_ HDRVR, _In_ LPARAM, _In_ LPARAM) = 0;
static LRESULT(WINAPI*MMSendDriverMessage)(_In_ HDRVR, _In_ UINT, _Inout_ LPARAM, _Inout_ LPARAM) = 0;
static BOOL(WINAPI*MMmmDrvInstall)(LPCSTR, LPCSTR, BOOL) = 0;
static DWORD(WINAPI*MMmmGetCurrentTask)() = 0;
static HMODULE(WINAPI*MMDrvGetModuleHandle)(_In_ HDRVR) = 0;
static HMODULE(WINAPI*MMGetDriverModuleHandle)(_In_ HDRVR) = 0;
static LRESULT(WINAPI*MMDefDriverProc)(DWORD_PTR, HDRVR, UINT, LONG, LONG) = 0;
static LRESULT(WINAPI*MMDriverCallback)(DWORD, DWORD, HDRVR, DWORD, DWORD, DWORD, DWORD) = 0;
// Driver stuff

// Game-related stuff
static MMRESULT(WINAPI*MMjoyConfigChanged)(DWORD) = 0;
static MMRESULT(WINAPI*MMjoyGetDevCapsA)(UINT, LPJOYCAPSA, UINT) = 0;
static MMRESULT(WINAPI*MMjoyGetDevCapsW)(UINT, LPJOYCAPSW, UINT) = 0;
static MMRESULT(WINAPI*MMjoyGetPos)(UINT, LPJOYINFO) = 0;
static MMRESULT(WINAPI*MMjoyGetPosEx)(UINT, LPJOYINFOEX) = 0;
static MMRESULT(WINAPI*MMjoyGetThreshold)(UINT, LPUINT) = 0;
static MMRESULT(WINAPI*MMjoySetThreshold)(UINT, UINT) = 0;
static MMRESULT(WINAPI*MMjoyReleaseCapture)(UINT) = 0;
static MMRESULT(WINAPI*MMjoySetCapture)(HWND, UINT, UINT, BOOL) = 0;
static UINT(WINAPI*MMjoyGetNumDevs)() = 0;
// Game related stuff

// MCI stuff
static BOOL(WINAPI*MMmciDriverNotify)(HWND, UINT, UINT) = 0;
static BOOL(WINAPI*MMmciExecute)(LPCSTR) = 0;
static BOOL(WINAPI*MMmciFreeCommandResource)(UINT) = 0;
static BOOL(WINAPI*MMmciGetErrorStringA)(DWORD, LPTSTR, UINT) = 0;
static BOOL(WINAPI*MMmciGetErrorStringW)(DWORD, LPWSTR, UINT) = 0;
static BOOL(WINAPI*MMmciSetDriverData)(UINT, DWORD) = 0;
static DWORD(WINAPI*MMmciGetDriverData)(UINT) = 0;
static HANDLE(WINAPI*MMmciGetCreatorTask)(MCIDEVICEID) = 0;
static MCIDEVICEID(WINAPI*MMmciGetDeviceIDA)(LPCTSTR) = 0;
static MCIDEVICEID(WINAPI*MMmciGetDeviceIDFromElementIDA)(DWORD, LPCTSTR) = 0;
static MCIDEVICEID(WINAPI*MMmciGetDeviceIDFromElementIDW)(DWORD, LPCWSTR) = 0;
static MCIDEVICEID(WINAPI*MMmciGetDeviceIDW)(LPCWSTR) = 0;
static MCIERROR(WINAPI*MMmciSendCommandA)(MCIDEVICEID, UINT, DWORD_PTR, DWORD_PTR) = 0;
static MCIERROR(WINAPI*MMmciSendCommandW)(MCIDEVICEID, UINT, DWORD_PTR, DWORD_PTR) = 0;
static MCIERROR(WINAPI*MMmciSendStringA)(LPCTSTR, LPTSTR, UINT, HANDLE) = 0;
static MCIERROR(WINAPI*MMmciSendStringW)(LPCWSTR, LPWSTR, UINT, HANDLE) = 0;
static UINT(WINAPI*MMmciDriverYield)(UINT) = 0;
static UINT(WINAPI*MMmciLoadCommandResource)(HINSTANCE, LPCWSTR, UINT) = 0;
static UINT(WINAPI*MMmciSetYieldProc)(MCIDEVICEID, YIELDPROC, DWORD) = 0;
static YIELDPROC(WINAPI*MMmciGetYieldProc)(MCIDEVICEID, LPDWORD) = 0;
// MCI stuff

// Mixer stuff
static DWORD(WINAPI*MMmixerMessage)(HMIXER, UINT, DWORD_PTR, DWORD_PTR) = 0;
static MMRESULT(WINAPI*MMmixerClose)(HMIXER) = 0;
static MMRESULT(WINAPI*MMmixerGetControlDetailsA)(HMIXEROBJ, LPMIXERCONTROLDETAILS, DWORD) = 0;
static MMRESULT(WINAPI*MMmixerGetControlDetailsW)(HMIXEROBJ, LPMIXERCONTROLDETAILS, DWORD) = 0;
static MMRESULT(WINAPI*MMmixerGetDevCapsA)(UINT_PTR, LPMIXERCAPSA, UINT) = 0;
static MMRESULT(WINAPI*MMmixerGetDevCapsW)(UINT_PTR, LPMIXERCAPSW, UINT) = 0;
static MMRESULT(WINAPI*MMmixerGetID)(HMIXEROBJ, UINT FAR *, DWORD) = 0;
static MMRESULT(WINAPI*MMmixerGetLineControlsA)(HMIXEROBJ, LPMIXERLINECONTROLSA, DWORD) = 0;
static MMRESULT(WINAPI*MMmixerGetLineControlsW)(HMIXEROBJ, LPMIXERLINECONTROLSW, DWORD) = 0;
static MMRESULT(WINAPI*MMmixerGetLineInfoA)(HMIXEROBJ, LPMIXERLINEA, DWORD) = 0;
static MMRESULT(WINAPI*MMmixerGetLineInfoW)(HMIXEROBJ, LPMIXERLINEW, DWORD) = 0;
static MMRESULT(WINAPI*MMmixerOpen)(LPHMIXER, UINT, DWORD_PTR, DWORD_PTR, DWORD) = 0;
static MMRESULT(WINAPI*MMmixerSetControlDetails)(HMIXEROBJ, LPMIXERCONTROLDETAILS, DWORD) = 0;
static UINT(WINAPI*MMmixerGetNumDevs)() = 0;
// Mixer stuff

// MMIO stuff
static FOURCC(WINAPI*MMmmioStringToFOURCCA)(LPCTSTR, UINT) = 0;
static FOURCC(WINAPI*MMmmioStringToFOURCCW)(LPCWSTR, UINT) = 0;
static LONG(WINAPI*MMmmioRead)(HMMIO, HPSTR, LONG) = 0;
static LONG(WINAPI*MMmmioSeek)(HMMIO, LONG, INT) = 0;
static LONG(WINAPI*MMmmioWrite)(HMMIO, char _huge *, LONG) = 0;
static LPMMIOPROC(WINAPI*MMmmioInstallIOProcA)(FOURCC, LPMMIOPROC, DWORD) = 0;
static LPMMIOPROC(WINAPI*MMmmioInstallIOProcW)(FOURCC, LPMMIOPROC, DWORD) = 0;
static LRESULT(WINAPI*MMmmioSendMessage)(HMMIO, UINT, LPARAM, LPARAM) = 0;
static MMRESULT(WINAPI*MMmmioAdvance)(HMMIO, LPMMIOINFO, UINT) = 0;
static MMRESULT(WINAPI*MMmmioAscend)(HMMIO, LPMMCKINFO, UINT) = 0;
static MMRESULT(WINAPI*MMmmioClose)(HMMIO, UINT) = 0;
static MMRESULT(WINAPI*MMmmioCreateChunk)(HMMIO, LPMMCKINFO, UINT) = 0;
static MMRESULT(WINAPI*MMmmioDescend)(HMMIO, LPMMCKINFO, const MMCKINFO *, UINT) = 0;
static MMRESULT(WINAPI*MMmmioFlush)(HMMIO, UINT) = 0;
static MMRESULT(WINAPI*MMmmioGetInfo)(HMMIO, LPMMIOINFO, UINT) = 0;
static MMRESULT(WINAPI*MMmmioOpenA)(LPTSTR, LPMMIOINFO, DWORD) = 0;
static MMRESULT(WINAPI*MMmmioOpenW)(LPWSTR, LPMMIOINFO, DWORD) = 0;
static MMRESULT(WINAPI*MMmmioRenameA)(LPCTSTR, LPCTSTR, const LPMMIOINFO, DWORD);
static MMRESULT(WINAPI*MMmmioRenameW)(LPCWSTR, LPCWSTR, const LPMMIOINFO, DWORD);
static MMRESULT(WINAPI*MMmmioSetBuffer)(HMMIO, LPSTR, LONG, UINT) = 0;
static MMRESULT(WINAPI*MMmmioSetInfo)(HMMIO, LPMMIOINFO, UINT) = 0;
// MMIO stuff

// MM stuff
static BOOL(WINAPI*MMmmTaskSignal)(DWORD) = 0;
static UINT(WINAPI*MMmmTaskCreate)(LPTASKCALLBACK, HANDLE, DWORD_PTR) = 0;
static VOID(WINAPI*MMmmTaskBlock)(DWORD) = 0;
static VOID(WINAPI*MMmmTaskYield)() = 0;
// MM stuff

// PlaySound stuff
static BOOL(WINAPI*MMPlaySoundA)(LPCTSTR, HMODULE, DWORD) = 0;
static BOOL(WINAPI*MMPlaySoundW)(LPCWSTR, HMODULE, DWORD) = 0;
static BOOL(WINAPI*MMsndPlaySoundA)(LPCWSTR, UINT) = 0;
static BOOL(WINAPI*MMsndPlaySoundW)(LPCTSTR, UINT) = 0;
// PlaySound stuff

// Aux stuff
static DWORD(WINAPI*MMauxOutMessage)(UINT, UINT, DWORD_PTR, DWORD_PTR) = 0;
static MMRESULT(WINAPI*MMauxGetDevCapsA)(UINT_PTR, LPAUXCAPSA, UINT) = 0;
static MMRESULT(WINAPI*MMauxGetDevCapsW)(UINT_PTR, LPAUXCAPSW, UINT) = 0;
static MMRESULT(WINAPI*MMauxGetVolume)(UINT, LPDWORD) = 0;
static MMRESULT(WINAPI*MMauxSetVolume)(UINT, DWORD) = 0;
static UINT(WINAPI*MMauxGetNumDevs)() = 0;
// Aux stuff

// Wave out stuff
static DWORD(WINAPI*MMwaveOutMessage)(HWAVEOUT, UINT, DWORD_PTR, DWORD_PTR) = 0;
static MMRESULT(WINAPI*MMwaveOutAddBuffer)(HWAVEOUT, LPWAVEHDR, UINT) = 0;
static MMRESULT(WINAPI*MMwaveOutBreakLoop)(HWAVEOUT) = 0;
static MMRESULT(WINAPI*MMwaveOutClose)(HWAVEOUT) = 0;
static MMRESULT(WINAPI*MMwaveOutGetDevCapsA)(UINT_PTR, LPWAVEOUTCAPSA, UINT) = 0;
static MMRESULT(WINAPI*MMwaveOutGetDevCapsW)(UINT_PTR, LPWAVEOUTCAPSW, UINT) = 0;
static MMRESULT(WINAPI*MMwaveOutGetErrorTextA)(MMRESULT, LPTSTR, UINT) = 0;
static MMRESULT(WINAPI*MMwaveOutGetErrorTextW)(MMRESULT, LPWSTR, UINT) = 0;
static MMRESULT(WINAPI*MMwaveOutGetID)(HWAVEOUT, LPUINT) = 0;
static MMRESULT(WINAPI*MMwaveOutGetPitch)(HWAVEOUT, LPDWORD) = 0;
static MMRESULT(WINAPI*MMwaveOutGetPlaybackRate)(HWAVEOUT, LPDWORD) = 0;
static MMRESULT(WINAPI*MMwaveOutGetPosition)(HWAVEOUT, LPMMTIME, UINT) = 0;
static MMRESULT(WINAPI*MMwaveOutGetVolume)(HWAVEOUT, LPDWORD) = 0;
static MMRESULT(WINAPI*MMwaveOutOpen)(LPHWAVEOUT, UINT, LPCWAVEFORMATEX, DWORD_PTR, DWORD_PTR, DWORD) = 0;
static MMRESULT(WINAPI*MMwaveOutPrepareHeader)(HWAVEOUT, LPWAVEHDR, UINT) = 0;
static MMRESULT(WINAPI*MMwaveOutReset)(HWAVEOUT) = 0;
static MMRESULT(WINAPI*MMwaveOutRestart)(HWAVEOUT) = 0;
static MMRESULT(WINAPI*MMwaveOutPause)(HWAVEOUT) = 0;
static MMRESULT(WINAPI*MMwaveOutSetPitch)(HWAVEOUT, DWORD) = 0;
static MMRESULT(WINAPI*MMwaveOutSetPlaybackRate)(HWAVEOUT, DWORD) = 0;
static MMRESULT(WINAPI*MMwaveOutSetVolume)(HWAVEOUT, DWORD) = 0;
static MMRESULT(WINAPI*MMwaveOutStart)(HWAVEOUT) = 0;
static MMRESULT(WINAPI*MMwaveOutStop)(HWAVEOUT) = 0;
static MMRESULT(WINAPI*MMwaveOutUnprepareHeader)(HWAVEOUT, LPWAVEHDR, UINT) = 0;
static MMRESULT(WINAPI*MMwaveOutWrite)(HWAVEOUT, LPWAVEHDR, UINT) = 0;
static UINT(WINAPI*MMwaveOutGetNumDevs)() = 0;
// Wave out stuff

// Wave in stuff
static DWORD(WINAPI*MMwaveInMessage)(HWAVEIN, UINT, DWORD_PTR, DWORD_PTR) = 0;
static MMRESULT(WINAPI*MMwaveInAddBuffer)(HWAVEIN, LPWAVEHDR, UINT) = 0;
static MMRESULT(WINAPI*MMwaveInClose)(HWAVEIN) = 0;
static MMRESULT(WINAPI*MMwaveInGetDevCapsA)(UINT_PTR, LPWAVEINCAPSA, UINT) = 0;
static MMRESULT(WINAPI*MMwaveInGetDevCapsW)(UINT_PTR, LPWAVEINCAPSW, UINT) = 0;
static MMRESULT(WINAPI*MMwaveInGetErrorTextA)(MMRESULT, LPTSTR, UINT) = 0;
static MMRESULT(WINAPI*MMwaveInGetErrorTextW)(MMRESULT, LPWSTR, UINT) = 0;
static MMRESULT(WINAPI*MMwaveInGetID)(HWAVEIN, LPUINT) = 0;
static MMRESULT(WINAPI*MMwaveInGetPosition)(HWAVEIN, LPMMTIME, UINT) = 0;
static MMRESULT(WINAPI*MMwaveInOpen)(LPHWAVEIN, UINT, LPCWAVEFORMATEX, DWORD_PTR, DWORD_PTR, DWORD) = 0;
static MMRESULT(WINAPI*MMwaveInPrepareHeader)(HWAVEIN, LPWAVEHDR, UINT) = 0;
static MMRESULT(WINAPI*MMwaveInReset)(HWAVEIN) = 0;
static MMRESULT(WINAPI*MMwaveInStart)(HWAVEIN) = 0;
static MMRESULT(WINAPI*MMwaveInStop)(HWAVEIN) = 0;
static MMRESULT(WINAPI*MMwaveInUnprepareHeader)(HWAVEIN, LPWAVEHDR, UINT) = 0;
static UINT(WINAPI*MMwaveInGetNumDevs)() = 0;
// Wave in stuff

// Quickity quackity
#define MMI(f) {& MM##f, #f}
struct MMImport
{
	void** ptr;
	const char* name;
} MMImports[] =
{
	MMI(CloseDriver),
	MMI(DefDriverProc),
	MMI(DriverCallback),
	MMI(DrvGetModuleHandle),
	MMI(GetDriverModuleHandle),
	MMI(OpenDriver),
	MMI(PlaySoundA),
	MMI(PlaySoundW),
	MMI(SendDriverMessage),
	MMI(auxGetDevCapsA),
	MMI(auxGetDevCapsW),
	MMI(auxGetNumDevs),
	MMI(auxGetVolume),
	MMI(auxOutMessage),
	MMI(auxSetVolume),
	MMI(joyConfigChanged),
	MMI(joyGetDevCapsA),
	MMI(joyGetDevCapsW),
	MMI(joyGetNumDevs),
	MMI(joyGetPos),
	MMI(joyGetPosEx),
	MMI(joyGetThreshold),
	MMI(joyReleaseCapture),
	MMI(joySetCapture),
	MMI(joySetThreshold),
	MMI(mciDriverNotify),
	MMI(mciDriverYield),
	MMI(mciExecute),
	MMI(mciFreeCommandResource),
	MMI(mciGetCreatorTask),
	MMI(mciGetDeviceIDA),
	MMI(mciGetDeviceIDFromElementIDA),
	MMI(mciGetDeviceIDFromElementIDW),
	MMI(mciGetDeviceIDW),
	MMI(mciGetDriverData),
	MMI(mciGetErrorStringA),
	MMI(mciGetErrorStringW),
	MMI(mciGetYieldProc),
	MMI(mciLoadCommandResource),
	MMI(mciSendCommandA),
	MMI(mciSendCommandW),
	MMI(mciSendStringA),
	MMI(mciSendStringW),
	MMI(mciSetDriverData),
	MMI(mciSetYieldProc),
	MMI(midiConnect),
	MMI(midiDisconnect),
	MMI(midiInAddBuffer),
	MMI(midiInClose),
	MMI(midiInGetDevCapsA),
	MMI(midiInGetDevCapsW),
	MMI(midiInGetErrorTextA),
	MMI(midiInGetErrorTextW),
	MMI(midiInGetID),
	MMI(midiInGetNumDevs),
	MMI(midiInMessage),
	MMI(midiInOpen),
	MMI(midiInPrepareHeader),
	MMI(midiInReset),
	MMI(midiInStart),
	MMI(midiInStop),
	MMI(midiInUnprepareHeader),
	MMI(midiOutCacheDrumPatches),
	MMI(midiOutCachePatches),
	MMI(midiOutClose),
	MMI(midiOutGetDevCapsA),
	MMI(midiOutGetDevCapsW),
	MMI(midiOutGetErrorTextA),
	MMI(midiOutGetErrorTextW),
	MMI(midiOutGetID),
	MMI(midiOutGetNumDevs),
	MMI(midiOutGetVolume),
	MMI(midiOutLongMsg),
	MMI(midiOutMessage),
	MMI(midiOutOpen),
	MMI(midiOutPrepareHeader),
	MMI(midiOutReset),
	MMI(midiOutSetVolume),
	MMI(midiOutShortMsg),
	MMI(midiOutUnprepareHeader),
	MMI(midiStreamClose),
	MMI(midiStreamOpen),
	MMI(midiStreamOut),
	MMI(midiStreamPause),
	MMI(midiStreamPosition),
	MMI(midiStreamProperty),
	MMI(midiStreamRestart),
	MMI(midiStreamStop),
	MMI(mixerClose),
	MMI(mixerGetControlDetailsA),
	MMI(mixerGetControlDetailsW),
	MMI(mixerGetDevCapsA),
	MMI(mixerGetDevCapsW),
	MMI(mixerGetID),
	MMI(mixerGetLineControlsA),
	MMI(mixerGetLineControlsW),
	MMI(mixerGetLineInfoA),
	MMI(mixerGetLineInfoW),
	MMI(mixerGetNumDevs),
	MMI(mixerMessage),
	MMI(mixerOpen),
	MMI(mixerSetControlDetails),
	MMI(mmGetCurrentTask),
	MMI(mmTaskBlock),
	MMI(mmTaskCreate),
	MMI(mmTaskSignal),
	MMI(mmTaskYield),
	MMI(mmioAdvance),
	MMI(mmioAscend),
	MMI(mmioClose),
	MMI(mmioCreateChunk),
	MMI(mmioDescend),
	MMI(mmioFlush),
	MMI(mmioGetInfo),
	MMI(mmioInstallIOProcA),
	MMI(mmioInstallIOProcW),
	MMI(mmioOpenA),
	MMI(mmioOpenW),
	MMI(mmioRead),
	MMI(mmioRenameA),
	MMI(mmioRenameW),
	MMI(mmioSeek),
	MMI(mmioSendMessage),
	MMI(mmioSetBuffer),
	MMI(mmioSetInfo),
	MMI(mmioStringToFOURCCA),
	MMI(mmioStringToFOURCCW),
	MMI(mmioWrite),
	MMI(sndPlaySoundA),
	MMI(sndPlaySoundW),
	MMI(timeBeginPeriod),
	MMI(timeEndPeriod),
	MMI(timeGetDevCaps),
	MMI(timeGetSystemTime),
	MMI(timeGetTime),
	MMI(timeKillEvent),
	MMI(timeSetEvent),
	MMI(waveInAddBuffer),
	MMI(waveInClose),
	MMI(waveInGetDevCapsA),
	MMI(waveInGetDevCapsW),
	MMI(waveInGetErrorTextA),
	MMI(waveInGetErrorTextW),
	MMI(waveInGetID),
	MMI(waveInGetNumDevs),
	MMI(waveInGetPosition),
	MMI(waveInMessage),
	MMI(waveInOpen),
	MMI(waveInPrepareHeader),
	MMI(waveInReset),
	MMI(waveInStart),
	MMI(waveInStop),
	MMI(waveInUnprepareHeader),
	MMI(waveOutBreakLoop),
	MMI(waveOutClose),
	MMI(waveOutGetDevCapsA),
	MMI(waveOutGetDevCapsW),
	MMI(waveOutGetErrorTextA),
	MMI(waveOutGetErrorTextW),
	MMI(waveOutGetID),
	MMI(waveOutGetNumDevs),
	MMI(waveOutGetPitch),
	MMI(waveOutGetPlaybackRate),
	MMI(waveOutGetPosition),
	MMI(waveOutGetVolume),
	MMI(waveOutMessage),
	MMI(waveOutOpen),
	MMI(waveOutPause),
	MMI(waveOutPrepareHeader),
	MMI(waveOutReset),
	MMI(waveOutRestart),
	MMI(waveOutSetPitch),
	MMI(waveOutSetPlaybackRate),
	MMI(waveOutSetVolume),
	MMI(waveOutUnprepareHeader),
	MMI(waveOutWrite)
};

DWORD WINAPI INTERNAL_timeGetTime() {
	return (DWORD)TGT64();
}

BOOL InitializeWinMM() {
	// Load WinMM
	wchar_t SystemDirectory[MAX_PATH];
	GetSystemDirectoryW(SystemDirectory, MAX_PATH);
	wcscat(SystemDirectory, L"\\winmm.dll");

	OWINMM = LoadLibraryW(SystemDirectory);
	if (!OWINMM) {
		MessageBox(
			NULL,
			"Failed to initialize the Windows Multimedia API!\nThe interpreter won't work if Windows Multimedia isn't present.\n\nPress OK to exit.",
			"Keppy's Direct MIDI API",
			MB_ICONERROR | MB_OK | MB_SYSTEMMODAL
		);

		return FALSE;
	}

	// LOAD EVERYTHING!
	for (int i = 0; i < sizeof(MMImports) / sizeof(MMImports[0]); i++)
	{
		*(MMImports[i].ptr) = (void*)GetProcAddress(OWINMM, MMImports[i].name);

		if (!*(MMImports[i].ptr)) {
			MessageBoxA(NULL, MMImports[i].name, "KDMAPI ERROR", MB_ICONERROR | MB_SYSTEMMODAL);
			return FALSE;
		}
	}

	// Check if the user wants to use WinMM's stock TGT or internal version
	HKEY hKey;
	DWORD dwType = REG_DWORD, dwSize = sizeof(DWORD);
	BOOL val = FALSE;
	LSTATUS op = RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\OmniMIDI\\Configuration", 0, KEY_READ, &hKey);
	RegQueryValueEx(hKey, "NewTGT", NULL, &dwType, (LPBYTE)&val, &dwSize);

	if (val) INtimeGetTime = INTERNAL_timeGetTime;
	else INtimeGetTime = MMtimeGetTime;

	RegCloseKey(hKey);

	return TRUE;
}

// Functions start -> HERE <-

DWORD KDMAPI_timeGetTime() {
	return INtimeGetTime();
}

HDRVR WINMM_OpenDriver(_In_ LPCWSTR lpDN, _In_ LPCWSTR lpSN, _In_ LPARAM lp) {
	return MMOpenDriver(lpDN, lpSN, lp);
}

LRESULT WINMM_CloseDriver(_In_ HDRVR drv, _In_ LPARAM lP1, _In_ LPARAM lP2) {
	return MMCloseDriver(drv, lP1, lP2);
}

LRESULT WINMM_SendDriverMessage(_In_ HDRVR drv, _In_ UINT msg, _Inout_ LPARAM lP1, _Inout_ LPARAM lP2) {
	return MMSendDriverMessage(drv, msg, lP1, lP2);
}

DWORD WINMM_mmGetCurrentTask() {
	return MMmmGetCurrentTask();
}

HMODULE WINMM_DrvGetModuleHandle(_In_ HDRVR drv) {
	return MMDrvGetModuleHandle(drv);
}

HMODULE WINMM_GetDriverModuleHandle(_In_ HDRVR drv) {
	return MMGetDriverModuleHandle(drv);
}

LRESULT WINMM_DefDriverProc(DWORD_PTR dwDId, HDRVR drv, UINT msg, LONG lP1, LONG lP2) {
	return MMDefDriverProc(dwDId, drv, msg, lP1, lP2);
}

LRESULT WINMM_DriverCallback(DWORD dwC, DWORD dwF, HDRVR drv, DWORD msg, DWORD dwU, DWORD dwP1, DWORD dwP2) {
	return MMDriverCallback(dwC, dwF, drv, msg, dwU, dwP1, dwP2);
}

MMRESULT WINMM_midiOutGetErrorTextA(MMRESULT err, LPSTR pszT, UINT cchT) {
	return MMmidiOutGetErrorTextA(err, pszT, cchT);
}

MMRESULT WINMM_midiOutGetErrorTextW(MMRESULT err, LPWSTR pszT, UINT cchT) {
	return MMmidiOutGetErrorTextW(err, pszT, cchT);
}

MMRESULT WINMM_midiConnect(HMIDI hS, HMIDIIN hM, LPVOID lpV) {
	return MMmidiConnect(hS, hM, lpV);
}

MMRESULT WINMM_midiDisconnect(HMIDI hS, HMIDIIN hM, LPVOID lpV) {
	return MMmidiDisconnect(hS, hM, lpV);
}

MMRESULT WINMM_midiInAddBuffer(HMIDIIN hM, LPMIDIHDR buf, UINT bufsize) {
	return MMmidiInAddBuffer(hM, buf, bufsize);
}

MMRESULT WINMM_midiInPrepareHeader(HMIDIIN hM, LPMIDIHDR buf, UINT bufsize) {
	return MMmidiInPrepareHeader(hM, buf, bufsize);
}

MMRESULT WINMM_midiInUnprepareHeader(HMIDIIN hM, LPMIDIHDR buf, UINT bufsize) {
	return MMmidiInPrepareHeader(hM, buf, bufsize);
}

MMRESULT WINMM_midiInOpen(LPHMIDIIN lphM, UINT uDID, DWORD_PTR dwC, DWORD_PTR dwCI, DWORD dwF) {
	return MMmidiInOpen(lphM, uDID, dwC, dwCI, dwF);
}

MMRESULT WINMM_midiInStart(HMIDIIN hM) {
	return MMmidiInStart(hM);
}

MMRESULT WINMM_midiInReset(HMIDIIN hM) {
	return MMmidiInReset(hM);
}

MMRESULT WINMM_midiInStop(HMIDIIN hM) {
	return MMmidiInStop(hM);
}

MMRESULT WINMM_midiInClose(HMIDIIN hM) {
	return MMmidiInClose(hM);
}

MMRESULT WINMM_midiInGetID(HMIDIIN hM, LPUINT lpU) {
	return MMmidiInGetID(hM, lpU);
}

MMRESULT WINMM_midiInGetDevCapsA(UINT_PTR uP, LPMIDIINCAPSA LPMIC, UINT u) {
	return MMmidiInGetDevCapsA(uP, LPMIC, u);
}

MMRESULT WINMM_midiInGetDevCapsW(UINT_PTR uP, LPMIDIINCAPSA LPMIC, UINT u) {
	return MMmidiInGetDevCapsW(uP, LPMIC, u);
}

MMRESULT WINMM_midiInGetErrorTextA(MMRESULT mmr, LPSTR str, UINT u) {
	return MMmidiInGetErrorTextA(mmr, str, u);
}

MMRESULT WINMM_midiInGetErrorTextW(MMRESULT mmr, LPSTR str, UINT u) {
	return MMmidiInGetErrorTextW(mmr, str, u);
}

MMRESULT WINMM_midiInMessage(HMIDIIN hM, UINT u, DWORD_PTR dwP1, DWORD_PTR dwP2) {
	return MMmidiInMessage(hM, u, dwP1, dwP2);
}

MMRESULT WINMM_midiInGetNumDevs() {
	return MMmidiInGetNumDevs();
}

MMRESULT WINMM_joyConfigChanged(DWORD dwF) {
	return MMjoyConfigChanged(dwF);
}

MMRESULT WINMM_joyGetDevCapsA(UINT uDID, LPJOYCAPSA LPJC, UINT size) {
	return MMjoyGetDevCapsA(uDID, LPJC, size);
}

MMRESULT WINMM_joyGetDevCapsW(UINT uDID, LPJOYCAPSW LPJC, UINT size) {
	return MMjoyGetDevCapsW(uDID, LPJC, size);
}

MMRESULT WINMM_joyGetPos(UINT uDID, LPJOYINFO LPJI) {
	return MMjoyGetPos(uDID, LPJI);
}

MMRESULT WINMM_joyGetPosEx(UINT uDID, LPJOYINFOEX LPJI) {
	return MMjoyGetPosEx(uDID, LPJI);
}

MMRESULT WINMM_joyGetThreshold(UINT uDID, LPUINT val) {
	return MMjoyGetThreshold(uDID, val);
}

MMRESULT WINMM_joySetThreshold(UINT uDID, UINT val) {
	return MMjoySetThreshold(uDID, val);
}

MMRESULT WINMM_joyReleaseCapture(UINT uDID) {
	return MMjoyReleaseCapture(uDID);
}

MMRESULT WINMM_joySetCapture(HWND h, UINT uDID, UINT uP, BOOL fC) {
	return MMjoySetCapture(h, uDID, uP, fC);
}

UINT WINMM_joyGetNumDevs() {
	return MMjoyGetNumDevs();
}

BOOL WINMM_mciDriverNotify(HWND hC, UINT wDID, UINT uS) {
	return MMmciDriverNotify(hC, wDID, uS);
}

BOOL WINMM_mciExecute(LPCSTR pC) {
	return MMmciExecute(pC);
}

UINT WINMM_mciLoadCommandResource(HANDLE hI, LPCWSTR lpRN, UINT wT) {
	return MMmciLoadCommandResource(hI, lpRN, wT);
}

BOOL WINMM_mciFreeCommandResource(UINT wT) {
	return MMmciFreeCommandResource(wT);
}

BOOL WINMM_mciGetErrorStringA(DWORD MCIE, LPTSTR pT, UINT cT) {
	return MMmciGetErrorStringA(MCIE, pT, cT);
}

BOOL WINMM_mciGetErrorStringW(DWORD MCIE, LPWSTR pT, UINT cT) {
	return MMmciGetErrorStringW(MCIE, pT, cT);
}

BOOL WINMM_mciSetDriverData(UINT wDID, DWORD dwD) {
	return MMmciSetDriverData(wDID, dwD);
}

BOOL WINMM_mciGetDriverData(UINT wDID) {
	return MMmciGetDriverData(wDID);
}

HANDLE WINMM_mciGetCreatorTask(MCIDEVICEID IDD) {
	return MMmciGetCreatorTask(IDD);
}

MCIDEVICEID WINMM_mciGetDeviceIDA(LPCTSTR pszD) {
	return MMmciGetDeviceIDA(pszD);
}

MCIDEVICEID WINMM_mciGetDeviceIDW(LPCWSTR pszD) {
	return MMmciGetDeviceIDW(pszD);
}

MCIDEVICEID WINMM_mciGetDeviceIDFromElementIDA(DWORD dwEID, LPCTSTR pszD) {
	return MMmciGetDeviceIDFromElementIDA(dwEID, pszD);
}

MCIDEVICEID WINMM_mciGetDeviceIDFromElementIDW(DWORD dwEID, LPCWSTR pszD) {
	return MMmciGetDeviceIDFromElementIDW(dwEID, pszD);
}

MCIERROR WINMM_mciSendCommandA(MCIDEVICEID uDID, UINT msg, DWORD_PTR dwP1, DWORD_PTR dwP2) {
	return MMmciSendCommandA(uDID, msg, dwP1, dwP2);
}

MCIERROR WINMM_mciSendCommandW(MCIDEVICEID uDID, UINT msg, DWORD_PTR dwP1, DWORD_PTR dwP2) {
	return MMmciSendCommandW(uDID, msg, dwP1, dwP2);
}

MCIERROR WINMM_mciSendStringA(LPCTSTR lpszC, LPTSTR lpszR, UINT cchR, HANDLE hC) {
	return MMmciSendStringA(lpszC, lpszR, cchR, hC);
}

MCIERROR WINMM_mciSendStringW(LPCWSTR lpszC, LPWSTR lpszR, UINT cchR, HANDLE hC) {
	return MMmciSendStringW(lpszC, lpszR, cchR, hC);
}

UINT WINMM_mciDriverYield(MCIDEVICEID wDID) {
	return MMmciDriverYield(wDID);
}

UINT WINMM_mciSetYieldProc(MCIDEVICEID wDID, YIELDPROC fpYP, DWORD dwYD) {
	return MMmciSetYieldProc(wDID, fpYP, dwYD);
}

YIELDPROC WINMM_mciGetYieldProc(MCIDEVICEID wDID, DWORD lpdwYD) {
	return MMmciGetYieldProc(wDID, lpdwYD);
}

DWORD WINMM_mixerMessage(HMIXER dID, UINT msg, DWORD_PTR dwP1, DWORD_PTR dwP2) {
	return MMmixerMessage(dID, msg, dwP1, dwP2);
}

MMRESULT WINMM_mixerClose(HMIXER dID) {
	return MMmixerClose(dID);
}

MMRESULT WINMM_mixerGetLineInfoA(HMIXEROBJ dIDO, LPMIXERLINEA LPML, DWORD size) {
	return MMmixerGetLineInfoA(dIDO, LPML, size);
}

MMRESULT WINMM_mixerGetLineInfoW(HMIXEROBJ dIDO, LPMIXERLINEW LPML, DWORD size) {
	return MMmixerGetLineInfoW(dIDO, LPML, size);
}

MMRESULT WINMM_mixerGetLineControlsA(HMIXEROBJ dIDO, LPMIXERLINECONTROLSA LPMLC, DWORD size) {
	return MMmixerGetLineControlsA(dIDO, LPMLC, size);
}

MMRESULT WINMM_mixerGetLineControlsW(HMIXEROBJ dIDO, LPMIXERLINECONTROLSW LPMLC, DWORD size) {
	return MMmixerGetLineControlsW(dIDO, LPMLC, size);
}

MMRESULT WINMM_mixerSetControlDetails(HMIXEROBJ dIDO, LPMIXERCONTROLDETAILS LPMCD, DWORD size) {
	return MMmixerSetControlDetails(dIDO, LPMCD, size);
}

MMRESULT WINMM_mixerGetControlDetailsA(HMIXEROBJ dIDO, LPMIXERCONTROLDETAILS LPMCD, DWORD size) {
	return MMmixerGetControlDetailsA(dIDO, LPMCD, size);
}

MMRESULT WINMM_mixerGetControlDetailsW(HMIXEROBJ dIDO, LPMIXERCONTROLDETAILS LPMCD, DWORD size) {
	return MMmixerGetControlDetailsW(dIDO, LPMCD, size);
}

MMRESULT WINMM_mixerGetDevCapsA(UINT_PTR uDID, LPMIXERCAPSA LPMC, UINT size) {
	return MMmixerGetDevCapsA(uDID, LPMC, size);
}

MMRESULT WINMM_mixerGetDevCapsW(UINT_PTR uDID, LPMIXERCAPSW LPMC, UINT size) {
	return MMmixerGetDevCapsW(uDID, LPMC, size);
}

MMRESULT WINMM_mixerGetID(HMIXEROBJ dIDO, UINT FAR * puMxId, DWORD fdwld) {
	return MMmixerGetID(dIDO, puMxId, fdwld);
}

MMRESULT WINMM_mixerOpen(LPHMIXER lpdID, UINT uMxId, DWORD_PTR dwC, DWORD_PTR dwI, DWORD fdwO) {
	return MMmixerOpen(lpdID, uMxId, dwC, dwI, fdwO);
}

UINT WINMM_mixerGetNumDevs() {
	return MMmixerGetNumDevs();
}

FOURCC WINMM_mmioStringToFOURCCA(LPCTSTR sz, UINT f) {
	return MMmmioStringToFOURCCA(sz, f);
}

FOURCC WINMM_mmioStringToFOURCCW(LPCWSTR sz, UINT f) {
	return MMmmioStringToFOURCCW(sz, f);
}

LONG WINMM_mmioRead(HMMIO hm, HPSTR pch, LONG cch) {
	return MMmmioRead(hm, pch, cch);
}

LONG WINMM_mmioSeek(HMMIO hm, LONG lO, INT iO) {
	return MMmmioSeek(hm, lO, iO);
}

LONG WINMM_mmioWrite(HMMIO hm, char _huge * pch, LONG cch) {
	return MMmmioWrite(hm, pch, cch);
}

LPMMIOPROC WINMM_mmioInstallIOProcA(FOURCC fccIOP, LPMMIOPROC pIOP, DWORD dwF) {
	return MMmmioInstallIOProcA(fccIOP, pIOP, dwF);
}

LPMMIOPROC WINMM_mmioInstallIOProcW(FOURCC fccIOP, LPMMIOPROC pIOP, DWORD dwF) {
	return MMmmioInstallIOProcW(fccIOP, pIOP, dwF);
}

LRESULT WINMM_mmioSendMessage(HMMIO hm, UINT msg, LPARAM lP1, LPARAM lP2) {
	return MMmmioSendMessage(hm, msg, lP1, lP2);
}

MMRESULT WINMM_mmioAdvance(HMMIO hm, LPMMIOINFO pmmioi, UINT fuA) {
	return MMmmioAdvance(hm, pmmioi, fuA);
}

MMRESULT WINMM_mmioAscend(HMMIO hm, LPMMCKINFO pmmcki, UINT fuA) {
	return MMmmioAscend(hm, pmmcki, fuA);
}

MMRESULT WINMM_mmioClose(HMMIO hm, UINT fuC) {
	return MMmmioClose(hm, fuC);
}

MMRESULT WINMM_mmioCreateChunk(HMMIO hm, LPMMCKINFO pmmcki, UINT fuC) {
	return MMmmioCreateChunk(hm, pmmcki, fuC);
}

MMRESULT WINMM_mmioDescend(HMMIO hm, LPMMCKINFO pmmcki, const MMCKINFO * pmmckiP, UINT fuD) {
	return MMmmioDescend(hm, pmmcki, pmmckiP, fuD);
}

MMRESULT WINMM_mmioFlush(HMMIO hm, UINT fuF) {
	return MMmmioFlush(hm, fuF);
}

MMRESULT WINMM_mmioGetInfo(HMMIO hm, LPMMIOINFO pmmioi, UINT fuI) {
	return MMmmioGetInfo(hm, pmmioi, fuI);
}

MMRESULT WINMM_mmioSetInfo(HMMIO hm, const LPMMIOINFO pmmioi, UINT fuI) {
	return MMmmioSetInfo(hm, pmmioi, fuI);
}

MMRESULT WINMM_mmioOpenA(LPTSTR pszFN, LPMMIOINFO pmmioi, DWORD fdwO) {
	return MMmmioOpenA(pszFN, pmmioi, fdwO);
}

MMRESULT WINMM_mmioOpenW(LPWSTR pszFN, LPMMIOINFO pmmioi, DWORD fdwO) {
	return MMmmioOpenW(pszFN, pmmioi, fdwO);
}

MMRESULT WINMM_mmioRenameA(LPCTSTR pszFN, LPCTSTR pszNFN, const LPMMIOINFO pmmioi, DWORD fdwR) {
	return MMmmioRenameA(pszFN, pszNFN, pmmioi, fdwR);
}

MMRESULT WINMM_mmioRenameW(LPCWSTR pszFN, LPCWSTR pszNFN, const LPMMIOINFO pmmioi, DWORD fdwR) {
	return MMmmioRenameW(pszFN, pszNFN, pmmioi, fdwR);
}

MMRESULT WINMM_mmioSetBuffer(HMMIO hm, LPSTR pchB, LONG cchB, UINT fuB) {
	return MMmmioSetBuffer(hm, pchB, cchB, fuB);
}

BOOL WINMM_mmTaskSignal(DWORD undef) {
	return MMmmTaskSignal(undef);
}

UINT WINMM_mmTaskCreate(LPTASKCALLBACK undef1, HANDLE undef2, DWORD_PTR undef3) {
	return MMmmTaskCreate(undef1, undef2, undef3);
}

VOID WINMM_mmTaskBlock(DWORD undef) {
	return MMmmTaskBlock(undef);
}

VOID WINMM_mmTaskYield() {
	MMmmTaskYield();
}

BOOL WINMM_PlaySound(LPCTSTR pszS, HMODULE hmod, DWORD fdwS) {
	return MMPlaySoundA(pszS, hmod, fdwS);
}

BOOL WINMM_PlaySoundA(LPCTSTR pszS, HMODULE hmod, DWORD fdwS) {
	return WINMM_PlaySound(pszS, hmod, fdwS);
}

BOOL WINMM_PlaySoundW(LPCWSTR pszS, HMODULE hmod, DWORD fdwS) {
	return MMPlaySoundW(pszS, hmod, fdwS);
}

BOOL WINMM_sndPlaySoundA(LPCTSTR pszS, DWORD fuS) {
	return MMsndPlaySoundA(pszS, fuS);
}

BOOL WINMM_sndPlaySoundW(LPCWSTR pszS, DWORD fuS) {
	return MMsndPlaySoundW(pszS, fuS);
}

DWORD WINMM_auxOutMessage(UINT uDID, UINT msg, DWORD_PTR dwP1, DWORD_PTR dwP2) {
	return MMauxOutMessage(uDID, msg, dwP1, dwP2);
}

MMRESULT WINMM_auxGetDevCapsA(UINT_PTR uDID, LPAUXCAPSA pac, UINT cbac) {
	return MMauxGetDevCapsA(uDID, pac, cbac);
}

MMRESULT WINMM_auxGetDevCapsW(UINT_PTR uDID, LPAUXCAPSW pac, UINT cbac) {
	return MMauxGetDevCapsW(uDID, pac, cbac);
}

MMRESULT WINMM_auxGetVolume(UINT uDID, LPDWORD lpV) {
	return MMauxGetVolume(uDID, lpV);
}

MMRESULT WINMM_auxSetVolume(UINT uDID, DWORD V) {
	return MMauxSetVolume(uDID, V);
}

UINT WINMM_auxGetNumDevs() {
	return MMauxGetNumDevs();
}

DWORD WINMM_waveOutMessage(HWAVEOUT hw, UINT msg, DWORD_PTR dwP1, DWORD_PTR dwP2) {
	return MMwaveOutMessage(hw, msg, dwP1, dwP2);
}

MMRESULT WINMM_waveOutAddBuffer(HWAVEOUT hw, LPWAVEHDR pch, UINT cch) {
	return MMwaveOutAddBuffer(hw, pch, cch);
}

MMRESULT WINMM_waveOutBreakLoop(HWAVEOUT hw) {
	return MMwaveOutBreakLoop(hw);
}

MMRESULT WINMM_waveOutClose(HWAVEOUT hw) {
	return MMwaveOutClose(hw);
}

MMRESULT WINMM_waveOutGetDevCapsA(UINT_PTR uDID, LPWAVEOUTCAPSA LPWOT, UINT size) {
	return MMwaveOutGetDevCapsA(uDID, LPWOT, size);
}

MMRESULT WINMM_waveOutGetDevCapsW(UINT_PTR uDID, LPWAVEOUTCAPSW LPWOT, UINT size) {
	return MMwaveOutGetDevCapsW(uDID, LPWOT, size);
}

MMRESULT WINMM_waveOutGetErrorTextA(MMRESULT err, LPTSTR pszT, UINT cchT) {
	return MMwaveOutGetErrorTextA(err, pszT, cchT);
}

MMRESULT WINMM_waveOutGetErrorTextW(MMRESULT err, LPWSTR pszT, UINT cchT) {
	return MMwaveOutGetErrorTextW(err, pszT, cchT);
}

MMRESULT WINMM_waveOutGetID(HWAVEOUT hw, LPUINT puDID) {
	return MMwaveOutGetID(hw, puDID);
}

MMRESULT WINMM_waveOutSetPitch(HWAVEOUT hw, DWORD P) {
	return MMwaveOutSetPitch(hw, P);
}

MMRESULT WINMM_waveOutGetPitch(HWAVEOUT hw, LPDWORD pP) {
	return MMwaveOutGetPitch(hw, pP);
}

MMRESULT WINMM_waveOutSetPlaybackRate(HWAVEOUT hw, DWORD PR) {
	return MMwaveOutSetPlaybackRate(hw, PR);
}

MMRESULT WINMM_waveOutGetPlaybackRate(HWAVEOUT hw, LPDWORD pPR) {
	return MMwaveOutGetPlaybackRate(hw, pPR);
}

MMRESULT WINMM_waveOutGetPosition(HWAVEOUT hw, LPMMTIME pmmt, UINT cbmmt) {
	return MMwaveOutGetPosition(hw, pmmt, cbmmt);
}

MMRESULT WINMM_waveOutSetVolume(HWAVEOUT hw, DWORD V) {
	return MMwaveOutSetVolume(hw, V);
}

MMRESULT WINMM_waveOutGetVolume(HWAVEOUT hw, LPDWORD pV) {
	return MMwaveOutGetVolume(hw, pV);
}

MMRESULT WINMM_waveOutOpen(LPHWAVEOUT lphw, UINT uDID, LPCWAVEFORMATEX LPWFEX, DWORD_PTR dwC, DWORD_PTR dwI, DWORD fdwO) {
	return MMwaveOutOpen(lphw, uDID, LPWFEX, dwC, dwI, fdwO);
}

MMRESULT WINMM_waveOutPrepareHeader(HWAVEOUT hw, LPWAVEHDR pchB, UINT cchB) {
	return MMwaveOutPrepareHeader(hw, pchB, cchB);
}

MMRESULT WINMM_waveOutUnprepareHeader(HWAVEOUT hw, LPWAVEHDR pchB, UINT cchB) {
	return MMwaveOutUnprepareHeader(hw, pchB, cchB);
}

MMRESULT WINMM_waveOutWrite(HWAVEOUT hw, LPWAVEHDR pchB, UINT cchB) {
	return MMwaveOutWrite(hw, pchB, cchB);
}

MMRESULT WINMM_waveOutReset(HWAVEOUT hw) {
	return MMwaveOutReset(hw);
}

MMRESULT WINMM_waveOutRestart(HWAVEOUT hw) {
	return MMwaveOutRestart(hw);
}

MMRESULT WINMM_waveOutPause(HWAVEOUT hw) {
	return MMwaveOutPause(hw);
}

MMRESULT WINMM_waveOutStart(HWAVEOUT hw) {
	return MMwaveOutStart(hw);
}

MMRESULT WINMM_waveOutStop(HWAVEOUT hw) {
	return MMwaveOutStop(hw);
}

UINT WINMM_waveOutGetNumDevs() {
	return MMwaveOutGetNumDevs();
}

DWORD WINMM_waveInMessage(HWAVEIN hw, UINT msg, DWORD_PTR dwP1, DWORD_PTR dwP2) {
	return MMwaveInMessage(hw, msg, dwP1, dwP2);
}

MMRESULT WINMM_waveInAddBuffer(HWAVEIN hw, LPWAVEHDR pch, UINT cch) {
	return MMwaveInAddBuffer(hw, pch, cch);
}

MMRESULT WINMM_waveInClose(HWAVEIN hw) {
	return MMwaveInClose(hw);
}

MMRESULT WINMM_waveInGetDevCapsA(UINT_PTR uDID, LPWAVEINCAPSA LPWOT, UINT size) {
	return MMwaveInGetDevCapsA(uDID, LPWOT, size);
}

MMRESULT WINMM_waveInGetDevCapsW(UINT_PTR uDID, LPWAVEINCAPSW LPWOT, UINT size) {
	return MMwaveInGetDevCapsW(uDID, LPWOT, size);
}

MMRESULT WINMM_waveInGetErrorTextA(MMRESULT err, LPTSTR pszT, UINT cchT) {
	return MMwaveInGetErrorTextA(err, pszT, cchT);
}

MMRESULT WINMM_waveInGetErrorTextW(MMRESULT err, LPWSTR pszT, UINT cchT) {
	return MMwaveInGetErrorTextW(err, pszT, cchT);
}

MMRESULT WINMM_waveInGetID(HWAVEIN hw, LPUINT puDID) {
	return MMwaveInGetID(hw, puDID);
}

MMRESULT WINMM_waveInGetPosition(HWAVEIN hw, LPMMTIME pmmt, UINT cbmmt) {
	return MMwaveInGetPosition(hw, pmmt, cbmmt);
}

MMRESULT WINMM_waveInOpen(LPHWAVEIN lphw, UINT uDID, LPCWAVEFORMATEX LPWFEX, DWORD_PTR dwC, DWORD_PTR dwI, DWORD fdwO) {
	return MMwaveInOpen(lphw, uDID, LPWFEX, dwC, dwI, fdwO);
}

MMRESULT WINMM_waveInPrepareHeader(HWAVEIN hw, LPWAVEHDR pchB, UINT cchB) {
	return MMwaveInPrepareHeader(hw, pchB, cchB);
}

MMRESULT WINMM_waveInUnprepareHeader(HWAVEIN hw, LPWAVEHDR pchB, UINT cchB) {
	return MMwaveInUnprepareHeader(hw, pchB, cchB);
}

MMRESULT WINMM_waveInReset(HWAVEIN hw) {
	return MMwaveInReset(hw);
}

MMRESULT WINMM_waveInStart(HWAVEIN hw) {
	return MMwaveInStart(hw);
}

MMRESULT WINMM_waveInStop(HWAVEIN hw) {
	return MMwaveInStop(hw);
}

UINT WINMM_waveInGetNumDevs() {
	return MMwaveInGetNumDevs();
}

MMRESULT WINMM_timeGetDevCaps(LPTIMECAPS ptc, UINT cbtc) {
	return MMtimeGetDevCaps(ptc, &cbtc);
}

MMRESULT WINMM_timeSetEvent(UINT uDelay, UINT uResolution, LPTIMECALLBACK lpTimeProc, DWORD_PTR dwUser, UINT fuEvent) {
	return MMtimeSetEvent(uDelay, uResolution, lpTimeProc, dwUser, fuEvent);
}

MMRESULT WINMM_timeKillEvent(UINT uTimerID) {
	return MMtimeKillEvent(uTimerID);
}

MMRESULT WINMM_timeGetSystemTime(LPMMTIME pmmt, UINT cbmmt) {
	return MMtimeGetSystemTime(pmmt, &cbmmt);
}

MMRESULT WINMM_timeBeginPeriod(UINT uPeriod) {
	return MMtimeBeginPeriod(uPeriod);
}

MMRESULT WINMM_timeEndPeriod(UINT uPeriod) {
	return MMtimeEndPeriod(uPeriod);
}