#include "jvs.hpp"
#include "jvsutils.hpp"
#include <iostream>

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
