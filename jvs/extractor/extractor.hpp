#pragma once
#include <miniz/miniz.h>
#include <filesystem>

namespace jvs
{
	class ZIP
	{
	public:
		ZIP(const std::filesystem::path& a_path);
		bool extractToFolder(const std::filesystem::path& dest_path, bool deleteArchive = false);
	private:
		std::filesystem::path path;
	};
}