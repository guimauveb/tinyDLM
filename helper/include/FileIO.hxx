#include <fstream>
#include <sstream>

#include "../../Log/include/Log.hxx"

class FileIO {
    public:
        FileIO(const std::string& f)
            :filename(f), out_f(filename), in_f(filename)
        {}

        ~FileIO()
        {
            if (out_f.is_open() && write) {
                out_f << out_buffer.str();
                out_f.close();
            }
            else {
                Log() << "Unable to open file '" << filename << "'\n";
            }
        }

        // Content will be stored in buffer and written to filename on object destruction.
        template <typename T>
            FileIO& operator <<(const T& p_value)
            {
                write = true;
                out_buffer << p_value;
                return *this;
            }

        // Read from file using >> operator. 
        template <typename T>
            std::string operator>>(const T& p_value)
            {
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
        std::ofstream out_f;
        std::ifstream in_f;
        std::stringstream out_buffer;
        bool write = false;
};
