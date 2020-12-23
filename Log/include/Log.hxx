#ifndef LOG_HXX
#define LOG_HXX

#include <syslog.h>
#include <sstream>

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
        
        // Write buffer content on destruction
        ~Log()
        {
            writeToSyslog(buffer.str(), error_level);    
        }

        void writeToSyslog(const std::string& fs, int err_level = LOG_ERR) {
            openlog("tinyDLM",  LOG_PID, LOG_USER);
            syslog(err_level, "%s", fs.c_str());
            closelog();
        }

        // Store message into buffer
        template <typename T>
            Log& operator<<(const T& p_value)
            {
                buffer << p_value;
                return *this;
            }

    private:
        int error_level = 0;
        std::stringstream buffer;
};
#endif
