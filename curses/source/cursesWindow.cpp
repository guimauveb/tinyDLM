#include "../include/cursesWindow.hpp"
#include <fstream>

cursesWindow::cursesWindow(int r, int c, int by, int bx, std::string name)
    :row{r}, col{c}, begy{by}, begx{bx}
{
    if ((win = newwin(row, col, begy, begx)) == nullptr) {

    }
    winName = name;
    wrefresh(win);
}

cursesWindow::~cursesWindow()
{
    delwin(win);
}

WINDOW *cursesWindow::getRawPtr()
{
    return win;
}

void cursesWindow::winAttrOn(chtype ct)
{
    wattron(win, ct);
}

void cursesWindow::winAttrOff(chtype ct)
{
    wattroff(win, ct);
}

void cursesWindow::touchWin()
{
    touchwin(win);
}

void cursesWindow::refreshWin()
{
    wrefresh(win);
}

void cursesWindow::eraseWin()
{
    werase(win);
}

/* reset == erase then refresh */
void cursesWindow::resetWin()
{
    werase(win);
    wrefresh(win);
}

/* We need to delete the reinit the window to properly resize it - wresize() only changes row and col */
void cursesWindow::resizeWin(winSize newSz)
{
    int r = delwin(win);
    if (r == ERR) {
        std::ofstream ofstr;
        std::string e = "Error while freeing memory of window: " + winName;
        ofstr.open("del_win_logs.txt", std::ios::out);
        ofstr << e;
        ofstr.close();
        /* Callback aborted means that the transfer was paused from the progress functor */
    }

    win = newwin(newSz.row, newSz.col, newSz.begy, newSz.begx);
    if (win == NULL) {
        std::ofstream ofstr;
        std::string e = "Error while allocating memory for window: " + winName;
        ofstr.open("new_win_logs.txt", std::ios::out);
        ofstr << e;
        ofstr.close();
    }
}

void cursesWindow::printInMiddle(int starty, int startx, int width, std::string str, chtype color)
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

void cursesWindow::printAtTop(int starty, int startx, int width, std::string str, chtype color)
{

}

/* TODO - pass pointers */
point cursesWindow::getBegyx()
{
    int by, bx;
    getbegyx(win, by, bx);
    return point {.y = by, .x = bx};
}

/* TODO - pass pointers */
point cursesWindow::getMaxyx()
{
    int my, mx;
    getmaxyx(win, my, mx);
    return point {.y = my, .x = mx};
}

void cursesWindow::addStr(int y, int x, std::string str)
{
    mvwaddstr(win, y, x, str.c_str());
}

void cursesWindow::drawBox(int y, int x)
{
    box(win, y, x);
}

void cursesWindow::keyPad(bool b)
{
    keypad(win, b);
}

