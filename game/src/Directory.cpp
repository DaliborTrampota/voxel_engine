#include "Directory.h"

#ifdef _WIN32
#include <shlobj.h>
#include <windows.h>

#elif __APPLE__
#include <limits.h>
#include <mach-o/dyld.h>

#else
#include <limits.h>
#include <unistd.h>

#endif

namespace fs = std::filesystem;

namespace {

    fs::path s_executableDirectory;
    fs::path s_assetsDirectory;
    fs::path s_userDataDirectory;


    fs::path _getUserDataDirectoryBase() {
#if defined(_WIN32)
        PWSTR path = nullptr;
        SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &path);
        fs::path result(path);
        CoTaskMemFree(path);
        return result;

#elif defined(__APPLE__)
        return fs::path(std::getenv("HOME")) / "Library/Application Support";

#else
        if (const char* xdg = std::getenv("XDG_DATA_HOME"))
            return fs::path(xdg);
        return fs::path(std::getenv("HOME")) / ".local/share";
#endif
    }

    fs::path _getExecutableDirectory(const char* argv0) {
#if defined(_WIN32)
        char buffer[MAX_PATH];
        GetModuleFileNameA(nullptr, buffer, MAX_PATH);
        return fs::path(buffer).parent_path();

#elif defined(__APPLE__)
        char buffer[PATH_MAX];
        uint32_t size = sizeof(buffer);
        _NSGetExecutablePath(buffer, &size);
        return fs::path(buffer).parent_path();

#else
        char buffer[PATH_MAX];
        ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
        if (len != -1) {
            buffer[len] = '\0';
            return fs::path(buffer).parent_path();
        }
        return fs::absolute(argv0).parent_path();
#endif
    }
}  // namespace


void initDirectories(const char* argv0) {
    s_executableDirectory = _getExecutableDirectory(argv0);
    s_assetsDirectory = s_executableDirectory / "resources";
    s_userDataDirectory = _getUserDataDirectoryBase() / "MyGame";


    fs::create_directories(s_assetsDirectory);
    fs::create_directories(s_userDataDirectory);
}

fs::path getExecutableDirectory() {
    return s_executableDirectory;
}

fs::path getAssetsDirectory() {
    return s_assetsDirectory;
}

fs::path getUserDataDirectory() {
    return s_userDataDirectory;
}