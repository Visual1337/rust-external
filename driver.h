#pragma once

#include <Windows.h>
#include <TlHelp32.h>
#include <cstdint>
#include <stdio.h>
#include <string_view>

uintptr_t image_base;

uintptr_t game_image_address;
uintptr_t UP_image_base;

#include <winternl.h>
#include <vector>
#include "crypt.h"

#define code_rw CTL_CODE(FILE_DEVICE_UNKNOWN, 0x8672564, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define code_ba CTL_CODE(FILE_DEVICE_UNKNOWN, 0x8672565, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define code_ba_module CTL_CODE(FILE_DEVICE_UNKNOWN, 0x8672567, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define code_control_register CTL_CODE(FILE_DEVICE_UNKNOWN, 0x8672566, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

#define code_security 0x72b6f3c
#define code_security1 0x1e9c5af
#define code_security2 0xf4a7d8b

typedef struct _rw {
	INT32 security;
	INT32 security1;
	INT32 security2;
	INT32 process_id;
	ULONGLONG address;
	ULONGLONG buffer;
	ULONGLONG size;
	BOOLEAN write;
} rw, * prw;

typedef struct _cr3 {
	INT32 process_id;
} cr3, * pcr3;

typedef struct _ba {
	INT32 security;
	INT32 security1;
	INT32 security2;
	INT32 process_id;
	ULONGLONG* address;
} ba, * pba;

typedef struct _ba_module_ {
	ULONG     pid;
	const char* module_name;
	PVOID returned_base_address;
} ba_module, * pba_module;

uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName)
{
	uintptr_t modBaseAddr = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
	if (hSnap != INVALID_HANDLE_VALUE)
	{
		MODULEENTRY32 modEntry;
		modEntry.dwSize = sizeof(modEntry);
		if (Module32First(hSnap, &modEntry))
		{
			do
			{
				if (!_wcsicmp(modEntry.szModule, modName))
				{
					modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
					break;
				}
			} while (Module32Next(hSnap, &modEntry));
		}
	}
	CloseHandle(hSnap);
	return modBaseAddr;
}

namespace IO {

	HANDLE drv_handle;
	INT32 process_id;

	inline bool driver_init() {
		drv_handle = CreateFileW(SECURECRYPT(L"\\\\.\\\{e54b7e0b333703d4652c3da365490518}"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

		if (!drv_handle || (drv_handle == INVALID_HANDLE_VALUE))
			return false;

		return true;
	}

	inline bool read_PA(PVOID address, PVOID buffer, DWORD size) {
		OVERLAPPED gOverlapped;
		HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		gOverlapped.Offset = 0;
		gOverlapped.OffsetHigh = 0;
		gOverlapped.hEvent = hEvent;
		_rw arguments = { 0 };

		arguments.security = code_security;
		arguments.security1 = code_security1;
		arguments.security2 = code_security2;
		arguments.address = (ULONGLONG)address;
		arguments.buffer = (ULONGLONG)buffer;
		arguments.size = size;
		arguments.process_id = process_id;
		arguments.write = FALSE;

		if (DeviceIoControl(drv_handle, code_rw, &arguments, sizeof(arguments), nullptr, NULL, NULL,&gOverlapped))
			return true;

		unsigned long transfBytes;
		if (GetOverlappedResult(drv_handle, &gOverlapped, &transfBytes, false) == FALSE)
		{

		}

		ResetEvent(gOverlapped.hEvent);
		CloseHandle(hEvent);

		return false;
	}

	inline void write_PA(PVOID address, PVOID buffer, DWORD size) {
		_rw arguments = { 0 };

		arguments.security = code_security;
		arguments.security1 = code_security1;
		arguments.security2 = code_security2;
		arguments.address = (ULONGLONG)address;
		arguments.buffer = (ULONGLONG)buffer;
		arguments.size = size;
		arguments.process_id = process_id;
		arguments.write = TRUE;

		DeviceIoControl(drv_handle, code_rw, &arguments, sizeof(arguments), nullptr, NULL, NULL, NULL);
	}

	inline void get_cr3()
	{
		_cr3 arguments = { 0 };
		arguments.process_id = process_id;

		DeviceIoControl(drv_handle, code_control_register, &arguments, sizeof(arguments), nullptr, NULL, NULL, NULL);

		std::cout << ("[*] got cr3 from pid: ") << process_id << std::endl;
	}

	inline uintptr_t find_image() {
		uintptr_t image_address = { NULL };
		_ba arguments = { NULL };

		arguments.security = code_security;
		arguments.security1 = code_security1;
		arguments.security2 = code_security2;
		arguments.process_id = process_id;
		arguments.address = (ULONGLONG*)&image_address;
		DeviceIoControl(drv_handle, code_ba, &arguments, sizeof(arguments), nullptr, NULL, NULL, NULL);

		return image_address;
	}

	inline uintptr_t get_module(const char* moduleName) {
		PVOID module_address = 0;
		_ba_module_ arguments = { NULL };

		IN arguments.pid = process_id;
		IN arguments.module_name = moduleName;
		OUT arguments.returned_base_address = module_address;

		DeviceIoControl(drv_handle, code_ba_module, &arguments, sizeof(arguments), nullptr, NULL, NULL, NULL);

		return (uintptr_t)module_address;
	}

	INT32 find_process(LPCTSTR process_name) {
		PROCESSENTRY32 pt;
		HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		pt.dwSize = sizeof(PROCESSENTRY32);
		if (Process32First(hsnap, &pt)) {
			do {
				if (!lstrcmpi(pt.szExeFile, process_name)) {
					CloseHandle(hsnap);
					process_id = pt.th32ProcessID;
					return pt.th32ProcessID;
				}
			} while (Process32Next(hsnap, &pt));
		}
		CloseHandle(hsnap);

		return { NULL };
	}

	template <typename T>
	T read(uint64_t address) {
		T buffer{ };
		read_PA((PVOID)address, &buffer, sizeof(T));
		return buffer;
	}

	static std::wstring read_wstring(uintptr_t String_address, SIZE_T size)
	{
		const auto buffer = std::make_unique<wchar_t[]>(size);
		read_PA((PVOID)String_address, buffer.get(), size * 2);
		return std::wstring(buffer.get());
	}

	template <typename T>
	T chain_read(const std::uintptr_t address, std::vector<uintptr_t> chain)
	{
		uintptr_t cur_read = address;

		for (int i = 0; i < chain.size() - 1; ++i)
			cur_read = read<uintptr_t>(cur_read + chain[i]);

		return read<T>(cur_read + chain[chain.size() - 1]);
	}

	template <typename T>
	T write(uint64_t address, T buffer) {

		write_PA((PVOID)address, &buffer, sizeof(T));
		return buffer;
	}

	std::string read_wstr(uintptr_t address)
	{
		wchar_t buffer[1024 * sizeof(wchar_t)];
		read_PA((PVOID)address, &buffer, 1024 * sizeof(wchar_t));

		std::wstring new_buffer = std::wstring(buffer);

		return std::string(new_buffer.begin(), new_buffer.end());
	}

	std::string read_native(ULONG64 Address)
	{
		char buffer[1024] = { '\0' };
		read_PA((PVOID)read<ULONG64>(Address), buffer, 1024 - 1);
		return std::string(buffer);
	}

	std::string read_string(UINT_PTR String_address, SIZE_T size)
	{
		std::unique_ptr<char[]> buffer(new char[size]);
		read_PA((PVOID)String_address, buffer.get(), size);
		return std::string(buffer.get());
	}

	std::string read_string1(uintptr_t address)
	{
		wchar_t buffer[1024 * sizeof(wchar_t)];
		read_PA((PVOID)address, &buffer, 1024 * sizeof(wchar_t));

		std::wstring new_buffer = std::wstring(buffer);

		return std::string(new_buffer.begin(), new_buffer.end());
	}
}