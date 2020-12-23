#ifndef DOWNLOAD_HXX
#define DOWNLOAD_HXX

#include "../../Log/include/Log.hxx"
#include <string>
#include <vector>

/* A download has a status e.g downloading, pending etc */
enum class downloadStatus {PENDING, DOWNLOADING, PAUSED, STOPPED, COMPLETED, ERROR};

/* Using a struct to save download's info such as id, filename, url, speed, status and eta as displayed */ 
struct Download {
    int id;
    int can_resume;             /* Set if download can resume */
    int invalid;                /* Set if error encountered during download */
    int http_code;              /* http return code */
    int downloaded;

    double speed;
    double progress;

    downloadStatus status;          

    std::string filename;
    std::string saveAs; 
    std::string url;
    std::string eta;
    std::string unescaped;
    std::string referer;
    std::string cookie;

    /* Vector holding a pair of the number of bytes downloaded at one call of the progress functor and 
     * a timestamp of when the call was made. Used to compute the average current speed. */
    std::vector<std::pair<double, double>> duration_and_dl;
};

/* Infos to display on the status window */
struct DownloadWinInfo {
    const int id;
    const std::string status;
    const std::string speed;
    const std::string progress;
    const std::string eta;

    DownloadWinInfo(int i, const std::string s, const std::string sp, const std::string prog, 
            const std::string e)
        :id(i), status(s), speed(sp), progress(prog), eta(e)
    {}
};

/* Struct storing filenames infos for every download. Allows to properly update filenamesRecord */
/* E.g : archive(1)'s dlRecord = {origFilename: "archive", dupNum: 1} */
struct DownloadRecord {
    const std::string orig_filename;
    int dup_num;
};

#endif
