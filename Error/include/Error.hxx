#include <string>

/* Used to return an error code and print a message in curses windows. */
struct Error {
    int code = 0;
    std::string message;
};
