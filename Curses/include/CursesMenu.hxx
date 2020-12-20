#include "CursesForm.hxx"
#include <menu.h>

#include <vector>
#include <map>
#include <cstring>

class CursesMenu {
    public:
        CursesMenu(std::vector<std::string> items_data);
        CursesMenu();
        ~CursesMenu();

        void newMenu();
        void menuOptsOff(Menu_Options);
        void menuOptsOn(Menu_Options);

        /* We use unique_ptr references since we know the pointer will outlive the CursesMenu object */
        void setMenuSubDer(std::unique_ptr<CursesWindow>& win, int a, int b, int c, int d);
        void setMenuDer(std::unique_ptr<CursesWindow>& win);
        void setMenuWin(std::unique_ptr<CursesWindow>& win);
        void setMenuSub(std::unique_ptr<CursesWindow>& win);

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
        std::vector<std::string> items_strings;
        std::map<std::string, int> items_map;
        std::vector<ITEM *> items;
        MENU *menu = nullptr;
};
