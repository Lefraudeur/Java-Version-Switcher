#pragma once
#include <filesystem>

namespace jvs
{
	std::filesystem::path getPathForVersion(const char* version);
	bool addJavaPath(const std::filesystem::path& path);
	bool setJavaHome(const std::filesystem::path& path);
}