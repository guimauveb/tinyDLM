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

        std::string getDownloadsDirectory(const std::string& path);
        double getMaximumTransferSpeed(const double& speed);
        int getMaximumSimultaneousTransfers(const int& number);

    private:
        /* Defaults:
         *      - Downloads directory: app/downloads
         *      - Maximum transfer speed: no limit
         *      - Maximum simultaneous transfers: number of cpu's
         */
        /* Obtained from Environment */
        void setDefaults();
        std::string generateDefaultDownloadsFolder(const std::string& username);

        std::string downloads_dir;
        double max_transfer_speed;
        int max_transfers_num;
};
