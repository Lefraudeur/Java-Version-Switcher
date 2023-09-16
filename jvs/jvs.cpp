#include "jvs.hpp"
#include "jvsutils.hpp"
#include <iostream>
#include "downloader/downloader.hpp"
#include "extractor/extractor.hpp"
#include <unordered_map>
#include <fstream>

std::filesystem::path jvs::getPathForVersion(const char* version)
{
	const nlohmann::json& json = getJson();
	if (json.empty())
	{
		return std::filesystem::path();
	}

	if (!json.contains(version))
	{
		std::cerr << "[Error] the specified version could not be found in the config" << '\n';
		return std::filesystem::path();
	}

	const nlohmann::json& version_json = json[version];
	try
	{
		return std::filesystem::path(version_json.get<std::string>());
	}
	catch (const nlohmann::json::exception& e)
	{
		std::cerr << "[Error] " << e.what() << '\n';
	}
	return std::filesystem::path();
}

bool jvs::addJavaPath(jvs::process& proc, const std::filesystem::path& path)
{
	std::string newPath = "PATH=" + path.string() + ";" + removeExistingJavaPath();
	if (!proc._putenv(newPath))
	{
		std::cerr << "[Error] Failed to set PATH env variable" << '\n';
		return false;
	}
	return true;
}

bool jvs::setJavaHome(jvs::process& proc, const std::filesystem::path& path)
{
	std::string JAVA_HOME = "JAVA_HOME=" + path.string();
	if (!proc._putenv(JAVA_HOME))
	{
		std::cerr << "[Error] Failed to set JAVA_HOME env variable" << '\n';
		return false;
	}
	return true;
}

bool jvs::handleDownload(std::string versionName)
{
	nlohmann::json json = jvs::getJson();
	if (json.empty())
	{
		return false;
	}

	std::string versionStr{};
	try
	{
		versionStr = json["install"][versionName].get<std::string>();
	}
	catch (...)
	{
		std::cerr << "[Error] No download url found for this version" << '\n';
		return false;
	}
	URL version(versionStr);

	std::filesystem::path exeDir = jvs::getExeDir();
	std::filesystem::path zipPath = exeDir.string() + "/" + versionName + ".zip";
	std::filesystem::path extractPath = exeDir.string() + "/" + versionName;
	if 
	(
		!version.download(zipPath)
		|| !jvs::ZIP(zipPath).extractToFolder(extractPath, true)
	)
		return false;

	std::filesystem::directory_iterator dir{ extractPath };
	bool shouldMove = false;
	std::filesystem::path to_move{};
	int fileCount = 0;
	for (; dir != std::filesystem::directory_iterator(); dir++)
	{
		if (dir->is_directory())
		{
			shouldMove = true;
			to_move = dir->path();
		}
		else
			shouldMove = false;
		fileCount++;
		if (fileCount > 1)
		{
			shouldMove = false;
			break;
		}
	}
	if (shouldMove)
	{
		try
		{
			for (const std::filesystem::path& path : std::filesystem::directory_iterator(to_move))
			{
				std::filesystem::rename(path, extractPath.string() + "/" + path.filename().string());
			}
			std::filesystem::remove(to_move);
		}
		catch (...)
		{
			std::cerr << "[Error] Failed to move extracted files" << '\n';
			return false;
		}
	}

	json[versionName] = extractPath;
	std::ofstream jsonFile(jvs::json_path);
	if (!jsonFile)
	{
		std::cerr << "[Error] Failed to write config file" << '\n';
		return false;
	}
	jsonFile << json.dump(4);
	jsonFile.close();
	std::cout << "[Success] Java Version Downloaded and added to config, you can now jvs " << versionName << '\n';
	return true;
}