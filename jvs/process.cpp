#include "process.hpp"
#include <tlhelp32.h>
#include <iostream>


jvs::process::process(DWORD pid) :
	processId(pid)
{
	this->processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
	if (!processHandle)
	{
		std::cerr << "[Error] Failed to open parent process" << std::endl;
	}
}

jvs::process::~process()
{
	if (processHandle) 
		CloseHandle(processHandle);
}

jvs::process::operator bool() const
{
	return processHandle != nullptr;
}

bool jvs::process::_putenv(const std::string& env)
{
	HMODULE msvcrtdll = GetModuleHandleA("msvcrt.dll");
	if (!msvcrtdll)
	{
		msvcrtdll = LoadLibraryA("msvcrt.dll");
		if (!msvcrtdll)
		{
			std::cerr << "[Error] Failed to load msvcrt.dll" << std::endl;
			return false;
		}
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
	DWORD exitCode = STILL_ACTIVE;
	while (exitCode == STILL_ACTIVE)
	{
		if (!GetExitCodeThread(remoteThread, &exitCode))
		{
			std::cerr << "[Error] Failed to get thread exit code" << std::endl;
			break;
		}
	}
	CloseHandle(remoteThread);
	if (!VirtualFreeEx(processHandle, allocatedString, 0, MEM_RELEASE))
	{
		std::cerr << "[Error] Failed to deallocate env in target process" << std::endl;
		return false;
	}
	if (exitCode)
	{
		std::cerr << "[Error] Failed _putenv in target process" << std::endl;
		return false;
	}
	return true;
}