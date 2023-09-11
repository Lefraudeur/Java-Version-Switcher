#include <iostream>
#include "jvs/jvs.hpp"

int main(int size, char* args[])
{
	if (size != 2)
	{
		std::cerr << "Usage: jvs <javaVersion>" << std::endl;
		return -1;
	}
	const std::filesystem::path& path = jvs::getPathForVersion(args[1]);
	jvs::process proc(jvs::getParentProcessId());
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