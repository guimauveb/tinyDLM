#include <syslog.h>
#include <string>

class Log {
    public:
        // Defaults to LOG_ERR.
        Log()
            :error_level(LOG_ERR)
        {}
        // Initialize Log with an error level. 
        Log(int e_level) 
            :error_level(e_level)
        {}

        void writeToSyslog(const std::string& fs, int err_level = LOG_ERR) {
            openlog("tinyDLM",  LOG_PID, LOG_USER);
            syslog(err_level, "%s", fs.c_str());
            closelog();
        }

        // Write to syslog using << operator. 
        template <typename T>
            Log& operator<<(const T& p_value)
            {
                writeToSyslog(p_value, error_level);
                return *this;
            }

    private:
        int error_level;
};
