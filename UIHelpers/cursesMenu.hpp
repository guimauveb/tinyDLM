#include "cursesForm.hpp"
#include <menu.h>

#include <vector>
#include <iostream>
#include <cstring>

class cursesMenu {
    public:
        cursesMenu(std::vector<std::string> itemsData);
        cursesMenu();
        ~cursesMenu();

        void newMenu();
        void menuOptsOff(Menu_Options);
        void menuOptsOn(Menu_Options);
        void setMenuWin(std::unique_ptr<cursesWindow>& win);
        void setMenuSub(std::unique_ptr<cursesWindow>& win, int nl, int nc, int begy, int begx);
        void setMenuFormat(int r, int c);
        void setMenuMark(const char *m);
        void postMenu();
        void clearMenu();

        void menuDriver(int c);
        std::string getItemName();
        void clearItems();

    private:
        std::vector<std::string> itemStrings;
        std::vector<ITEM *> items;
        MENU *menu = nullptr;
};
