#include "../include/CursesMenu.hxx"

/* Initialize a menu with items as strings corresponding to downloads filenames */
CursesMenu::CursesMenu(std::vector<std::string> items_data)
{
    items_strings = items_data;
    /* Push choices elements into a vector of ITEM * */
    for (size_t i = 0; i < items_strings.size(); ++i) {
        /* The filename of each download will be used as the item name */
        items.push_back(new_item(items_strings.at(i).c_str(), nullptr)); 
        /* Update map - each item has a corresponding id */
        items_map[items_strings.at(i)] = i;
    }

    /* Push back a null item */
    items.push_back((ITEM *)nullptr);
    menu = new_menu((ITEM **)items.data());

}

int CursesMenu::getItemNum()
{
    return (int)items.size();
}

/* Returns highlighted item id */
int CursesMenu::getItemNo()
{
    std::string tmp = getItemName();
    std::map<std::string, int>::iterator it = items_map.find(tmp);
    if (it != items_map.end()) {
        return it->second;
    }
    /* Should not happen */
    return 1; 
}

/* Creates an empty CursesMenu object to initialize the program */
CursesMenu::CursesMenu(){}

CursesMenu::~CursesMenu()
{
    clearMenu();
    clearItems();
}

void CursesMenu::clearMenu()
{
    /* If menu already cleared manually and not by the destructor, set to yes so we don't try to free it twice */
    if (!menuFreed) {
        unpost_menu(menu);
        free_menu(menu);
        menuFreed = true;
    }
}

void CursesMenu::clearItems()
{
    if (!itemsFreed) {
        const int n = items.size();
        for(int i = 0; i < n; ++i)
            free_item(items[i]);
        items.clear();
        items_strings.clear();
        itemsFreed = true;
    }

}

void CursesMenu::menuOptsOff(Menu_Options)
{
    menu_opts_off(menu, O_SHOWDESC);
}

void CursesMenu::menuOptsOn(Menu_Options)
{

}

void CursesMenu::setMenuWin(std::unique_ptr<CursesWindow>& win)
{
    set_menu_win(menu, win->getRawPtr());
}

/* Works on macOS, not on Linux */
void CursesMenu::setMenuSubDer(std::unique_ptr<CursesWindow>& win, int a, int b, int c, int d)
{
    set_menu_win(menu, derwin(win->getRawPtr(), a, b, c, d));
}

/* Set the window to be associated with the menu */
void CursesMenu::setMenuSub(std::unique_ptr<CursesWindow>& win)
{
    set_menu_sub(menu, win->getRawPtr());
}

void CursesMenu::setMenuDer(std::unique_ptr<CursesWindow>& der)
{
    set_menu_sub(menu, der->getDerwin());
}

/* Set the number of elements to fit the screen size */
void CursesMenu::setMenuFormat(int r, int c)
{
    set_menu_format(menu, r, c);
}

void CursesMenu::postMenu()
{
    post_menu(menu);
}

void CursesMenu::setMenuMark(const char *m)
{
    set_menu_mark(menu, m);
}

std::string CursesMenu::getItemName()
{
    std::string itemName = item_name(current_item(menu));
    /* char * returned by item_name doesn't seem to be null terminated */
    itemName.push_back('\0');
    return itemName;
}

void CursesMenu::menuDriver(int c)
{
    menu_driver(menu, c);
}

