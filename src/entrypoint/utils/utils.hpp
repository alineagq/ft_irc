#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <cstdlib>
#include <sstream>
#include <vector>

void validateArgs(int argc, char* argv[]);
int validatePort(int port);
std::vector<std::string> split(const std::string &str, char delim);

#endif