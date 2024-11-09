//
// Created by louvtt on 9/18/24.
//

#include "dust/io/resourceFile.hpp"

#include "dust/utils/string_utils.hpp"

#include <filesystem>
namespace fs = std::filesystem;

dust::io::ResourceFile::ResourceFile(const std::string &filepath)
: filepath(filepath), previousTimestamp(0) { }

void dust::io::ResourceFile::update() {
    return;
    if(filepath.find(':') != std::string::npos) {
        auto files = dust::split_string(filepath, ':');
        for(const auto f : files) {
            if (fs::exists(filepath)) {
                const auto last_write = fs::last_write_time(filepath).time_since_epoch().count();
                if (!hasBeenLoaded || last_write>previousTimestamp) {
                    reload(!hasBeenLoaded);
                    hasBeenLoaded = true;
                    previousTimestamp = last_write;
                }
            }
        }
    } else {
        if (fs::exists(filepath)) {
            const auto last_write = fs::last_write_time(filepath).time_since_epoch().count();
            if (!hasBeenLoaded || last_write>previousTimestamp) {
                reload(!hasBeenLoaded);
                hasBeenLoaded = true;
                previousTimestamp = last_write;
            }
        }
    }
}

std::string dust::io::ResourceFile::getFilepath() const {
    return filepath;
}

void dust::io::ResourceFile::reload(bool first_load) {}
