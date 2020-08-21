#include "../../include/helper/userInputCheck.hpp"

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


