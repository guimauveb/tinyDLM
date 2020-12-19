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
    username = Environment::getUsername();
    downloads_dir = "/Users/" + username + "/Downloads";
    cpu_count = Environment::getCPUCount();
    max_simultaneous_transfers = cpu_count;
}

