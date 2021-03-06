#ifndef STRINGIFY_HXX
#include "../../DownloaderCore/include/download.hxx"
#include <string>

std::string stringifyNumber(const double val, const int n);
std::string stringifyETA();
std::string stringifyStatus(const downloadStatus& status);
#endif
