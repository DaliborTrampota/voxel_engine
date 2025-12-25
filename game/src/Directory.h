#pragma once

#include <filesystem>

namespace fs = std::filesystem;


void initDirectories(const char* argv0);

fs::path getExecutableDirectory();
fs::path getAssetsDirectory();
fs::path getUserDataDirectory();
