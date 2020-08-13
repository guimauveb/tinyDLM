#include "../dlManagerUI.hpp"

int main()
{
    /* Initialize our dlManager user interface */
    std::unique_ptr<dlManagerUI> dlmc = std::make_unique<dlManagerUI>();
    dlmc->Browser();
    return 0;
}

