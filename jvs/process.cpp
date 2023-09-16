#include "process.hpp"
#include <tlhelp32.h>
#include <iostream>

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
	if (!parentProcessId)
	{
		std::cerr << "[Error] Failed to get parent PID" << '\n';
	}
	return parentProcessId;
}

jvs::process::process(DWORD pid) :
	processId(pid)
{
	this->processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
	if (!processHandle)
	{
		std::cerr << "[Error] Failed to open parent process" << '\n';
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
		std::cerr << "[Error] Failed to load msvcrt.dll" << '\n';
		return false;
	}
	FARPROC _putenv_addr = GetProcAddress(msvcrtdll, "_putenv");
	if (!_putenv_addr)
	{
		std::cerr << "[Error] Failed to get _putenv address" << '\n';
		return false;
	}
	LPVOID allocatedString = VirtualAllocEx(processHandle, nullptr, env.size() + 1, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE); //+1 for null terminator
	if (!allocatedString)
	{
		std::cerr << "[Error] Failed to allocate env in target process" << '\n';
		return false;
	}
	if (!WriteProcessMemory(processHandle, allocatedString, env.c_str(), env.size() + 1, nullptr))
	{
		std::cerr << "[Error] Failed to write env in target process" << '\n';
		return false;
	}
	HANDLE remoteThread = CreateRemoteThread(processHandle, nullptr, 0, (LPTHREAD_START_ROUTINE)_putenv_addr, allocatedString, 0, nullptr);
	if (!remoteThread)
	{
		std::cerr << "[Error] Failed to call _putenv in target process" << '\n';
		return false;
	}
	DWORD exitCode = STILL_ACTIVE;
	while (exitCode == STILL_ACTIVE)
	{
		if (!GetExitCodeThread(remoteThread, &exitCode))
		{
			std::cerr << "[Error] Failed to get thread exit code" << '\n';
			break;
		}
	}
	CloseHandle(remoteThread);
	if (!VirtualFreeEx(processHandle, allocatedString, 0, MEM_RELEASE))
	{
		std::cerr << "[Error] Failed to deallocate env in target process" << '\n';
		return false;
	}
	if (exitCode)
	{
		std::cerr << "[Error] Failed _putenv in target process" << '\n';
		return false;
	}
	return true;
}