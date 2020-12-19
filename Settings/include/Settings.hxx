#include <filesystem>
#include "../../helper/include/Environment.hxx"

/* Settings are modified through the UI "Settings" window */
class Settings {
    public:
        /* Ran on launch - check if a .conf file is present, otherwise create one with defaults values
         * and use these values. */
        Settings();

        void loadSettings();

        /* Setters and getters */
        void setDownloadsDirectory(const std::string& path);
        void setMaximumTransferSpeed(const double& speed);
        void setMaximumSimultaneousTransfers(const int& number);

        std::string getDownloadsDirectory();
        std::string getDownloadsDirAbsPath();
        /* TODO - use an unsigned 64 bit here */
        double getMaximumTransferSpeed();
        int getMaximumSimultaneousTransfers();

    private:
        /* Defaults:
         *      - Downloads directory: /Users/<username>/Downloads
         *      - Maximum transfer speed: no limit
         *      - Maximum simultaneous transfers: number of cpu's
         */
        /* Defaults are obtained from Environment */
        void setDefaults();
        std::string generateDefaultDownloadsFolder(const std::string& username);

        std::string username;
        std::string downloads_dir;
        std::string downloads_dir_abs_path;
        uint64_t max_transfer_speed;
        int max_simultaneous_transfers;
        int cpu_count;
};
