#include "process.hpp"
#include <tlhelp32.h>
#include <iostream>


jvs::process::process(DWORD pid) :
	processId(pid)
{
	this->processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
}

jvs::process::~process()
{
	CloseHandle(processHandle);
}

jvs::process::operator bool() const
{
	return processHandle != nullptr;
}

HMODULE jvs::process::getModule(const std::wstring& moduleName) const
{
	HMODULE module = nullptr;
	HANDLE allProcessModules = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processId);
	MODULEENTRY32 moduleEntry{};
	moduleEntry.dwSize = sizeof(MODULEENTRY32);
	Module32First(allProcessModules, &moduleEntry);
	do
	{
		if (std::wstring(moduleEntry.szModule) != moduleName)
			continue;
		module = moduleEntry.hModule;
		break;
	} while (Module32Next(allProcessModules, &moduleEntry));
	CloseHandle(allProcessModules);
	return module;
}

bool jvs::process::_putenv(const std::string& env)
{
	HMODULE msvcrtdll = GetModuleHandleA("msvcrt.dll");
	if (!msvcrtdll)
	{
		std::cerr << "[Error] Failed to get msvcrt.dll" << std::endl;
		return false;
	}
	FARPROC _putenv_addr = GetProcAddress(msvcrtdll, "_putenv");
	if (!_putenv_addr)
	{
		std::cerr << "[Error] Failed to get _putenv address" << std::endl;
		return false;
	}
	LPVOID allocatedString = VirtualAllocEx(processHandle, nullptr, env.size() + 1, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE); //+1 for null terminator
	if (!allocatedString)
	{
		std::cerr << "[Error] Failed to allocate env in target process" << std::endl;
		return false;
	}
	if (!WriteProcessMemory(processHandle, allocatedString, env.c_str(), env.size() + 1, nullptr))
	{
		std::cerr << "[Error] Failed to write env in target process" << std::endl;
		return false;
	}
	HANDLE remoteThread = CreateRemoteThread(processHandle, nullptr, 0, (LPTHREAD_START_ROUTINE)_putenv_addr, allocatedString, 0, nullptr);
	if (!remoteThread)
	{
		std::cerr << "[Error] Failed to call _putenv in target process" << std::endl;
		return false;
	}
	while (WaitForSingleObject(remoteThread, 0) != WAIT_OBJECT_0) {};
	CloseHandle(remoteThread);
	if (!VirtualFreeEx(processHandle, allocatedString, 0, MEM_RELEASE))
	{
		std::cerr << "[Error] Failed to deallocate env in target process" << std::endl;
		return false;
	}
	return true;
}

DWORD jvs::getParentProcessId()
{
	DWORD parentProcessId = 0;
	DWORD currentPID = GetCurrentProcessId();
	HANDLE allProcesses = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 processEntry{};
	processEntry.dwSize = sizeof(PROCESSENTRY32);
	Process32First(allProcesses, &processEntry);
	do
	{
		if (processEntry.th32ProcessID != currentPID)
			continue;
		parentProcessId = processEntry.th32ParentProcessID;
		break;
	} while (Process32Next(allProcesses, &processEntry));
	CloseHandle(allProcesses);
	return parentProcessId;
}