#include "UIhelperFunc.hpp"

/* TODO - move somewhere else */
int checkURLlength(std::string& url)
{
    /* Check if the url is at least 7 char long (https://...) */
    if (url.length() < 7) { 
        return 1;
    }
    return 0;
}

int checkURL(std::string& url)
{
    if (checkURLlength(url))
        return 1;
    if (checkHttp(url))
        return 2;
    if (checkHttps(url))
        return 3;
    return 0;
}

int checkHttp(std::string& url)
{
    const std::string http  = "http://";
    /* Now that we checked for its length we can compare safely without risking a segfault */
    if (url.substr(0, 7) != http)
        return 1;
    return 0;
}

int checkHttps(std::string& url)
{
    const std::string https = "https://";
    /* http failed but let's check https */
    if (url.substr(0, 8) != https)
        return 1;
    return 0;
}

int checkFilename(std::string& filename)
{
    return filename.length();
}

std::string& trimSpaces(std::string& str)
{
    int i = 0, j = str.length() - 1;
    {
        /* Parse until '\0' or a alphanumeric char */
        while (str[i] && (!isalpha(str[i]) && !isdigit(str[i]))) {
            i++;
        }
        /* Move from the end till first char */
        while(!isalpha(str[j]) && !isdigit(str[j])) {
            j--;
        }
    } 
    return str = str.substr(i, j + 1);
}


std::string stringifyNumber(const double val, const int n)
{
    std::ostringstream out;
    out.precision(n);
    out << std::fixed << val;
    return out.str();
}

std::string stringifyETA()
{
    return "TODO";
}


std::string stringifyStatus(const downloadStatus& status)
{
    std::string statusStr;

    switch (status) {
        case downloadStatus::DOWNLOADING:
            return statusStrDl;
            break;
        case downloadStatus::PAUSED: 
            return statusStrPd;
            break;
        case downloadStatus::COMPLETED:
            return statusStrCd;
            break;
        case downloadStatus::ERROR:
            return statusStrEr;
            break;
        case downloadStatus::PENDING:
            return statusStrPd;
            break;
        default:
            return statusStrUn;
            break;
    }
}




