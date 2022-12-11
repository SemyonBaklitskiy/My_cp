#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <algorithm>
#include "functions.h"

static long unsigned int get_size(FILE* stream);
static char* get_buffer(const char* name, long unsigned int* sizeOfFile);
static bool file_is_exists(const char* name);
static int get_weight(const std::string str);
static bool comparator(const std::string str1, const std::string str2);
static void copy_with_flags(const char* srcPath, const char* copyPath, const char* buffer, const int sizeOfBuffer, const std::vector<std::string> flags);

static long unsigned int get_size(FILE* stream) {
    struct stat buf = {};
    fstat(stream->_fileno, &buf);

    return buf.st_size / sizeof(char);
}

static char* get_buffer(const char* name, long unsigned int* sizeOfFile) {
    FILE* file = fopen(name, "r");

    if (file == NULL) {
        printf("Cannot open file with path: %s\n", name);
        return NULL;
    }

    const long unsigned int size = get_size(file);
    *sizeOfFile = size;

    char* buffer = (char*)calloc(size + 1, sizeof(char));

    if (buffer == NULL) {
        printf("Calloc returned NULL\n");
        return NULL;
    } 

    fread(buffer, sizeof(char), size, file);
    buffer[size] = '\0';

    fclose(file);

    return buffer;
}

static bool file_is_exists(const char* name) {
    FILE* file = fopen(name, "r");

    if (file == NULL) {
        return false;

    } else {
        fclose(file);
        return true;
    }
}

int copy(std::vector<std::string> flags, const std::vector<std::string> filePathes) {
    const char* srcPath = NULL;
    const char* copyPath = NULL;

    if (file_is_exists(filePathes[0].c_str())) {
        srcPath = filePathes[0].c_str();
        copyPath = filePathes[1].c_str();

    } else if (file_is_exists(filePathes[1].c_str())) {
        srcPath = filePathes[1].c_str();
        copyPath = filePathes[0].c_str();

    } else {
        std::cout << "Both files daoes not exist" << std::endl;
        return -1;
    }

    long unsigned int sizeOfBuffer = 0;
    char* buffer = get_buffer(srcPath, &sizeOfBuffer);
    
    if (buffer == NULL) {
        std::cout << "Can not create buffer" << std::endl;
        return -1;
    }

    if (flags.size() == 0) {
        FILE* copyFile = fopen(copyPath, "w");
        fwrite(buffer, sizeof(char), sizeOfBuffer, copyFile);
        fclose(copyFile);

    } else {
        std::sort(flags.begin(), flags.end(), comparator);
        copy_with_flags(srcPath, copyPath, buffer, sizeOfBuffer, flags);
    }

    free(buffer);
    return 0;
}

bool is_flag(const std::string str) {
    const int size = possibleFlags.size();
    for (int i = 0; i < size; ++i) {
        if (str == possibleFlags[i])
            return true;

        continue;
    }

    return false;
}


int input(std::vector<std::string>& flags, std::vector<std::string>& filePathes, const int argc, char* argv[]) {
    int amountOfFiles = 0;

    for (int argIndex = 1; argIndex <= argc - 1; ++argIndex) {
        if (is_flag(argv[argIndex])) {
            flags.push_back(argv[argIndex]);

        } else {
            ++amountOfFiles;
            filePathes.push_back(argv[argIndex]);
        }

        if (amountOfFiles > 2) {
            std::cout << "Too many file pathes or wrong flags were given" << std::endl;
            return -1;
        }
    }

    return 0;
}

static bool comparator(const std::string str1, const std::string str2) {
    int strWeight1 = get_weight(str1);
    int strWeight2 = get_weight(str2);

    return (strWeight1 - strWeight2) >= 0;
}

static void copy_with_flags(const char* srcPath, const char* copyPath, const char* buffer, const int sizeOfBuffer, const std::vector<std::string> flags) {
    bool hFlag = false;
    bool iFlag = false;
    bool fFlag = false;
    bool vFlag = false;
    bool pFlag = false;

    const int size = flags.size();

    for (int flagIndex = 0; flagIndex < size; ++flagIndex) {
        if (((flags[flagIndex] == "-h") || (flags[flagIndex] == "--help")) && !hFlag) {
            std::cout << "This program copies file1 to file2. Open man cp in terminal to get more info" << std::endl;
            hFlag = true;

        } else if (((flags[flagIndex] == "-i") || (flags[flagIndex] == "--interactive")) && !iFlag) {
            if (file_is_exists(copyPath)) {
                printf("Are you sure (Y/N)? ");

                char answer = '\0';
                std::cin >> answer;

                if ((answer == 'N') || (answer == 'n')) {
                    return;

                } else if ((answer != 'N') && (answer != 'Y') && (answer != 'n') && (answer != 'y')) {
                    std::cout << "Given wrong answer try again" << std::endl;
                    --flagIndex;
                    continue;
                }
            }
            
            FILE* copyFile = fopen(copyPath, "w");
            fwrite(buffer, sizeof(char), sizeOfBuffer, copyFile);
            fclose(copyFile);
            
            iFlag = true;

        } else if (((flags[flagIndex] == "-f") || (flags[flagIndex] == "--force")) && !fFlag) {
            FILE* copyFile = fopen(copyPath, "w");
            int error = errno;

            if (error == EACCES) {
                fclose(copyFile);
                remove(copyPath);

                copyFile = fopen(copyPath, "w");
            }

            fwrite(buffer, sizeof(char), sizeOfBuffer, copyFile);
            fFlag = true;

            fclose(copyFile);

        } else if (((flags[flagIndex] == "-v") || (flags[flagIndex] == "--verbose")) && !vFlag) {
            FILE* copyFile = fopen(copyPath, "w");

            fwrite(buffer, sizeof(char), sizeOfBuffer, copyFile);
            printf("\"%s\" > \"%s\"\n", srcPath, copyPath);
            vFlag = true;

            fclose(copyFile);

        } else if (((flags[flagIndex] == "-p") || (flags[flagIndex] == "--preserve")) && !pFlag) {
            struct stat info = {};
            FILE* srcFile = fopen(srcPath, "r");
            fstat(srcFile -> _fileno, &info);
            fclose(srcFile);

            FILE* copyFile = fopen(copyPath, "w");

            fwrite(buffer, sizeof(char), sizeOfBuffer, copyFile);
            fprintf(copyFile, "\n%d %ld %ld %d %d", info.st_mode, info.st_atim.tv_sec, info.st_mtim.tv_sec, info.st_uid, info.st_gid);
            pFlag = true;

            fclose(copyFile);
        } 
    }
}

static int get_weight(const std::string str) {
    if ((str == "-h") || (str == "--help")) {
        return 5;

    } else if ((str == "-i") || (str == "--interactive")) {
        return 4;

    } else if ((str == "-f") || (str == "--force")) {
        return 3;

    } else if ((str == "-v") || (str == "--verbose")) {
        return 2;

    } else if ((str == "-p") || (str == "--preserve")) {
        return 1;

    } else {
        return 0;
    }
}

