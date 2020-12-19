#include "../include/Settings.hxx"

#include <iostream>

Settings::Settings()
{
    loadSettings();
}

void Settings::loadSettings()
{
    username  = Environment::getUsername();
    cpu_count = Environment::getCPUCount();
    home_dir = "/Users/" + username + "/";

    /* TODO - 
     *      Check if .conf exists in config/
     *      If .conf -> read settings and copy them into the variables 
     *      Else create default values using system information.
     */

    setDefaults();
}

void Settings::setDefaults()
{
    downloads_dir = "~/Downloads/tinyDownloads/";
    downloads_dir_abs_path = "/Users/" + username + "/Downloads/tinyDownloads/";
    createDirectory(downloads_dir_abs_path);

    max_simultaneous_transfers = cpu_count;
    max_transfer_speed = 0;
}

bool Settings::createDirectory(const std::filesystem::path& p)
{
    std::filesystem::create_directories(downloads_dir_abs_path);
    std::filesystem::permissions(downloads_dir_abs_path,
            std::filesystem::perms::owner_all  |
            std::filesystem::perms::group_read | 
            std::filesystem::perms::group_exec |
            std::filesystem::perms::others_read); 

    return directoryExists(downloads_dir_abs_path);
}

// TODO - return false if directory does not exist. Keep from saving the settings if this is the case.
bool Settings::directoryExists(const std::filesystem::path& p, std::filesystem::file_status s)
{
    bool err = false;
    std::string err_msg;
    if (!(std::filesystem::status_known(s) ? std::filesystem::exists(s) : std::filesystem::exists(p))) {
        err = true;
    }
    return err;
}

bool Settings::setDownloadsDirectory(const std::string& path)
{
    return false;
}

bool Settings::setMaximumTransferSpeed(const double& speed)
{
    return false;
}

bool Settings::setMaximumSimultaneousTransfers(const int& number) 
{
    return false;
}

std::string Settings::getDownloadsDirectory()
{
    return downloads_dir;
}

std::string Settings::getDownloadsDirAbsPath()
{
    return downloads_dir_abs_path;
}

/* TODO - use an unsigned 64 bit here */
double Settings::getMaximumTransferSpeed()
{
    return max_transfer_speed;
}

int Settings::getMaximumSimultaneousTransfers()
{
    return max_simultaneous_transfers;
}

// TODO - move to helper functions
// /Users/<username>/ to ~
