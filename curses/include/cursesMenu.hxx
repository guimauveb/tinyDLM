#include "cursesForm.hxx"
#include <menu.h>

#include <vector>
#include <map>
#include <cstring>

class cursesMenu {
    public:
        cursesMenu(std::vector<std::string> itemsData);
        cursesMenu();
        ~cursesMenu();

        void newMenu();
        void menuOptsOff(Menu_Options);
        void menuOptsOn(Menu_Options);
        /* We use unique_ptr references since we know the pointer will outlive the cursesMenu object */
        void setMenuSub(std::unique_ptr<cursesWindow>& win);
        void setMenuFormat(int r, int c);
        void setMenuMark(const char *m);
        void postMenu();
        void clearMenu();

        void menuDriver(int c);

        std::string getItemName();
        int getItemNo();
        int getItemNum();

        void clearItems();


    private:
        bool itemsFreed = false;
        bool menuFreed = false;
        std::vector<std::string> itemStrings;
        std::map<std::string, int> itemsMap;
        std::vector<ITEM *> items;
        MENU *menu = nullptr;
};
