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
        /* Store filenames and number of duplicates for each filenames to generate proper non-conflicting filenames */
        std::map<std::string, int> filenamesRecords;
        /* Keep track of the number of downloads */
        int dlCounter;

        /* dlManager instances are stored in a vector of unique_ptrs */
        std::vector<std::unique_ptr<dlManager>> dlManagerVec;

        /* Maps a filename to a download id */
        std::map<std::string, int> downloadsMap;
        std::map<int, std::string> sortDownloadsMapByIds();

};
