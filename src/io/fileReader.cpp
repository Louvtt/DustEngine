#include "dust/io/fileReader.hpp"
#include "dust/core/log.hpp"

#include <fstream>
#include <filesystem>
#include <system_error>

// from : https://insanecoding.blogspot.com/2011/11/how-to-read-in-file-in-c.html
std::string dust::io::getFileRawContent(const std::string& path)
{
    std::error_code error{};
    if(!std::filesystem::exists(path, error)) {
        DUST_ERROR("[File] {} doesn't exists (error {} : {})", path, error.value(), error.message());
        return "";
    }

    std::ifstream in(path, std::ios::in);
    in.exceptions(std::ios::badbit | std::ios::failbit);
    if (in)
    {
        std::string contents;
        in.seekg(0, std::ios::end);
        contents.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&contents[0], contents.size());
        in.close();
        return(contents);
    }
    throw(errno);
}
