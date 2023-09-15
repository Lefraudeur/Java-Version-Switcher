#include "jvs.hpp"
#include "jvsutils.hpp"
#include <iostream>
#include "downloader/downloader.hpp"

std::filesystem::path jvs::getPathForVersion(const char* version)
{
	const nlohmann::json& json = getJson();
	if (json.empty())
	{
		return std::filesystem::path();
	}

	if (!json.contains(version))
	{
		std::cerr << "[Error] the specified version could not be found in the config" << std::endl;
		return std::filesystem::path();
	}

	const nlohmann::json& version_json = json[version];
	try
	{
		return std::filesystem::path(version_json.get<std::string>());
	}
	catch (const nlohmann::json::exception& e)
	{
		std::cerr << "[Error] " << e.what() << std::endl;
	}
	return std::filesystem::path();
}

bool jvs::addJavaPath(jvs::process& proc, const std::filesystem::path& path)
{
	std::string newPath = "PATH=" + path.string() + ";" + removeExistingJavaPath();
	if (!proc._putenv(newPath))
	{
		std::cerr << "[Error] Failed to set PATH env variable" << std::endl;
		return false;
	}
	return true;
}

bool jvs::setJavaHome(jvs::process& proc, const std::filesystem::path& path)
{
	std::string JAVA_HOME = "JAVA_HOME=" + path.string();
	if (!proc._putenv(JAVA_HOME))
	{
		std::cerr << "[Error] Failed to set JAVA_HOME env variable" << std::endl;
		return false;
	}
	return true;
}

bool jvs::handleDownload(std::string versionName)
{
	static std::unordered_map<std::string, URL> versions =
	{
		{"zulu17", URL("https://cdn.azul.com/zulu/bin/zulu17.44.53-ca-jdk17.0.8.1-win_x64.zip")}
	};

	try
	{
		return versions.at(versionName).download(jvs::getExeDir().string() + "/" + versionName + ".zip");
	}
	catch (...)
	{
		std::cerr << "[Error] No download url found for this version" << std::endl;
	}
	return false;
}