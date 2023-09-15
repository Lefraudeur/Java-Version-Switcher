#pragma once
#include <Windows.h>
#include <string>
namespace jvs
{
	DWORD getParentProcessId();

	class process
	{
	public:
		process(DWORD pid);
		~process();

		operator bool() const;
		bool _putenv(const std::string& env);
	private:
		DWORD processId = 0;
		HANDLE processHandle = nullptr;
	};
}