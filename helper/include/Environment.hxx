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
                err.code = ErrorCode::dir_creat_ok;
                err.message = "Directory successfully created.";
            }
            /* Error will travel back to the settings window */
            catch (const std::filesystem::filesystem_error& e) {
                Log() << e.what();
                err.code = ErrorCode::dir_creat_err;
                err.message = "Error when creating directory. Check error logs."; 
            }

            return err;
        }

        // Return false if directory does not exist. Keep from saving the settings if this is the case.
        static bool directoryExists(const std::filesystem::path& p, std::filesystem::file_status s = std::filesystem::file_status{})
        {
            return std::filesystem::status_known(s) ? std::filesystem::exists(s) : std::filesystem::exists(p);
        }

        // Expand tilde to absolute path if '~' is the first char of the string, else return path untouched.
        static std::string expandTilde(const std::string h_dir, const std::string p)
        {
            std::string expanded;

            try {
                if (p.at(0) == '~') {
                    if (p.at(1) == '/') {
                        expanded = h_dir + p.substr(2, p.length());
                    }
                    else {
                        expanded = h_dir + p.substr(1, p.length());
                    }
                }
                else {
                    expanded = p;
                }
            }
            catch (std::out_of_range& e) {
                Log() << e.what();
            }

            return expanded;

        }

        // Convert full path to ~ notation path if path matches the pattern home_dir + remainder
        static std::string tildefyPath(const std::string h_dir, const std::string p)
        {
            const std::string tilde = "~/";
            std::string remainder;
            std::string tildefied;

            try {
                if (p.substr(0, h_dir.length()) == h_dir) {
                    remainder = p.substr(h_dir.length(), p.length() - 1);
                    tildefied = tilde + remainder; 
                }
                else {
                    tildefied = p;
                }
            }
            catch (std::out_of_range& e) {
                Log() << e.what();
            }

            return tildefied;
        }
};
#endif
