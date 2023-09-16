#include "jvsutils.hpp"
#include <iostream>
#include <fstream>
#include <Windows.h>

const std::string jvs::json_path = jvs::getExeDir().string() + "/jvs.json";

bool jvs::createJson()
{
	std::cout << "[Warning] json config not found" << '\n'
		<< "[Warning] creating jvs.json ..." << '\n'
		<< "[Warning] edit the config file with your java paths" << '\n';
	nlohmann::json json =
	{
		{"8", "C:/Program Files/Zulu/zulu-8"},
		{"17", "C:/Program Files/Zulu/zulu-17"},
		{"install", {
			{"zulu17", "https://cdn.azul.com/zulu/bin/zulu17.44.53-ca-jdk17.0.8.1-win_x64.zip"},
			{"zulu8", "https://cdn.azul.com/zulu/bin/zulu8.72.0.17-ca-jdk8.0.382-win_x64.zip"},
			{"zulu20", "https://cdn.azul.com/zulu/bin/zulu20.32.11-ca-jdk20.0.2-win_x64.zip"}
		}}
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

nlohmann::json jvs::getJson()
{
	nlohmann::json json{};

	if
		(
			!std::filesystem::exists(json_path)
			&& !createJson()
		)
	{
		std::cerr << "[Error] failed to create jvs.json" << '\n';
		return json;
	}

	std::ifstream config(json_path);
	if (!config)
	{
		std::cerr << "[Error] failed to open jvs.json" << '\n';
		return json;
	}

	try
	{
		json = nlohmann::json::parse(config);
	}
	catch (const nlohmann::json::exception& e)
	{
		std::cerr << "[Error] failed to parse jvs.json" << '\n'
			<< "[Error] " << e.what() << '\n';
	}
	config.close();
	return json;
}

std::string jvs::removeExistingJavaPath()
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

std::filesystem::path jvs::getExeDir()
{
	char fileName[FILENAME_MAX + 1] = { 0 };
	GetModuleFileNameA(nullptr, fileName, FILENAME_MAX + 1);
	for (char& c : fileName)
	{
		if (c == '\\')
			c = '/';
	}
	return std::filesystem::path(fileName).parent_path();
}