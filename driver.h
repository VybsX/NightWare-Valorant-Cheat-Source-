#include "check.h"
#include "Func.h"
#include "xor.h"

uintptr_t virtualaddy;

#define OMGSEXreadwyy CTL_CODE(FILE_DEVICE_UNKNOWN, 0x312, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define OMGSEXbab3 CTL_CODE(FILE_DEVICE_UNKNOWN, 0X520, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define OMGSEXget_guarded_regionm12 CTL_CODE(FILE_DEVICE_UNKNOWN, 0x431, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define OMGSEXcode_security 0x85b3e12

typedef struct _SEXreadwrite {
	INT32 security;
	INT32 process_id;
	ULONGLONG address;
	ULONGLONG buffer;
	ULONGLONG size;
	BOOLEAN write;
} rw, * prw;


typedef struct _ba {
	INT32 security;
	INT32 process_id;
	ULONGLONG* address;
} ba, * pba;

typedef struct _ga {
	INT32 security;
	ULONGLONG* address;
} ga, * pga;

namespace mem {
	HANDLE driver_handle;
	INT32 process_id;

	bool find_driver() {
		driver_handle = CreateFileW(_(L"\\\\.\\\OMGNightWare"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

		if (!driver_handle || (driver_handle == INVALID_HANDLE_VALUE))
			return false;

		return true;
	}

	void SEXread_physical(PVOID address, PVOID buffer, DWORD size) {
		_SEXreadwrite arguments = { 0 };

		arguments.security = OMGSEXcode_security;
		arguments.address = (ULONGLONG)address;
		arguments.buffer = (ULONGLONG)buffer;
		arguments.size = size;
		arguments.process_id = process_id;
		arguments.write = FALSE;

		DeviceIoControl(driver_handle, OMGSEXreadwyy, &arguments, sizeof(arguments), nullptr, NULL, NULL, NULL);
	}

	void SEXwrite_physical(PVOID address, PVOID buffer, DWORD size) {
		_SEXreadwrite arguments = { 0 };

		arguments.security = OMGSEXcode_security;
		arguments.address = (ULONGLONG)address;
		arguments.buffer = (ULONGLONG)buffer;
		arguments.size = size;
		arguments.process_id = process_id;
		arguments.write = TRUE;

		DeviceIoControl(driver_handle, OMGSEXreadwyy, &arguments, sizeof(arguments), nullptr, NULL, NULL, NULL);
	}

	uintptr_t SEXfind_image() {
		uintptr_t image_address = { NULL };
		_ba arguments = { NULL };

		arguments.security = OMGSEXcode_security;
		arguments.process_id = process_id;
		arguments.address = (ULONGLONG*)&image_address;

		DeviceIoControl(driver_handle, OMGSEXbab3, &arguments, sizeof(arguments), nullptr, NULL, NULL, NULL);

		return image_address;
	}

	uintptr_t SEXget_guarded() {
		uintptr_t guarded_region_address = { NULL };
		_ga arguments = { NULL };

		arguments.security = OMGSEXcode_security;
		arguments.address = (ULONGLONG*)&guarded_region_address;

		DeviceIoControl(driver_handle, OMGSEXget_guarded_regionm12, &arguments, sizeof(arguments), nullptr, NULL, NULL, NULL);

		return guarded_region_address;
	}

	INT32 SEXfind_process(LPCTSTR process_name) {
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
		return process_id;
	}

}

template <typename T>
T SEXread2(uint64_t address) {
	T buffer{ };
	mem::SEXread_physical((PVOID)address, &buffer, sizeof(T));
	if (check::is_guarded(buffer))
	{
		buffer = check::validate_pointer(buffer);
	}

	return buffer;
}

template <typename T>
T SEXread(uint64_t address) {
	T buffer{ };
	mem::SEXread_physical((PVOID)address, &buffer, sizeof(T));

	return buffer;
}

template <typename T>
T SEXwrite(uint64_t address, T buffer) {

	mem::SEXwrite_physical((PVOID)address, &buffer, sizeof(T));
	return buffer;
}