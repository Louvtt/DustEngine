//
// Created by louvtt on 11/5/24.
//

#include "dust/utils/string_utils.hpp"

std::vector<std::string> dust::split_string(const std::string &str, char split)
{
    std::vector<std::string> res;

    std::string::size_type next = 0;
    std::string::size_type previous = 0;
    while((next = str.find_first_of(split, previous)) != std::string::npos) {
        res.push_back(str.substr(previous, next));
        previous = next;
    }

    return res;
}

