#include <unistd.h>
#include <pwd.h>
#include <string>

class Environment {
    public:
        static std::string getUsername()
        {
            uid_t uid = geteuid();
            struct passwd *pw = getpwuid(uid);
            if (pw) {
                return std::string(pw->pw_name);
            }
            return {};
        }
        // TODO
        static int getCPUNumber()
        {
            return 777;
        }
};
