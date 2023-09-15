#include <string>
#include <curl/curl.h>
#include <filesystem>
#include <iostream>

namespace jvs
{
	class URL
	{
	public:
		URL(const std::string& a_url);
		bool download(const std::filesystem::path& path);
	private:
		static size_t file_write_callback(char* ptr, size_t size, size_t nmemb, void* userdata);
		static bool errorCheck(CURLcode code);
		static int xfer_callback(void* clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow);
		std::string url;
	};
}