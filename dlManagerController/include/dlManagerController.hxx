/* Controls all dlManager instances */
#include "../../helper/include/flipMap.hxx"
#include "../../helper/include/stringify.hxx"
#include "../../helper/include/userInputCheck.hxx"

#include "../../dlManager/include/dlManager.hxx"

class dlManagerController 
{
    public:
        dlManagerController();
        ~dlManagerController();

        std::string createNewDl(std::string folder, std::string filename, const std::string url,
                         const int lowSpeedLim, const int lowSpeedTim);
        void removeDl();

        void startDl(const std::vector<std::string>& dlsToStart);
        void startDl(const std::string& dlToStart);
        void resume(const std::string& dlToResume);
        void resumeAll();
        void pause(const std::string& dlToPause);
        void pauseAll();
        void stop(const std::string& dlToStop);
        void stopAll();
        void clearInactive();
        void killAll();

        /* TODO */
        void saveStateAtExit();

        /* Functions returning info about the download */
        std::string getURL(const std::string& filename);
        std::string getETA(const std::string& filename);
        double getProgress(const std::string& filename);
        double getSpeed(const std::string& filename);
        downloadStatus getStatus(const std::string& filename);

        std::vector<downloadWinInfo> getAllDownloadsInfos();
        /* Returns false if there's 0 download */
        bool isActive();
        std::vector<std::string> getDownloadsList();

    private:
        /* Keep track of the number of downloads */
        int dlCounter;
        
        /* TODO - move outside */
        /* Struct storing filenames infos for every download. Allows to properly update filenamesRecord */
        /* E.g : archive(1)'s dlRecord = {origFilename: "archive", dupNum: 1} */
        struct dlRecord {
            const std::string origFilename;
            int dupNum;
        };

        /* Access dlRecord struct for every download - Map key is the final filename */
        /* E.g: User deletes archive(2), we retreive its dlRecord file with the following code:
         * dlRecs.find(archive(2)) -> returns its dlRecord struct with the following content: 
         * { origFilename: "archive", dupNum: 2 }. We then know that we can delete the value 2 from filenamesRecords
         * array for this original filename so it can be used again */
        std::map<std::string, dlRecord> dlRecs;

        /* Store filenames and indexes of duplicates for each filenames to generate proper non-conflicting 
         * filenames */
        /* E.g: filenamesRecords[archive] = [1, 3, 4]. Allows us to know that this filename will be named
         * archive(2) where 2 is the smallest missing value in the array */
        std::map<std::string, std::vector<int>> filenamesRecords;
        
        /* Manage duplicate filenames */
        std::string& recordDuplicate(std::string& f);
        void createNewRecord(std::string& f);
        
        /* Maps filename to its unique id */
        std::map<std::string, int> downloadsMap;
        std::map<int, std::string> sortDownloadsMapByIds();

        /* dlManager instances are stored in a vector of unique_ptrs */
        std::vector<std::unique_ptr<dlManager>> dlManagerVec;
};

