#include "downloader.hpp"
#include <indicators/progress_bar.hpp>
#include <indicators/cursor_control.hpp>
#include <fstream>
#include <unordered_map>

jvs::URL::URL(const std::string& a_url) :
	url(a_url)
{
}

bool jvs::URL::download(const std::filesystem::path& path)
{
	if (url.empty())
		return false;
	using namespace indicators;

	CURL* curl = curl_easy_init();
	if (!curl)
	{
		std::cerr << "[Error] Failed to init curl, this shouldn't happen" << std::endl;
	}
	std::ofstream file(path);
	if (!file)
	{
		std::cerr << "[Error] Failed to create file " << path.string() << std::endl;
		curl_easy_cleanup(curl);
		return false;
	}
	bool curlSuccess = false;

	show_console_cursor(false);
	ProgressBar bar
	{
	  option::BarWidth{50},
	  option::Start{"\rDownloading Zip [ "},
	  option::Fill{"="},
	  option::Lead{">"},
	  option::ShowPercentage(true),
	  option::ShowElapsedTime(true),
	  option::ForegroundColor(Color::yellow),
	  option::Remainder{" "},
	  option::End{" ]"}
	};

	if
	(
		errorCheck(curl_easy_setopt(curl, CURLOPT_URL, url.c_str()))
		&& errorCheck(curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &file_write_callback))
		&& errorCheck(curl_easy_setopt(curl, CURLOPT_WRITEDATA, &file))
		&& errorCheck(curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, &xfer_callback))
		&& errorCheck(curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0))
		&& errorCheck(curl_easy_setopt(curl, CURLOPT_XFERINFODATA, &bar))
	)
	{
		curlSuccess = errorCheck(curl_easy_perform(curl));
	}
	show_console_cursor(true);
	file.close();
	curl_easy_cleanup(curl);
	return curlSuccess;
}

size_t jvs::URL::file_write_callback(char* ptr, size_t size, size_t nmemb, void* userdata)
{
	size_t real_size = size * nmemb;
	if (!ptr || real_size < 1)
		return real_size;
	std::ofstream* file = (std::ofstream*)userdata;
	file->write(ptr, real_size);
	return real_size;
}

bool jvs::URL::errorCheck(CURLcode code)
{
	if (code == CURLE_OK)
	{
		return true;
	}
	else
	{
		std::cerr << "[Error] " << curl_easy_strerror(code) << std::endl;
		return false;
	}
}

int jvs::URL::xfer_callback(void* clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow)
{
	indicators::ProgressBar* block = (indicators::ProgressBar*)clientp;
	if (dltotal > 1.0) block->set_progress(((double)dlnow / (double)dltotal) * 100.0);
	return 0;
}
