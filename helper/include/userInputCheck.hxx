#include "../../DownloaderCore/include/download.hxx"
#include "../../dialogs/include/dialogs.hxx"

#include <sstream>
#include <cstring>

int checkURLlength(const std::string& url);
int checkURL(std::string& url);
int checkHttp(std::string& url);
int checkHttps(std::string& url);
int checkFilename(std::string& filename);
std::string& createDuplicate(std::string& str, const int& i);
std::string& trimSpaces(std::string& str);

