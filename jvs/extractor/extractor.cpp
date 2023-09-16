#include "extractor.hpp"
#include <iostream>
#include <indicators/progress_bar.hpp>
#include <indicators/cursor_control.hpp>
#include <fstream>

jvs::ZIP::ZIP(const std::filesystem::path& a_path) :
	path(a_path)
{
}

bool jvs::ZIP::extractToFolder(const std::filesystem::path& dest_path, bool deleteArchive)
{
	mz_zip_archive archive{};
	if (!mz_zip_reader_init_file(&archive, path.string().c_str(), 0))
	{
		std::cerr << "[Error] Failed to open zip file " << path.string().c_str() << '\n';
		return false;
	}

	using namespace indicators;
	ProgressBar bars
	{
	  option::BarWidth{50},
	  option::Start{"\rExtracting Zip ["},
	  option::Fill{"="},
	  option::Lead{">"},
	  option::ShowPercentage(true),
	  option::ShowElapsedTime(true),
	  option::ForegroundColor(Color::yellow),
	  option::Remainder{" "},
	  option::End{"]"}
	};

	show_console_cursor(false);
	mz_uint fileNumber = mz_zip_reader_get_num_files(&archive);
	bool isSuccess = true;
	for (mz_uint i = 0U; i < fileNumber; ++i)
	{
		if (!mz_zip_reader_is_file_supported(&archive, i))
		{
			std::cerr << "[Error] Unsupported archive format " << path.string() << '\n';
			isSuccess = false;
			break;
		}
		char fileName[FILENAME_MAX + 1] = { 0 };
		mz_zip_reader_get_filename(&archive, i, fileName, FILENAME_MAX + 1);
		std::filesystem::path extract_path = dest_path.string() + (dest_path.has_filename() ? "/" : "") + fileName;
		if (std::filesystem::exists(extract_path))
			continue;
		if (mz_zip_reader_is_file_a_directory(&archive, i))
		{
			try
			{
				std::filesystem::create_directories(extract_path);
			}
			catch (...)
			{
				std::cerr << "[Error] Failed to create directory " << extract_path.string() << '\n';
				isSuccess = false;
				break;
			}
			continue;
		}
		if (!mz_zip_reader_extract_to_file(&archive, i, extract_path.string().c_str(), 0))
		{
			std::cerr << "[Error] Failed to extract file to " << extract_path.string() << '\n';
			isSuccess = false;
			break;
		}
		bars.set_progress(size_t((float(i + 1U)/float(fileNumber)) * 100.0f));
	}
	mz_zip_reader_end(&archive);
	show_console_cursor(true);
	if (deleteArchive)
	{
		try
		{
			std::filesystem::remove_all(path);
		}
		catch (...)
		{
			std::cerr << "[Error] Failed to delete archive " << path.string() << '\n';
		}
	}
	return isSuccess;
}
