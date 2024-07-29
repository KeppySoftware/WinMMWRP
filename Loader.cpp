#include "Loader.hpp"

#ifndef PURE_WRAPPER
bool OmniMIDI::Loader::LoadKDMAPIModule() {
	auto ptr = (LibImport*)OMLibImports;

	if (!KDMAPILib)
		KDMAPILib = new Lib(L"OmniMIDI", &ptr, OMLibImportsSize);

	if (KDMAPILib->LoadLib()) {
		return true;
	}

	return false;
}

bool OmniMIDI::Loader::FreeKDMAPIModule() {
	if (KDMAPILib->UnloadLib()) {
		delete KDMAPILib;
		KDMAPILib = nullptr;
		return true;
	}

	return false;
}

bool OmniMIDI::Loader::IsKDMAPILoaded() {
	return KDMAPILib->IsOnline();
}
#endif

bool OmniMIDI::Loader::LoadWinMMModule() {
	SysPath Utils;
	int swp = 0;
	wchar_t SysDir[MAX_PATH] = { 0 };

	auto ptr = (LibImport*)WMMBaseLibImps;
	auto bptr = (LibImport*)WinMMLibImports;

	if (Utils.GetFolderPath(FIDs::System, SysDir, sizeof(SysDir))) {
		if (WMMBaseLib == nullptr) {
			WMMBaseLib = new Lib(L"winmmbase", &ptr, WMMBaseLibImpSize);

			if (!WMMBaseLib->LoadLib(SysDir)) {
				delete WMMBaseLib;
				WMMBaseLib = new Lib(L"winmm", &ptr, WMMBaseLibImpSize);

				if (WMMBaseLib->LoadLib(SysDir)) {
					return true;
				}
				else return false;
			}		
		}

		if (WinMMLib == nullptr) {
			WinMMLib = new Lib(L"winmm", &bptr, WinMMLibImportsSize);

			if (WinMMLib->LoadLib(SysDir)) {
				return true;
			}
			else return false;
		}
	}

	return true;
}

bool OmniMIDI::Loader::FreeWinMMModule() {
	if (WinMMLib != nullptr)
	{
		if (WinMMLib->UnloadLib()) {
			delete WinMMLib;
			WinMMLib = nullptr;
		}
		else return false;
	}

	if (WMMBaseLib != nullptr) {
		if (WMMBaseLib->UnloadLib()) {
			delete WMMBaseLib;
			WMMBaseLib = nullptr;
		}
		else return false;
	}

	return true;
}