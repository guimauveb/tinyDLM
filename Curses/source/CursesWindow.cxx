#include "../include/CursesWindow.hxx"

CursesWindow::CursesWindow(int r, int c, int by, int bx, std::string name)
    :row{r}, col{c}, begy{by}, begx{bx}, win_name{name}
{
    if ((win = newwin(row, col, begy, begx)) == NULL) {
        Log() << "Could not allocate memory for window " << win_name; 
        exit(1);
    }
    wrefresh(win);
}

CursesWindow::~CursesWindow()
{
    delwin(win);
}

WINDOW *CursesWindow::getRawPtr()
{
    return win;
}

void CursesWindow::winAttrOn(chtype ct)
{
    wattron(win, ct);
}

void CursesWindow::winAttrOff(chtype ct)
{
    wattroff(win, ct);
}

void CursesWindow::touchWin()
{
    touchwin(win);
}

void CursesWindow::refreshWin()
{
    wrefresh(win);
}

void CursesWindow::eraseWin()
{
    werase(win);
}

/* reset == erase then refresh */
void CursesWindow::resetWin()
{
    werase(win);
    wrefresh(win);
}

/* Couldn't get wresize() to work properly. Calling delwin() then newwin() instead */
void CursesWindow::resizeWin(WinSize new_size)
{
    if (win != nullptr) {
        if (delwin(win) == ERR) {
            Log() << "Error while freeing memory of window " << win_name;
        }
    }

    row = new_size.row;
    col = new_size.col;
    begy = new_size.begy;
    begx = new_size.begx;

    win = newwin(new_size.row, new_size.col, new_size.begy, new_size.begx);
}

void CursesWindow::printInMiddle(int starty, int startx, int width, std::string str, chtype color)
{
    int length, x, y;
    float temp;

    getyx(win, y, x);
    if (startx != 0)
        x = startx;
    if (starty != 0)
        y = starty;
    if (width == 0)
        width = col;

    length = str.length();
    temp = (width - length) / 2;
    x = startx + (int)temp;
    wattron(win, color);
    mvwprintw(win, y, x, "%s", str.c_str());
    wattroff(win, color);
    refresh();
}

// Pass text coordinates, text, color pair, row size
void CursesWindow::printInMiddleWithBackground(int y, int x, int str_begin, const std::string& str, chtype color)
{
    char *banner = (char*)malloc((col + 1)*sizeof(char));
    int i = 0;

    // First for loop = print background before str begining
    for (; i < str_begin; ++i) {
        banner[i] = ' '; 
    }
    // Second for loop - print str starting at str_begin
    for (; i < (int)str.length(); ++i) {
        banner[i] = str.at(i);
    }
    // Third for loop - print background color after str
    for (; i < col; ++i) {
        banner[i] = ' ';
    }
    banner[i] = '\0';

    wattron(win, color);
    mvwprintw(win, y, x, "%s", banner);
    wattroff(win, color);
}

point CursesWindow::getBegyx()
{
    int by, bx;
    getbegyx(win, by, bx);
    return point {.y = by, .x = bx};
}

point CursesWindow::getMaxyx()
{
    int my, mx;
    getmaxyx(win, my, mx);
    return point {.y = my, .x = mx};
}

void CursesWindow::addStr(int y, int x, std::string str)
{
    mvwaddstr(win, y, x, str.c_str());
}

void CursesWindow::drawBox(int y, int x)
{
    box(win, y, x);
}

void CursesWindow::keyPad(bool b)
{
    keypad(win, b);
}

void CursesWindow::wMove(int y, int x)
{
    wmove(win, y, x);
    wrefresh(win);
}

void CursesWindow::clearToEOL()
{
    clrtoeol();
}

void CursesWindow::setDerwin(int nl, int nc, int by, int bx) 
{
    der = derwin(win, nl, nc, by, bx);    
}

WINDOW * CursesWindow::getDerwin()
{
    return der;
}
