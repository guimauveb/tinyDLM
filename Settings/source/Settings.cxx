#include "../include/Settings.hxx"

#include <iostream>

Settings::Settings()
{
    std::cout << Environment::getUsername() << '\n';
    loadSettings();
}

void Settings::loadSettings()
{
    /* TODO - 
     *      Check if .conf exists in config/
     *      If .conf -> read settings and copy them into the variables 
     *      Else create default values using system information.
     */
    downloads_dir = "";
    max_transfer_speed = 0;
    max_transfers_num = 0;
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

std::string Settings::getDownloadsDirectory(const std::string& path)
{
    return "lol";
}

double Settings::getMaximumTransferSpeed(const double& speed)
{
    return 7;
}

int Settings::getMaximumSimultaneousTransfers(const int& number)
{
    return 0;
}

// TODO - Use Environment class
void Settings::setDefaults()
{
    /* TODO - Get cpu number
     *      - Get username to set the default downloads folder to /Users/<username>/Downloads
     *      - Other infos ? 
     */
}

std::string Settings::generateDefaultDownloadsFolder(const std::string& username)
{
    return "/Users/<username>/Downloads";
}
