/*

	OmniMIDI v15+ (Rewrite) for Windows NT

	This file contains the required code to run the driver under Windows 7 SP1 and later.

*/

#pragma once

#ifndef _UTILS_H
#define _UTILS_H

#include <Windows.h>
#include <guiddef.h>
#include <strsafe.h>
#include <cassert>

#include <ShlObj.h>

#define ImpMMFunc(f)		LibImport((void**)&MM##f, #f)
#define ImpFunc(f)			LibImport((void**)&f, #f)
#define loadLib				LoadLibraryA
#define loadLibW			LoadLibraryW
#define freeLib(x)			FreeLibrary((HMODULE)x)
#define getLib				GetModuleHandleA
#define getLibW				GetModuleHandleW
#define getAddr(x, y)		GetProcAddress((HMODULE)x, y)

namespace OmniMIDI {
	enum FIDs {
		System,
		UserFolder
	};

	class SysPath {
	public:
		bool GetFolderPath(const FIDs FolderID, wchar_t* String, size_t StringLen);
	};

	class LibImport
	{
	protected:
		void** funcptr = nullptr;
		const char* funcname = nullptr;

	public:
		LibImport(void** pptr, const char* pfuncname) {
			funcptr = pptr;
			*(funcptr) = nullptr;
			funcname = pfuncname;
		}

		~LibImport() {
			*(funcptr) = nullptr;
		}

		void* GetPtr() { return *(funcptr); }
		const char* GetName() { return funcname; }

		bool SetPtr(void* lib = nullptr, const char* ptrname = nullptr) {
			void* ptr = (void*)-1;

			if (lib == nullptr && ptrname == nullptr)
			{
				if (funcptr)
					*(funcptr) = nullptr;

				return true;
			}

			ptr = (void*)getAddr(lib, ptrname);

			if (!ptr)
				return false;

			if (ptr != *(funcptr))
				*(funcptr) = ptr;

			return true;
		}
	};

	class Lib {
	protected:
		const wchar_t* Name;
		void* Library = nullptr;
		bool Initialized = false;
		bool LoadFailed = false;
		bool AppSelfHosted = false;

		LibImport* Funcs = nullptr;
		size_t FuncsCount = 0;

	public:
		void* Ptr() { return Library; }
		bool IsOnline() { return (Library != nullptr && Initialized && !LoadFailed); }

		Lib(const wchar_t* pName, LibImport** pFuncs, size_t pFuncsCount) {
			Name = pName;
			Funcs = *pFuncs;
			FuncsCount = pFuncsCount;
		}

		~Lib() {
			UnloadLib();
		}

		bool LoadLib(wchar_t* CustomPath = nullptr) {
			OmniMIDI::SysPath Utils;

			char CName[MAX_PATH] = { 0 };
			wchar_t SysDir[MAX_PATH] = { 0 };
			wchar_t DLLPath[MAX_PATH] = { 0 };
			int swp = 0;

			if (Library == nullptr) {
				if (CustomPath != nullptr) {
					swp = swprintf(DLLPath, MAX_PATH, L"%s\\%s.dll\0", CustomPath, Name);
					assert(swp != -1);

					if (swp != -1) {
						wcstombs(CName, DLLPath, MAX_PATH);
						Library = loadLibW(DLLPath);

						if (!Library)
							return false;
					}
					else return false;
				}
				else {
					swp = swprintf(DLLPath, MAX_PATH, L"%s.dll\0", Name);
					assert(swp != -1);

					if (swp != -1) {
						Library = loadLibW(DLLPath);

						if (!Library)
						{
							if (Utils.GetFolderPath(FIDs::System, SysDir, sizeof(SysDir))) {
								swp = swprintf(DLLPath, MAX_PATH, L"%s\\OmniMIDI\\%s.dll\0", SysDir, Name);
								assert(swp != -1);
								if (swp != -1) {
									Library = loadLibW(DLLPath);
									assert(Library != 0);

									if (!Library) {
										wcstombs(CName, Name, MAX_PATH);
										return false;
									}

								}
								else return false;
							}
							else return false;
						}
					}
					else return false;

				}
			}

			for (size_t i = 0; i < FuncsCount; i++)
				Funcs[i].SetPtr(Library, Funcs[i].GetName());

			Initialized = true;
			return true;
		}

		bool UnloadLib() {
			if (Library != nullptr) {
				if (AppSelfHosted)
				{
					AppSelfHosted = false;
				}
				else {
					bool r = freeLib(Library);
					assert(r == true);
					if (!r) {
						throw;
					}
				}

				Library = nullptr;
			}

			LoadFailed = false;
			Initialized = false;
			return true;
		}
	};
}

#endif