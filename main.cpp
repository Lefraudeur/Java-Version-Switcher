#include <iostream>
#include "jvs/jvs.h"

int main(int size, char* args[])
{
	if (size != 2)
	{
		std::cerr << "Usage: jvs <javaVersion>" << std::endl;
		return -1;
	}
	const std::filesystem::path& path = jvs::getPathForVersion(args[1]);
	if
	(
		path.empty()
		|| !jvs::addJavaPath(path.string() + (path.has_filename() ? "/bin" : "bin"))
		|| !jvs::setJavaHome(path)
	) 
		return -1;
	std::cout << "[Success] Java version switched to " << args[1] << std::endl;
	std::cout << "[java " << args[1] << "] " << std::filesystem::current_path().string() << ">";
	std::string command{};
	while (std::getline(std::cin, command))
	{
		std::system(command.c_str());
		std::cout << "[java " << args[1] << "] " << std::filesystem::current_path().string() << ">";
	}
	return 0;
}