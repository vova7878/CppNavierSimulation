#include "FileUtils.hpp"
#include <stdio.h>
#include <stdexcept>

char* readFile(const char* name) {
    FILE *f = fopen(name, "rb");

    if (!f) {
        throw std::runtime_error("Unable to open file");
    }

    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *out = new char[fsize + 1];
    fread(out, fsize, 1, f);
    fclose(f);

    out[fsize] = 0;
    return out;
}
