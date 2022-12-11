#include <stdio.h>
#include <string.h>
#include "functions.h"

int main(int argc, char* argv[]) {
    std::vector<std::string> flags = {};
    std::vector<std::string> filePathes = {};

    if (input(flags, filePathes, argc, argv) == -1)
        return -1;

    return copy(flags, filePathes);
}