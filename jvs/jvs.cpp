#include "jvs.h"
#include <json.hpp>
#include <iostream>
#include <fstream>
#include <whereami/whereami.h>

namespace
{
	bool createJson();
	nlohmann::json getJson();
	std::string removeExistingJavaPath();
	std::filesystem::path getExeDir();

	const std::string json_path = getExeDir().string() + "/jvs.json";

	bool createJson()
	{
		std::cout << "[Warning] json config not found" << std::endl
			<< "[Warning] creating jvs.json ..." << std::endl
			<< "[Warning] edit the config file with your java paths" << std::endl;
		nlohmann::json json =
		{
			{"8", "C:/Program Files/Zulu/zulu-8"},
			{"17", "C:/Program Files/Zulu/zulu-17"}
		};
		std::ofstream config(json_path);
		if (!config)
		{
			return false;
		}
		config << json.dump(4);
		config.close();
		return true;
	}

	nlohmann::json getJson()
	{
		nlohmann::json json{};

		if 
		(
			!std::filesystem::exists(json_path)
			&& !createJson()
		)
		{
			std::cerr << "[Error] failed to create jvs.json" << std::endl;
			return json;
		}

		std::ifstream config(json_path);
		if (!config)
		{
			std::cerr << "[Error] failed to open jvs.json" << std::endl;
			return json;
		}

		try
		{
			 json = nlohmann::json::parse(config);
		}
		catch (const nlohmann::json::exception& e)
		{
			std::cerr << "[Error] failed to parse jvs.json" << std::endl
				<< "[Error] " << e.what() << std::endl;
		}
		config.close();
		return json;
	}

	std::string removeExistingJavaPath()
	{
		std::string path = std::getenv("PATH");

		std::vector<std::string> toRemove{};
		std::istringstream stream{};
		stream.str(path);
		for (std::string entry; std::getline(stream, entry, ';');)
		{
			if (entry.empty())
				continue;
			std::filesystem::path dir = entry;
			std::string lastDir = (dir.has_filename() ? dir.filename().string() : dir.parent_path().filename().string());
			if (lastDir != "bin" || !std::filesystem::is_directory(dir))
				continue;
			for (std::filesystem::directory_entry file : std::filesystem::directory_iterator(dir))
			{
				if (file.is_directory() || file.path().filename() != "java.exe")
					continue;
				toRemove.push_back(entry);
			}
		}

		for (const std::string& entry : toRemove)
		{
			size_t pos = path.find(entry);
			if (path[pos + entry.size()] == ';')
				path.erase(pos + entry.size(), 1);
			path.erase(pos, entry.size());
		}

		return path;
	}
	std::filesystem::path getExeDir()
	{
		int path_size = wai_getExecutablePath(nullptr, 0, nullptr);
		char* path_str = new char[path_size + 1] {0};
		wai_getExecutablePath(path_str, path_size, nullptr);
		path_str[path_size] = '\0';
		std::filesystem::path path(path_str);
		path = path.parent_path();
		delete[] path_str;
		return path;
	}
}

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

bool jvs::addJavaPath(const std::filesystem::path& path)
{
	std::string newPath = "PATH=" + path.string() + ";" + removeExistingJavaPath();
	if (putenv((char*)newPath.c_str()) != 0)
	{
		std::cerr << "[Error] Failed to set PATH env variable" << std::endl;
		return false;
	}
	return true;
}

bool jvs::setJavaHome(const std::filesystem::path& path)
{
	std::string JAVA_HOME = "JAVA_HOME=" + path.string();
	if (putenv((char*)JAVA_HOME.c_str()) != 0)
	{
		std::cerr << "[Error] Failed to set JAVA_HOME env variable" << std::endl;
		return false;
	}
	return true;
}
