#include <future>

/* Returns true when the future object is done executing - must do before continuing the execution of the rest
 * of the program */
template<typename T> bool futureIsReady(std::future<T>& t)
{
    return t.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready;
}



