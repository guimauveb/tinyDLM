#include "../../helper/include/userInputCheck.hxx"

int checkURLlength(std::string& url)
{
    /* Check if the url is at least 7 char long (https://...) */
    if (url.length() < 8) { 
        return 1;
    }
    return 0;
}

int checkHttp(std::string& url)
{
    const std::string http  = "http://";
    if (url.substr(0, 7) != http)
        return 1;
    return 0;
}

int checkHttps(std::string& url)
{
    const std::string https = "https://";
    if (url.substr(0, 8) != https)
        return 1;
    return 0;
}

int checkURL(std::string& url)
{
    if (checkURLlength(url))
        return 1;
    if (checkHttp(url) && checkHttps(url))
        return 2;
    return 0;
}

int checkFilename(std::string& filename)
{
    return filename.length();
}

/* Append (n) to an already existing filename where n is equal to dlCounter */
std::string& createDuplicate(std::string& str, const int& i)
{
    const std::string append_num = "(" + std::to_string(i) + ")";
    std::size_t found = str.find_last_of(".");

    str.erase(str.find('\0'));
    /* If no .extension found - simply append (n) */
    if (found == std::string::npos) {
        str.append(append_num);
    }
    /* Append (n) to filename then append .extension back */
    else {
        std::string part1 = str.substr(0, found);
        std::string part2 = str.substr(found + 1);
        str = part1 + append_num;
        str.push_back('.');
        str.append(part2);
    }
    str.push_back('\0');
    return str;
}

std::string& trimSpaces(std::string& str)
{
    int i = 0, j = str.length() - 1;
    {
        /* Parse until '\0' or a alphanumeric char */
        while (str[i] && (!isalpha(str[i]) && !isdigit(str[i]))) {
            i++;
        }
        /* Move from the end till first char - using isspace() seems to cause trouble in the curses field 
         * but would definetly be better to use here */
        while(j > 0 && !isalpha(str[j]) && !isdigit(str[j]) && 
                (str[j] != '/') && (str[j] != ':') &&
                (str[j] != '-') && (str[j] != '_') &&
                (str[j] != '.') && (str[j] != '~') &&
                (str[j] != ')') && (str[j] != '(')) {
            j--;
        }
    } 
    return str = str.substr(i, j + 1);
}


