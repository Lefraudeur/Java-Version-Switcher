#pragma once
#include "process.hpp"
#include <filesystem>

namespace jvs
{
	std::filesystem::path getPathForVersion(const char* version);
	bool addJavaPath(jvs::process& proc, const std::filesystem::path& path);
	bool setJavaHome(jvs::process& proc, const std::filesystem::path& path);

	bool handleDownload(std::string versionName);
}