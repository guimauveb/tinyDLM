#ifndef STRINGIFY_HPP
#include "../../dlManager/include/download.hpp"
#include <string>

std::string stringifyNumber(const double val, const int n);
std::string stringifyETA();
std::string stringifyStatus(const downloadStatus& status);
#endif
