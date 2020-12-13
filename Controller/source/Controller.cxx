#include "../include/Controller.hxx"

Controller::Controller()
{
    dl_counter = 0;
}

Controller::~Controller() {}

/* Initialize a new DownloaderCore object then place it in a vector */
std::string Controller::createNewDl(std::string folder, std::string filename, const std::string url,  
                                    const int low_speed_limit, const int low_speed_time_limit)
{
    std::string final_filename = filename;
    std::map<std::string, int>::iterator ita = downloads_map.find(filename);

    /* If filename exists create a duplicate according to the records for this filename. Returns the proper
     * duplicate filename */ 
    if (ita != downloads_map.end()) {
        final_filename = recordDuplicate(filename);
    }
    /* If filename is not present create new record and keep the original filename */
    else {
        createNewRecord(filename);
    }

    /* If the duplicate filename generated is also existing, it means that the user manually appended (n) to a file */
    /* Then leave the download using the same filename untouched and return without doing anything */
    std::map<std::string, int>::iterator itb = downloads_map.find(final_filename);
    if (itb == downloads_map.end()) {
        downloads_map.insert(itb, std::pair<std::string, int>(final_filename, dl_counter));

        const std::string saveAs = downloads_folder + final_filename;
        std::unique_ptr<DownloaderCore> dlm = std::make_unique<DownloaderCore>(dl_counter, url, final_filename, 
                                                                               saveAs, low_speed_limit, 
                                                                               low_speed_time_limit);
        downloader_core_vec.emplace_back(std::move(dlm));

        dl_counter++;

        assert(downloader_core_vec.size() == downloads_map.size());
    }
    else {
        final_filename = "NULL";
    }

    return final_filename;
}

/* Process the filename and find out what index must be appended according to records for this filename */
std::string& Controller::recordDuplicate(std::string& f) 
{
    std::string original_filename = f;
    /* Duplicate number */
    int n = 1;
    std::map<std::string, std::vector<int>>::iterator it = filenames_records.find(f);
    bool found = false;

    /* TODO - Use a binary tree instead ! */
    // Create a binary tree for each filename
    // Store filename index in the binary tree
    
    /* We suppose that the array minimum possible value is 1 */
    std::sort(filenames_records[f].begin(), filenames_records[f].end());
    /* If filename already has duplicates, append (n) to it where n is equal to the index of duplicates */
    if (it != filenames_records.end()) {
        /* If vector size == 0, duplicate value = 1 */
        if (filenames_records[f].size() == 0) {
            found = true;
            n = 1;
        }
        /* If the first value is not 1 we can conclude 1 is the minimum missing value */
        else if (filenames_records[f].at(0) != 1) {
            n = 1;
            found = true;
        }
        else {

            for (int i = 0; i < filenames_records[f].size(); ++i) {
                /* Check if next element exists */
                if (i + 1 < filenames_records[f].size()) {
                    int j = i + 1;
                    /* If the difference between the next element and the current element is not one, then there 
                     * is a gap meaning that the min missing value is equal to current element + 1 */
                    if (filenames_records[f].at(j) - filenames_records[f].at(i) != 1) {
                        n = filenames_records[f].at(i) + 1;
                        found = true;
                        break;
                    } 
                }
            }
            if (!found) {
                /* If we could not find a gap, then n is equal to the largest element + 1 */
                n = filenames_records[f].at(filenames_records[f].size() - 1) + 1;
            }
        }

    }
    /* If not duplicates was found, n = 1 */
    /* Append duplicate value to filename vector of duplicates values */
    filenames_records[f].push_back(n);
    createDuplicate(f, n);
    /* Store original filename and duplicate value in dl_records and use final filename as key */
    dl_records.insert(dl_records.end(), std::pair<std::string, DownloadRecord>(f, {original_filename, n}));

    return f;
}

void Controller::createNewRecord(std::string& f)
{
    filenames_records.insert(filenames_records.end(), std::pair<std::string, std::vector<int>>(f, std::vector<int>{}));
}

/* Start one dl */
void Controller::startDl(const std::string& download_to_start)
{
    try {
        downloader_core_vec.at(downloads_map[download_to_start])->runThread();
    }
    catch (const std::out_of_range& oor) {
        // TODO - write to log 
    } 
}

/* TODO - Start multiple downloads at the same time */
void Controller::startDl(const std::vector<std::string>& downloads_to_start) {}

void Controller::resume(const std::string& dlToResume)
{
    downloader_core_vec.at(downloads_map[dlToResume])->resume();
}

void Controller::resumeAll()
{
    for (size_t i = 0; i < downloader_core_vec.size(); ++i) {
        downloader_core_vec.at(i)->resume();
    }
}

void Controller::pause(const std::string& dlToPause)
{
    downloader_core_vec.at(downloads_map[dlToPause])->pause();
}

void Controller::pauseAll()
{
    for (size_t i = 0; i < downloader_core_vec.size(); ++i) {
        if (downloader_core_vec.at(i)->getDownloadInfos()->status == downloadStatus::DOWNLOADING)
            downloader_core_vec.at(i)->pause();
    }
}

void Controller::clearInactive()
{
    assert(downloader_core_vec.size() == downloads_map.size());
    std::map<std::string, int>::iterator it;
    std::vector<size_t> todel;
    for (size_t i = 0; i < downloader_core_vec.size(); ++i) {
        if((downloader_core_vec.at(i)->getDownloadInfos()->status == downloadStatus::ERROR) || 
           (downloader_core_vec.at(i)->getDownloadInfos()->status == downloadStatus::COMPLETED)) {
            it = downloads_map.find(downloader_core_vec.at(i)->getDownloadInfos()->filename);
            if (it != downloads_map.end()) {
                /* Decrement downloads ids above deleted item since they are used as 
                 * indexes to access downloader_core_vec */
                /* TODO - could we start iterating by ref at some index instead of map::begin ? */
                /* TODO - resizedownloadsmap ? */
                for (auto& el : downloads_map) {
                    if (el.second > it->second) {
                        --el.second;
                    }
                }
                downloads_map.erase(it);
                todel.push_back(i);
            }
        }
    }
    if (!todel.empty()) {
        for (size_t i = 0; i < todel.size(); ++i) {
            downloader_core_vec.erase(downloader_core_vec.begin() + (todel.at(i) - i));
            dl_counter--;
        }

    }
}

void Controller::stop(const std::string& dlToStop)
{
    downloader_core_vec.at(downloads_map[dlToStop])->pause();
    /* downloads_map item returns an int corresponding to the index of the item in the vec */
    downloader_core_vec.erase(downloader_core_vec.begin() + downloads_map[dlToStop]);

    std::map<std::string, int>::iterator it = downloads_map.find(dlToStop);
    if (it != downloads_map.end()) {
        /* Decrement downloads ids above deleted item since they are used as 
         * indexes to access downloader_core_vec */
        for (auto & el : downloads_map) {
            if (el.second > it->second) {
                --el.second;
            }
        }   
        downloads_map.erase(it);
    }
    std::map<std::string, DownloadRecord>::iterator itb = dl_records.find(dlToStop);
    /* If dl to stop is actually a duplicate */
    if (itb != dl_records.end()) {
        /* Remove filename's duplicate number from filenames_records */
        std::string orig = itb->second.orig_filename;
        int d = itb->second.dup_num;
        filenames_records[orig].erase(std::remove(filenames_records[orig].begin(), filenames_records[orig].end(), d), filenames_records[orig].end());
    }

    /* Decrement counter */
    dl_counter--;
}

void Controller::stopAll()
{
    for (size_t i = 0; i < downloader_core_vec.size(); ++i)
        downloader_core_vec.at(i)->pause();
}

/* Stops all ongoing transfers - clear downloads menu, downloads list and filenames records - reset download counter 
 * Sets activeDls to false. Does not delete already downloaded bytes from the hard drive */
void Controller::killAll()
{
    stopAll();
    downloader_core_vec.clear();
    downloads_map.clear();
    filenames_records.clear();
    dl_counter = 0;
}

std::string Controller::getURL(const std::string& filename)
{
    return downloader_core_vec.at(downloads_map[filename])->getDownloadInfos()->url;
}

std::string Controller::getETA(const std::string& filename)
{
    return downloader_core_vec.at(downloads_map[filename])->getDownloadInfos()->eta;
}

double Controller::getProgress(const std::string& filename)
{
    return downloader_core_vec.at(downloads_map[filename])->getDownloadInfos()->progress;
}

double Controller::getSpeed(const std::string& filename)
{
    downloader_core_vec.at(downloads_map[filename])->downloadSpeed();
    return downloader_core_vec.at(downloads_map[filename])->getDownloadInfos()->speed;
}

downloadStatus Controller::getStatus(const std::string& filename)
{
    return downloader_core_vec.at(downloads_map[filename])->getDownloadInfos()->status;
}

/* Get all downloads statuses infos */
std::vector<downloadWinInfo> Controller::getAllDownloadsInfos() 
{
    std::vector<downloadWinInfo> vec;
    /* Loop over all downloads sorted by id */
    std::map<int, std::string> dlsSorted = sortDownloadsMapByIds();
    for (auto &dl: dlsSorted) {
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
        //std::string eta = stringifyETA(DownloaderCoreControlgetETA(f));
        const std::string eta = "ETA";
        vec.emplace_back(id, strstatus, spd, prog, eta);
    }
    return vec;
}

/* Returns false if the map is empty */
bool Controller::isActive()
{
    return downloads_map.size() != 0 ? true : false;  
}

std::map<int, std::string> Controller::sortDownloadsMapByIds()
{
    /* Start by sorting the downloads by id then flip the map and use the name of the downloads as the key
     * while keeping the map sorted by id */
    return flipMap(downloads_map);
}

std::vector<std::string> Controller::getDownloadsList()
{
    std::vector<std::string> vec;
    std::map<int, std::string> m = sortDownloadsMapByIds();
    for (auto el : m) {
        vec.emplace_back(el.second);
    }
    return vec;
}

