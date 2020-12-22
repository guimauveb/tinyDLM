#include "../include/Settings.hxx"

// Load system info from Environment.
Settings::Settings()
    :username(Environment::getUsername()),  home_dir("/Users/" + username + "/")
     ,cpu_count(Environment::getCPUCount())
{}

Error Settings::load()
{
    /* TODO - 
     *      Check if .conf exists in config/
     *      If .conf -> read settings and copy them into the variables 
     *      Else create default values using system information.
     */

    // Temporary
    return setDefaults();
}

/* If everything went fine, create .conf file in ~/.tinyDLM and signal to UI to display new user window
 * with default info. */
Error Settings::setDefaults()
{
    Error err;

    max_simultaneous_transfers = cpu_count;
    max_transfer_speed = 0;
    /* TODO - Use ~ to display path if dir in $HOME
     * Something like str::replace('~', home_dir) */
    downloads_dir = "~/Downloads/tinyDownloads/";
    downloads_dir_abs_path = home_dir + "Downloads/tinyDownloads/";

    Error dir_err = setDownloadsDirectory(downloads_dir_abs_path);

    // TODO - writeConfig()
    if (dir_err.code != ErrorCode::dir_creat_err) {
        FileIO(home_dir + ".tinyDLM") << "downloads_directory=" << downloads_dir_abs_path <<
            "\nmax_download_speed=" << std::to_string(max_transfer_speed) << 
            "\nmax_simultaneous_transfers=" << std::to_string(max_simultaneous_transfers);

        err.code = ErrorCode::first_start_ok;
        err.message = msgNewUserOk; 
    }
    else {
        err.code = ErrorCode::first_start_err;
        err.message = msgNewUserErr;
    }

    return err;
}

// TODO - Move to helper
Error Settings::createDirectory(const std::filesystem::path& p)
{
    Error err;

    try {
        std::filesystem::create_directories(p);
        std::filesystem::permissions(p,
                std::filesystem::perms::owner_all  |
                std::filesystem::perms::group_read | 
                std::filesystem::perms::group_exec |
                std::filesystem::perms::others_read); 
    }
    /* Error will travel back to the settings window */
    catch (const std::filesystem::filesystem_error& e) {
        // Write original error code to syslog.
        Log() << e.what();

        // Return a readable error to the user.
        err.code = ErrorCode::dir_creat_err;
        err.message = "Error when creating directory. Check error logs."; 
    }

    return err;
}

// TODO - Move to helper
// Return false if directory does not exist. Keep from saving the settings if this is the case.
bool Settings::directoryExists(const std::filesystem::path& p, std::filesystem::file_status s)
{
    return std::filesystem::status_known(s) ? std::filesystem::exists(s) : std::filesystem::exists(p);
}

// Returns true if the path returned a valid directory. Else signal error in settings window.
Error Settings::setDownloadsDirectory(const std::string& p)
{
    Error err;

    if (directoryExists(p)) {
        err.code = ErrorCode::dir_exists_err;
        err.message = "Directory already exists.";
    }
    else {
        err = createDirectory(p);
        if (err.code == ErrorCode::dir_creat_ok) {
            downloads_dir_abs_path = p;
        }
    }

    return err;
}

Error Settings::setMaximumTransferSpeed(const double& speed)
{
    Error err;
    return err;
}

Error Settings::setMaximumSimultaneousTransfers(const int& number) 
{
    Error err;
    return err;
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

void Settings::writeConfigFile(const std::string& dir, const std::string& max_speed, const std::string& max_sim_trans)
{

}

// TODO - move to helper functions
// Convert absolute path to ~ path if path is in $HOME

