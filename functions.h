#include <iostream>
#include <vector>

const std::vector<std::string> possibleFlags = { "-h", "--help", "-v", "--verbose", "-i", "--interactive", "-f", "--force", "-p", "--preserve"};

bool is_flag(const std::string str);
int input(std::vector<std::string>& flags, std::vector<std::string>& filePathes, const int argc, char* argv[]);
int copy(std::vector<std::string> flags, const std::vector<std::string> filePathes);