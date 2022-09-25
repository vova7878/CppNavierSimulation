#ifndef STRINGUTILS_HPP
#define STRINGUTILS_HPP

#include <string>
#include <sstream>

void write_ss(std::stringstream&) { }

template<typename T, typename... Tp>
void write_ss(std::stringstream &ss, T value, Tp... args) {
    ss << value;
    write_ss(ss, args...);
}

template<typename... Tp>
std::string toString(Tp... args) {
    std::stringstream ss;
    write_ss(ss, args...);
    return ss.str();
}

#endif /* STRINGUTILS_HPP */

