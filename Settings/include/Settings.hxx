#include <filesystem>

#include "../../dialogs/include/dialogs.hxx"
#include "../../helper/include/Environment.hxx"
#include "../../Error/include/Error.hxx"
#include "../../Log/include/Log.hxx"

/* Settings are modified through the UI "Settings" window */
class Settings {
    public:
        /* Ran on tinyDLM initialization - check if a .conf file is present, otherwise create one with defaults values
         * and use these values. */
        Settings();

        Error load();

        /* Setters and getters */
        Error setDownloadsDirectory(const std::string& p);
        Error setMaximumTransferSpeed(const double& speed);
        Error setMaximumSimultaneousTransfers(const int& number);

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
        Error setDefaults();
        // TODO - move to helper
        Error createDirectory(const std::filesystem::path& p);
        bool directoryExists(const std::filesystem::path& p, std::filesystem::file_status s = std::filesystem::file_status{});
        // TODO
        std::string generateDefaultDownloadsFolder(const std::string& username);

        std::string username;
        std::string home_dir;
        std::string downloads_dir;
        std::string downloads_dir_abs_path;
        uint64_t max_transfer_speed;
        int max_simultaneous_transfers;
        int cpu_count;
};
