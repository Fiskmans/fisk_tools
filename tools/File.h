#ifndef FISK_TOOLS_FILE_H
#define FISK_TOOLS_FILE_H

#include <filesystem>
#include <optional>
#include <string>

namespace fisk::tools
{
	std::optional<std::string> ReadWholeFile(std::filesystem::path aFilePath);
}

#endif