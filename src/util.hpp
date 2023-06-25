#ifndef YY_UTIL
#define YY_UTIL

#include <string>
#include <vector>

namespace yunying {
    // trim from start (in place)
    static inline void ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
            return !std::isspace(ch);
        }));
    }

    // trim from end (in place)
    static inline void rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
            return !std::isspace(ch);
        }).base(), s.end());
    }

    // trim from both ends (in place)
    static inline void trim(std::string &s) {
        rtrim(s);
        ltrim(s);
    }

    std::vector<std::string> split(const std::string str, const std::string delimiter);
    std::vector<std::string> split(const std::string str, const std::string delimiter, int limit);
} // namespace yunying

#endif