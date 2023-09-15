#include <iostream>
#include "jvs/jvs.hpp"

int main(int size, char* args[])
{
	if (size < 2)
	{
		std::cerr << "Usage: jvs <javaVersion>" << std::endl;
		return -1;
	}

	if (std::string(args[1]) == "install")
	{
		if (size != 3)
		{
			std::cerr << "Usage: jvs install <javaVersion>" << std::endl;
			return -1;
		}
		if (jvs::handleDownload(args[2]))
			return 0;
		return -1;
	}

	const std::filesystem::path& path = jvs::getPathForVersion(args[1]);
	DWORD pid = jvs::getParentProcessId();
	jvs::process proc(pid);
	if
	(
		path.empty()
		|| !proc
		|| !jvs::addJavaPath(proc, path.string() + (path.has_filename() ? "/bin" : "bin"))
		|| !jvs::setJavaHome(proc, path)
	) 
		return -1;
	std::cout << "[Success] Java version switched to " << args[1] << std::endl;
	return 0;
}