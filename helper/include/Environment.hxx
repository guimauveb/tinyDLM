#include <unistd.h>
#include <pwd.h>
#include <thread>
#include <string>

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
};
