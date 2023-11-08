#include "tools/File.h"

#include <fstream>
#include <vector>

namespace fisk::tools
{
	std::optional<std::string> ReadWholeFile(std::filesystem::path aFilePath)
	{
		std::vector<char> buffer;

		std::ifstream file(aFilePath, std::ios::ate | std::ios::binary);

		if (!file)
			return {};

		size_t size = file.tellg();
		file.seekg(0, file.beg);
		
		buffer.reserve(size + 1);
		buffer.resize(size);
		file.read(buffer.data(), size);

		buffer.push_back('\0');
		return buffer.data();
}
}
