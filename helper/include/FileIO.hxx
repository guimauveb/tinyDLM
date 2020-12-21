#include <fstream>
#include <sstream>

#include "../../Log/include/Log.hxx"

class FileIO {
    public:
        FileIO(const std::string& f)
            :filename(f)
        {}

        // Write to file using << operator. 
        template <typename T>
            void operator<<(const T& p_value)
            {
                std::ofstream out_f(filename);
                if (out_f.is_open()) {
                    out_f << p_value << '\n';
                    out_f.close();
                }
                else {
                    // Log error to syslog
                    Log() << "Unable to open file '" << filename << "'\n";
                }

            }
        // Read from file using >> operator. 
        template <typename T>
            std::string operator>>(const T& p_value)
            {
                std::ifstream in_f(filename);
                std::stringstream buffer;
                if (in_f.is_open()) {
                    buffer << in_f.rdbuf();
                    in_f.close();
                    return buffer.str();
                }

                Log() << "Unable to open file '" << filename << "'\n";
            }
    private:
        std::string filename;
};
