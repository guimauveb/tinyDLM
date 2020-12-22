#ifndef ERROR_HXX
#define ERROR_HXX

#include <string>

enum class ErrorCode {
    err_ok, 
    first_start_ok,
    first_start_err,
    dir_creat_ok,
    dir_creat_err,
    dir_exists_err
};

/* Used to return an error code and print a message in curses windows. */
struct Error {
    ErrorCode code;
    std::string message;
};

#endif

