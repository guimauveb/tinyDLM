#ifndef ENVIRONMENT_HXX
#define ENVIRONMENT_HXX

#include <unistd.h>
#include <pwd.h>
#include <thread>
#include <string>
#include <filesystem>

#include "../../Error/include/Error.hxx"
#include "../../Log/include/Log.hxx"

class Environment {
    public:
        /* whoami */
        static std::string getUsername()
        {
            uid_t uid = geteuid();
            struct passwd *pw = getpwuid(uid);
            if (pw) {
                return std::string(pw->pw_name);
            }
            return {};
        }
        static int getCPUCount()
        {
            return (int)std::thread::hardware_concurrency();
        }

        static Error createDirectory(const std::filesystem::path& p)
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

        // Return false if directory does not exist. Keep from saving the settings if this is the case.
        static bool directoryExists(const std::filesystem::path& p, std::filesystem::file_status s)
        {
            return std::filesystem::status_known(s) ? std::filesystem::exists(s) : std::filesystem::exists(p);
        }
};
#endif
