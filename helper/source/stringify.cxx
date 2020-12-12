#include "../include/stringify.hxx"
#include "../../DownloaderCore/include/download.hxx"
#include "../../dialogs/include/dialogs.hxx"

#include <sstream>

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





