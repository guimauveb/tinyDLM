#include "../include/Settings.hxx"

#include <iostream>

Settings::Settings()
{
    loadSettings();
}

bool Settings::loadSettings()
{
    username  = Environment::getUsername();
    cpu_count = Environment::getCPUCount();
    home_dir = "/Users/" + username + "/";

    /* TODO - 
     *      Check if .conf exists in config/
     *      If .conf -> read settings and copy them into the variables 
     *      Else create default values using system information.
     */

    // tmp
    return setDefaults();
}

bool Settings::setDefaults()
{
    bool err = false;
    downloads_dir = "~/Downloads/tinyDownloads/";
    downloads_dir_abs_path = "/Users/" + username + "/Downloads/tinyDownloads/";
    err = createDirectory(downloads_dir_abs_path);

    max_simultaneous_transfers = cpu_count;
    max_transfer_speed = 0;

    return err;
}

bool Settings::createDirectory(const std::filesystem::path& p)
{
    std::filesystem::create_directories(downloads_dir_abs_path);
    std::filesystem::permissions(downloads_dir_abs_path,
            std::filesystem::perms::owner_all  |
            std::filesystem::perms::group_read | 
            std::filesystem::perms::group_exec |
            std::filesystem::perms::others_read); 

    // TODO - better error checking
    bool exists = directoryExists(downloads_dir_abs_path);

    return exists;
}

// Return false if directory does not exist. Keep from saving the settings if this is the case.
bool Settings::directoryExists(const std::filesystem::path& p, std::filesystem::file_status s)
{
    return std::filesystem::status_known(s) ? std::filesystem::exists(s) : std::filesystem::exists(p);
}

// Returns true if the path returned a valid directory. Else signal error in settings window.
bool Settings::setDownloadsDirectory(const std::string& path)
{
    bool exists = createDirectory(path);

    if (exists) {
        downloads_dir_abs_path = path;
    }

    return exists;
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
