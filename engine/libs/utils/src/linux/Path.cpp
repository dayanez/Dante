/*
 * Copyright (C) 2018 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include <utils/Path.h>

#include <pwd.h>

#include <cstdint>

#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

#include <stdlib.h>

namespace utils {

bool Path::mkdir() const {
    return ::mkdir(m_path.c_str(), S_IRUSR | S_IWUSR | S_IXUSR) == 0;
}

Path Path::getCurrentExecutable() {
    // First, need to establish resource path.
    char exec_buf[2048];
    Path result;

    uint32_t buffer_size = sizeof(exec_buf)-1;
    ssize_t sz = readlink("/proc/self/exe", exec_buf, buffer_size);
    if (sz > 0) {
        exec_buf[sz] = 0;
        result.setPath(exec_buf);
    }

    return result;
}

Path Path::getTemporaryDirectory() {
    return Path("/tmp/");
}

Path Path::getUserSettingsDirectory() {
    const char* home = getenv("HOME");
    if (!home) {
        struct passwd* pwd = getpwuid(getuid());
        if (pwd) {
           home = pwd->pw_dir;
        }
    }
    return Path(home);
}

std::vector<Path> Path::listContents() const {
    // Return an empty vector if the path doesn't exist or is not a directory
    if (!isDirectory() || !exists()) {
        return {};
    }

    struct dirent* directory;
    DIR* dir;

    dir = opendir(c_str());
    if (dir == nullptr) {
        // Path does not exist or could not be read
        return {};
    }

    std::vector<Path> directory_contents;

    while ((directory = readdir(dir)) != nullptr) {
        const char* file = directory->d_name;
        if (file[0] != '.') {
            directory_contents.push_back(concat(directory->d_name));
        }
    }

    closedir(dir);
    return directory_contents;
}
} // namespace utils
