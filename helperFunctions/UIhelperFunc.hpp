#include "../dlManager/download.hpp"
#include "../dialogs/dialogs.hpp"

#include <sstream>
#include <cstring>

int checkURLlength(const std::string& url);
int checkURL(std::string& url);
int checkHttp(std::string& url);
int checkHttps(std::string& url);
int checkFilename(std::string& filename);
std::string& trimSpaces(std::string& str);
std::string stringifyNumber(const double val, const int n);
std::string stringifyETA();
std::string stringifyStatus(const downloadStatus& status);
