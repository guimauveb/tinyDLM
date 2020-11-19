#include "../include/controller.hxx"

dlManagerController::dlManagerController()
{
    dlCounter = 0;
}

dlManagerController::~dlManagerController() {}

/* Initialize a new dlManager object then place it in a vector */
std::string dlManagerController::createNewDl(std::string folder, std::string filename, const std::string url,  
        const int lowSpeedLim, const int lowSpeedTim)
{
    std::string finalFilename = filename;
    std::map<std::string, int>::iterator itA = downloadsMap.find(filename);

    /* If filename exists create a duplicate according to the records for this filename. Returns the proper
     * duplicate filename */ 
    if (itA != downloadsMap.end()) {
        finalFilename = recordDuplicate(filename);
    }
    /* If filename is not present create new record and keep the original filename */
    else {
        createNewRecord(filename);
    }

    /* If the duplicate filename generated is also existing, it means that the user manually appended (n) to a file */
    /* Then leave the download using the same filename untouched and return without doing anything */
    std::map<std::string, int>::iterator itB = downloadsMap.find(finalFilename);
    if (itB == downloadsMap.end()) {
        downloadsMap.insert(itB, std::pair<std::string, int>(finalFilename, dlCounter));

        const std::string saveAs = dlFolder + finalFilename;
        std::unique_ptr<dlManager> dlm = std::make_unique<dlManager>(dlCounter, url, finalFilename, saveAs, lowSpeedLim, lowSpeedTim);
        dlManagerVec.emplace_back(std::move(dlm));

        dlCounter++;

        assert(dlManagerVec.size() == downloadsMap.size());
    }
    else {
        finalFilename = "NULL";
    }

    return finalFilename;
}

/* Process the filename and find out what index must be appended according to records for this filename */
std::string& dlManagerController::recordDuplicate(std::string& f) {
    std::string origFilename = f;
    /* Duplicate number */
    int n = 1;
    std::map<std::string, std::vector<int>>::iterator it = filenamesRecords.find(f);
    bool found = false;

    /* We suppose that the array minimum possible value is 1 */
    std::sort(filenamesRecords[f].begin(), filenamesRecords[f].end());
    /* If filename already has duplicates, append (n) to it where n is equal to the index of duplicates */
    if (it != filenamesRecords.end()) {
        /* If vector size == 0, duplicate value = 1 */
        if (filenamesRecords[f].size() == 0) {
            found = true;
            n = 1;
        }
        /* If the first value is not 1 we can conclude 1 is the minimum missing value */
        else if (filenamesRecords[f].at(0) != 1) {
            n = 1;
            found = true;
        }
        else {

            for (int i = 0; i < filenamesRecords[f].size(); ++i) {
                /* Check if next element exists */
                if (i + 1 < filenamesRecords[f].size()) {
                    int j = i + 1;
                    /* If the difference between the next element and the current element is not one, then there 
                     * is a gap meaning that the min missing value is equal to current element + 1 */
                    if (filenamesRecords[f].at(j) - filenamesRecords[f].at(i) != 1) {
                        n = filenamesRecords[f].at(i) + 1;
                        found = true;
                        break;
                    } 
                }
            }
            if (!found) {
                /* If we could not find a gap, then n is equal to the largest element + 1 */
                n = filenamesRecords[f].at(filenamesRecords[f].size() - 1) + 1;
            }
        }

    }
    /* If not duplicates was found, n = 1 */
    /* Append duplicate value to filename vector of duplicates values */
    filenamesRecords[f].push_back(n);
    createDuplicate(f, n);
    /* Store original filename and duplicate value in dlRecs and use final filename as key */
    dlRecs.insert(dlRecs.end(), std::pair<std::string, dlRecord>(f, {origFilename, n}));

    return f;
}

void dlManagerController::createNewRecord(std::string& f)
{
    filenamesRecords.insert(filenamesRecords.end(), std::pair<std::string, std::vector<int>>(f, std::vector<int>{}));
}

/* Start one dl */
void dlManagerController::startDl(const std::string& dlToStart)
{
    try {
        dlManagerVec.at(downloadsMap[dlToStart])->runThread();
    }
    catch (const std::out_of_range& oor) {
        // TODO - write to log 
    } 
}

/* TODO - Start multiple downloads at the same time */
void dlManagerController::startDl(const std::vector<std::string>& dlsToStart)
{
}

void dlManagerController::resume(const std::string& dlToResume)
{
    dlManagerVec.at(downloadsMap[dlToResume])->resume();
}

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

void dlManagerController::clearInactive()
{
    assert(dlManagerVec.size() == downloadsMap.size());
    std::map<std::string, int>::iterator it;
    std::vector<size_t> todel;
    for (size_t i = 0; i < dlManagerVec.size(); ++i) {
        if((dlManagerVec.at(i)->getDownloadInfos()->status == downloadStatus::ERROR) || 
                (dlManagerVec.at(i)->getDownloadInfos()->status == downloadStatus::COMPLETED)) {
            it = downloadsMap.find(dlManagerVec.at(i)->getDownloadInfos()->filename);
            if (it != downloadsMap.end()) {
                /* Decrement downloads ids above deleted item since they are used as 
                 * indexes to access dlManagerVec */
                /* TODO - could we start iterating by ref at some index instead of map::begin ? */
                /* TODO - resizedownloadsmap ? */
                for (auto& el : downloadsMap) {
                    if (el.second > it->second) {
                        --el.second;
                    }
                }
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

void dlManagerController::stop(const std::string& dlToStop)
{
    dlManagerVec.at(downloadsMap[dlToStop])->pause();
    /* downloadsMap item returns an int corresponding to the index of the item in the vec */
    dlManagerVec.erase(dlManagerVec.begin() + downloadsMap[dlToStop]);

    std::map<std::string, int>::iterator it = downloadsMap.find(dlToStop);
    if (it != downloadsMap.end()) {
        /* Decrement downloads ids above deleted item since they are used as 
         * indexes to access dlManagerVec */
        for (auto & el : downloadsMap) {
            if (el.second > it->second) {
                --el.second;
            }
        }   
        downloadsMap.erase(it);
    }
    std::map<std::string, dlRecord>::iterator itB = dlRecs.find(dlToStop);
    /* If dl to stop is actually a duplicate */
    if (itB != dlRecs.end()) {
        /* Remove filename's duplicate number from filenamesRecords */
        std::string orig = itB->second.origFilename;
        int d = itB->second.dupNum;
        filenamesRecords[orig].erase(std::remove(filenamesRecords[orig].begin(), filenamesRecords[orig].end(), d), filenamesRecords[orig].end());
    }

    /* Decrement counter */
    dlCounter--;
}

void dlManagerController::stopAll()
{
    for (size_t i = 0; i < dlManagerVec.size(); ++i)
        dlManagerVec.at(i)->pause();
}

/* Stops all ongoing transfers - clear downloads menu, downloads list and filenames records - reset download counter 
 * Sets activeDls to false. Does not delete already downloaded bytes from the hard drive */
void dlManagerController::killAll()
{
    stopAll();
    dlManagerVec.clear();
    downloadsMap.clear();
    filenamesRecords.clear();
    dlCounter = 0;
}

std::string dlManagerController::getURL(const std::string& filename)
{
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
    dlManagerVec.at(downloadsMap[filename])->downloadSpeed();
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
        std::string spd;
        /* Return ' - ' intead of a current speed that doesn't mean anything anymore */
        if (strstatus != statusStrDl) {
            spd = " - ";
        }
        else {
            spd = stringifyNumber(getSpeed(f), 2);
        }
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

