#ifndef FILEUTILS_HPP
#define FILEUTILS_HPP

#include <stdexcept>
#include <fstream>
#include <sstream>
#include <string>

namespace gl_utils {

    inline std::string readFile(const std::string &name) {
        std::filebuf fb;
        std::stringstream ss;
        if (!fb.open(name, std::ios::in)) {
            ss << "Open file \"" << name << "\" for read failed\n";
            throw std::runtime_error(ss.str());
        }
        ss << &fb; //read the file
        return ss.str();
    }
}

#endif /* FILEUTILS_HPP */

