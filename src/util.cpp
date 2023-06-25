#include "util.hpp"

namespace yunying {
    std::vector<std::string> split(const std::string str, const std::string delimiter) {
        return split(str, delimiter, str.length());
    }

    std::vector<std::string> split(const std::string str, const std::string delimiter, int limit) {
        std::vector<std::string> result;
        std::string s = str;
        while (s.find(delimiter) != std::string::npos && --limit > 0) {
            result.push_back(s.substr(0, s.find(delimiter)));
            s = s.substr(s.find(delimiter) + delimiter.length());
        }
        result.push_back(s);
        return result;
    }
}