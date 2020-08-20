#include "dlManagerController.hpp"
#include <curses.h>

dlManagerController::dlManagerController()
    /* TODO - see what can be initialized here */
{
    dlCounter = 0;
}

dlManagerController::~dlManagerController()
{

}

/* Initialize a new dlManager object then place it in a vector */
std::string dlManagerController::createNewDl(std::string dlFolder, std::string filename, const std::string url,  
        const int lowSpeedLim, const int lowSpeedTim)
{
    /* TODO - use an index for each name for better readability */
    std::string f = filename;
    std::map<std::string, int>::iterator it = downloadsMap.find(filename);
    if (it != downloadsMap.end()) {
        f.erase(f.find('\0'));
        f.append(std::to_string(dlCounter));
        f.push_back('\0');
    }
    it = downloadsMap.end();
    downloadsMap.insert(it, std::pair<std::string, int>(f, dlCounter));

    std::string saveAs = dlFolder + f;
    std::unique_ptr<dlManager> dlm = std::make_unique<dlManager>(dlCounter, url, f, saveAs, lowSpeedLim, lowSpeedTim);
    dlManagerVec.emplace_back(std::move(dlm));

    dlCounter++;

    assert(dlManagerVec.size() == downloadsMap.size());

    return f;
}

/* Start one dl */
void dlManagerController::startDl(const std::string& dlToStart)
{
    try {
        dlManagerVec.at(downloadsMap[dlToStart])->runThread();
    }
    catch (const std::out_of_range& oor) {
        /* TODO - write to log */
    } 
}

/* Multiple downloads to start */
void dlManagerController::startDl(const std::vector<std::string>& dlsToStart)
{
    /* TODO */
}

void dlManagerController::resume(const std::string& dlToResume)
{
    dlManagerVec.at(downloadsMap[dlToResume])->resume();
}

/* Check status of every download and resume if status == PAUSED */ 
void dlManagerController::resumeAll()
{
    for (size_t i = 0; i < dlManagerVec.size(); ++i) {
            dlManagerVec.at(i)->resume();
    }
}

void dlManagerController::pause(const std::string& dlToPause)
{
    dlManagerVec.at(downloadsMap[dlToPause])->pause();
}

void dlManagerController::pauseAll()
{
    for (size_t i = 0; i < dlManagerVec.size(); ++i) {
        if (dlManagerVec.at(i)->getDownloadInfos()->status == downloadStatus::DOWNLOADING)
            dlManagerVec.at(i)->pause();
    }
}

/* TODO - see why a download in progress - close to being completed - is halfway removed */
/* Bug here */
void dlManagerController::clearInactive()
{
    /* TODO - How can it fail ?? */
    assert(dlManagerVec.size() == downloadsMap.size());

    std::vector<size_t> todel;
    for (size_t i = 0; i < dlManagerVec.size(); ++i) {
        if(dlManagerVec.at(i)->getDownloadInfos()->status != downloadStatus::DOWNLOADING) {
            std::map<std::string, int>::iterator it = downloadsMap.find(dlManagerVec.at(i)->
                    getDownloadInfos()->filename);
            if (it != downloadsMap.end()) {
                downloadsMap.erase(it);
                todel.push_back(i);
            }
        }
    }
    if (!todel.empty()) {
        for (size_t i = 0; i < todel.size(); ++i) {
            dlManagerVec.erase(dlManagerVec.begin() + (todel.at(i) - i));
            dlCounter--;
        }
    }
}

/* TODO - not UI related - move to helper func */
void dlManagerController::stop(const std::string& dlToStop)
{
    dlManagerVec.at(downloadsMap[dlToStop])->pause();
}

void dlManagerController::stopAll()
{
    for (size_t i = 0; i < dlManagerVec.size(); ++i)
        dlManagerVec.at(i)->pause();
}

/* Stops all ongoing transfers - clear the downloads menu and the downloads List - reset the download counter 
 * Sets activeDls to false. Does not delete already downloaded bytes from the hard drive */
void dlManagerController::killAll()
{
    stopAll();
    dlManagerVec.clear();
    downloadsMap.clear();
    dlCounter = 0;
}

std::string dlManagerController::getURL(const std::string& filename)
{
    /* TODO - clearInactive bug is here */
    return dlManagerVec.at(downloadsMap[filename])->getDownloadInfos()->url;
}

std::string dlManagerController::getETA(const std::string& filename)
{
    return dlManagerVec.at(downloadsMap[filename])->getDownloadInfos()->eta;
}

double dlManagerController::getProgress(const std::string& filename)
{
    return dlManagerVec.at(downloadsMap[filename])->getDownloadInfos()->progress;
}

double dlManagerController::getSpeed(const std::string& filename)
{
    /* TODO - check status */
    //if (status == downloadStatus::PAUSED || status == downloadStatus::COMPLETED || status == downloadStatus::ERROR) 
    /* TODO */
    return dlManagerVec.at(downloadsMap[filename])->getDownloadInfos()->speed;
}

downloadStatus dlManagerController::getStatus(const std::string& filename)
{
    return dlManagerVec.at(downloadsMap[filename])->getDownloadInfos()->status;
}

/* Get all downloads statuses infos */
std::vector<downloadWinInfo> dlManagerController::getAllDownloadsInfos() 
{
    std::vector<downloadWinInfo> vec;
    /* Loop over all downloads sorted by id */
    std::map<int, std::string> dlsSorted = sortDownloadsMapByIds();
    for (auto dl : dlsSorted) {
        const int id = dl.first;
        const std::string f = dl.second;
        const std::string strstatus = stringifyStatus(getStatus(f));
        const std::string spd = stringifyNumber(getSpeed(f), 2);
        const std::string prog = stringifyNumber(getProgress(f), 2);
        //std::string eta = stringifyETA(dlManagerControlgetETA(f));
        const std::string eta = "ETA";
        vec.emplace_back(id, strstatus, spd, prog, eta);
    }
    return vec;
}

/* Returns false if the map is empty */
bool dlManagerController::isActive()
{
    return downloadsMap.size() != 0 ? true : false;  
}

std::map<int, std::string> dlManagerController::sortDownloadsMapByIds()
{
    /* Start by sorting the downloads by id then flip the map and use the name of the downloads as the key
     * while keeping the map sorted by id */
    return flipMap(downloadsMap);
}

std::vector<std::string> dlManagerController::getDownloadsList()
{
    std::vector<std::string> vec;
    std::map<int, std::string> m = sortDownloadsMapByIds();
    for (auto el : m) {
        vec.emplace_back(el.second);
    }
    return vec;
}


