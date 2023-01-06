#include "tools/File.h"

#include <fstream>
#include <vector>

namespace fisk::tools
{

	std::string ReadWholeFile(std::string aFilePath)
	{
		std::vector<char> buffer;

		std::ifstream file(aFilePath, std::ios::ate | std::ios::binary);
		size_t size = file.tellg();
		file.seekg(0, file.beg);
		
		buffer.reserve(size + 1);
		buffer.resize(size);
		file.read(buffer.data(), size);

		buffer.push_back('\0');
		return buffer.data();
}
}
