#include "../include/CursesWindow.hxx"
//#include <fstream>

CursesWindow::CursesWindow(int r, int c, int by, int bx, std::string name)
    :row{r}, col{c}, begy{by}, begx{bx}
{
    winName = name;
    //    std::ofstream ofstr;
    //    std::string e = "Allocating memory for window " + winName;
    //    ofstr.open("newwin.txt", std::ios::app);
    //    ofstr << e << '\n';
    //    ofstr.close();
    if ((win = newwin(row, col, begy, begx)) == NULL) {
        //        std::ofstream ofstr;
        //        std::string e = "Error while allocating memory for window: " + winName;
        //        ofstr.open("newwin_log.txt", std::ios::out);
        //        ofstr << e;
        //        ofstr.close();
    }
    wrefresh(win);
}

CursesWindow::~CursesWindow()
{
    //    std::ofstream ofstr;
    //    std::string e = "Freeing memory of window " + winName;
    //    ofstr.open("del_win_log.txt", std::ios::out);
    //    ofstr << e << '\n';
    //    ofstr.close();
    if (win != nullptr) {
        delwin(win);
    }
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
    //werase(win);
    //wresize(win, new_size.row, new_size.col);

    if (win != NULL) {
        delwin(win);
        //        int d = delwin(win);
        //        if (d == ERR) {
        //            std::ofstream ofstr;
        //            std::string e = "Error while freeing memory of window " + winName;
        //            ofstr.open("del_win_log.txt", std::ios::out);
        //            ofstr << e << '\n';
        //            ofstr.close();
        //        }
    }

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

void CursesWindow::printAtTop(int starty, int startx, int width, std::string str, chtype color)
{

}

/* TODO - pass pointers */
point CursesWindow::getBegyx()
{
    int by, bx;
    getbegyx(win, by, bx);
    return point {.y = by, .x = bx};
}

/* TODO - pass pointers */
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
