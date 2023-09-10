#include <json.hpp>

namespace jvs
{
	bool createJson();
	nlohmann::json getJson();
	std::string removeExistingJavaPath();
	std::filesystem::path getExeDir();

	static const std::string json_path = getExeDir().string() + "/jvs.json";
}