#include "../include/cursesMenu.hpp"

/* Initialize a menu with items as strings corresponding to downloads filenames */
cursesMenu::cursesMenu(std::vector<std::string> itemsData)
{
    itemStrings = itemsData;
    /* Push choices elements into a vector of ITEM * */
    for (size_t i = 0; i < itemStrings.size(); ++i) {
        /* The filename of each download will be used as the item name */
        items.push_back(new_item(itemStrings.at(i).c_str(), nullptr)); 
        /* Update map - each item has a corresponding id */
        itemsMap[itemStrings.at(i)] = i;
    }

    /* Push back a null item */
    items.push_back((ITEM *)nullptr);
    menu = new_menu((ITEM **)items.data());
}

int cursesMenu::getItemNum()
{
    return (int)items.size();
}

/* Returns highlighted item id */
int cursesMenu::getItemNo()
{
    std::string tmp = getItemName();
    std::map<std::string, int>::iterator it = itemsMap.find(tmp);
    if (it != itemsMap.end()) {
        return it->second;
    }
    /* Should not happen */
    return 1; 
}

/* Creates an empty cursesMenu object to initialize the program */
cursesMenu::cursesMenu(){}

cursesMenu::~cursesMenu()
{
    clearMenu();
    clearItems();
}

void cursesMenu::clearMenu()
{
    unpost_menu(menu);
    free_menu(menu);
}

void cursesMenu::clearItems()
{
    const int n = items.size();
    for(int i = 0; i < n; ++i)
        free_item(items[i]);
    items.clear();
    itemStrings.clear();
}

void cursesMenu::menuOptsOff(Menu_Options)
{
    menu_opts_off(menu, O_SHOWDESC);
}

void cursesMenu::menuOptsOn(Menu_Options)
{

}

/* Set the window to be associated with the menu */
void cursesMenu::setMenuWin(std::unique_ptr<cursesWindow>& win)
{
    set_menu_win(menu, win->getRawPtr());   
}

void cursesMenu::setMenuSub(std::unique_ptr<cursesWindow>& win, int nl, int nc, int begy, int begx)
{
    set_menu_sub(menu, derwin(win->getRawPtr(), nl, nc, begy, begx));
}

/* Set the number of elements to fit the screen size */
void cursesMenu::setMenuFormat(int r, int c)
{
    set_menu_format(menu, r, c);
}

void cursesMenu::postMenu()
{
    post_menu(menu);
}

void cursesMenu::setMenuMark(const char *m)
{
    set_menu_mark(menu, m);
}

std::string cursesMenu::getItemName()
{
    std::string itemName = item_name(current_item(menu));
    /* char * returned by item_name doesn't seem to be null terminated */
    itemName.push_back('\0');
    return itemName;
}

void cursesMenu::menuDriver(int c)
{
    menu_driver(menu, c);
}

