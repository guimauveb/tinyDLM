#include "../source/helper/WinSize.cxx"
#include "../source/helper/point.cxx"
#include "../../Log/include/Log.hxx"

#include <ncurses.h>

#include <string>
#include <iostream>
#include <thread>

class CursesWindow 
{
    public:
        CursesWindow(int r, int c, int by, int bx, std::string name);
        ~CursesWindow();

        WINDOW *getRawPtr();

        /* Basic wrappers around curses functions */
        void touchWin();
        void eraseWin();
        void refreshWin();
        void resetWin();

        // TODO - Store multiple sub/der windows
        void setDerwin(int nl, int nc, int by, int bx);
        WINDOW * getDerwin();

        point getBegyx();
        point getMaxyx();

        void winAttrOn(chtype ct);
        void winAttrOff(chtype ct);

        /* mvaddstr */
        void addStr(int y, int x, std::string str);
        void drawBox(int y, int x);

        /* Pass row / col etc */
        void printInMiddleWithBackground(int y, int x, int str_begin, const std::string& str, chtype color);
        void printInMiddle(int starty, int startx, int width, std::string str, chtype color);
        void wMove(int y, int x);
        void clearToEOL();

        /* Important ! Will basically call destroyWin() then initWin() */
        void resizeWin(WinSize new_size);
        void keyPad(bool b);

    private:
        /* We'll use the same pointer every time the window is resized (deleted then init) */
        WINDOW *win = nullptr;
        // TODO - Use a vector of derwins if needed  
        /* Derwin that can be used for a menu */
        WINDOW *der = nullptr;
        /* Save all parameters we got from the initialisation so we can resize it */
        int row, col, begy, begx;
        std::string win_name;
};

