#include <iostream>
#include <windows.h>
#include <tlhelp32.h>
#include "xorstr.h"
#include "lazy_importer.h"

using namespace std;

DWORD GetProcessIdByName(string name) {
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
		return 0;
	if (Process32First(hProcessSnap, &pe32)) {
		do {
			if (strcmp(pe32.szExeFile, name.c_str()) == 0) {
				CloseHandle(hProcessSnap);
				return pe32.th32ProcessID;
			}
		} while (Process32Next(hProcessSnap, &pe32));
	}
	CloseHandle(hProcessSnap);
	return 0;
}

typedef LONG(NTAPI* NtSuspendProcess)(IN HANDLE ProcessHandle);
void suspend(DWORD processId) {
	HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);

	NtSuspendProcess pfnNtSuspendProcess = (NtSuspendProcess)GetProcAddress(
		LI_FN(GetModuleHandleA).cached()(_("ntdll")), _("NtSuspendProcess"));

	pfnNtSuspendProcess(processHandle);
	CloseHandle(processHandle);
}

int main(char argc, char* argv[]) {
	try {
		suspend(GetProcessIdByName(argv[1]));
	}
	catch (exception e) {
		cout << e.what() << endl;
	}
}