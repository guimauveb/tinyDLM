#include "../include/Settings.hxx"

#include <iostream>

Settings::Settings()
{
    loadSettings();
}

void Settings::loadSettings()
{
    /* TODO - 
     *      Check if .conf exists in config/
     *      If .conf -> read settings and copy them into the variables 
     *      Else create default values using system information.
     */
    setDefaults();
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

// TODO - Use Environment class
void Settings::setDefaults()
{
    username = Environment::getUsername();

    downloads_dir = "~/Downloads/tinyDownloads/";
    downloads_dir_abs_path = "/Users/" + username + "/Downloads/tinyDownloads/";
    mkdir(downloads_dir_abs_path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    cpu_count = Environment::getCPUCount();
    max_simultaneous_transfers = cpu_count;
    max_transfer_speed = 0;
}

void Settings::setDownloadsDirectory(const std::string& path)
{

}

void Settings::setMaximumTransferSpeed(const double& speed)
{

}

void Settings::setMaximumSimultaneousTransfers(const int& number) 
{
}

