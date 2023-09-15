#include <json.hpp>

namespace jvs
{
	bool createJson();
	nlohmann::json getJson();
	std::string removeExistingJavaPath();
	std::filesystem::path getExeDir();
}