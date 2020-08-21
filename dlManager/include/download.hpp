#ifndef DOWNLOAD_HPP
#define DOWNLOAD_HPP

#include <string>
#include <vector>

/* A download has a status e.g downloading, pending etc */
enum class downloadStatus {PENDING, DOWNLOADING, PAUSED, STOPPED, COMPLETED, ERROR};

/* Using a struct to save download's info such as id, filename, url, speed, status and eta as displayed */ 
struct download {
    int id;
    int lowSpeedLimit;              /* Minimum time at low_speed_limitd before aborting download */
    int lowSpeedTime;               /* Limit under which the transfer will abort after low_speed_time */
    int canResume;                  /* Set if download can resume */
    int invalid;                    /* Set if error encountered during download */
    int httpCode;                   /* http return code */
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
    std::vector<std::pair<double, double>> durationAndDl;
};

/* Infos to display on the status window */
struct downloadWinInfo {
    const int id;
    const std::string status;
    const std::string speed;
    const std::string progress;
    const std::string eta;
    
    downloadWinInfo(int i, const std::string s, const std::string sp, const std::string prog, 
        const std::string e)
        :id(i), status(s), speed(sp), progress(prog), eta(e)
    {}
};

#endif
