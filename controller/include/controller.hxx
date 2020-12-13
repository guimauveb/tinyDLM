/* Controls all DownloadManager instances. */
#include "../../helper/include/flipMap.hxx"
#include "../../helper/include/stringify.hxx"
#include "../../helper/include/userInputCheck.hxx"

#include "../../helper/include/BinaryTree.hxx"

#include "../../DownloaderCore/include/DownloaderCore.hxx"

class Controller 
{
    public:
        Controller();
        ~Controller();

        std::string createNewDl(std::string folder, std::string filename, const std::string url,
                const int low_speed_limit, const int low_speed_time_limit);
        void removeDl();

        void startDl(const std::vector<std::string>& downloads_to_start);
        void startDl(const std::string& download_to_start);
        void resume(const std::string& download_to_resule);
        void resumeAll();
        void pause(const std::string& download_to_pause);
        void pauseAll();
        void stop(const std::string& download_to_stop);
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
        int dl_counter;

        /* Struct storing filenames infos for every download. Allows to properly update filenamesRecord */
        /* E.g : archive(1)'s dlRecord = {origFilename: "archive", dupNum: 1} */
        /* TODO - Move to its proper place */
        struct DownloadRecord {
            const std::string orig_filename;
            int dup_num;
        };

        /* Access dlRecord struct for every download - Map key is the final filename */
        /* E.g: User deletes archive(2), we retreive its dlRecord file with the following code:
         * dlRecs.find(archive(2)) -> returns its dlRecord struct with the following content: 
         * { origFilename: "archive", dupNum: 2 }. We then know that we can delete the value 2 from filenamesRecords
         * array for this original filename so it can be used again */
        std::map<std::string, DownloadRecord> dl_records;

        /* Store filenames and indexes of duplicates for each filenames to generate proper non-conflicting 
         * filenames */
        /* E.g: filenamesRecords[archive] = [1, 3, 4]. Allows us to know that this filename will be named
         * archive(2) where 2 is the smallest missing value in the array */
        std::map<std::string, std::vector<int>> filenames_records;

        /* Manage duplicate filenames */
        std::string& recordDuplicate(std::string& f);
        void createNewRecord(std::string& f);

        /* Maps filename to its unique id */
        std::map<std::string, int> downloads_map;
        std::map<int, std::string> sortDownloadsMapByIds();

        /* dlManager instances are stored in a vector of unique_ptrs */
        std::vector<std::unique_ptr<DownloaderCore>> downloader_core_vec;
};

