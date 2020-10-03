#include "../source/helper/winSize.cxx"
#include "../source/helper/point.cxx"

#include <ncurses.h>

#include <string>
#include <iostream>
#include <thread>

class cursesWindow 
{
    public:
        cursesWindow(int r, int c, int by, int bx, std::string name);
        ~cursesWindow();

        WINDOW *getRawPtr();

        /* Basic wrappers around curses functions */
        void touchWin();
        void eraseWin();
        void refreshWin();
        void resetWin();

        point getBegyx();
        point getMaxyx();

        void winAttrOn(chtype ct);
        void winAttrOff(chtype ct);

        /* mvaddstr */
        void addStr(int y, int x, std::string str);
        void drawBox(int y, int x);

        /* Pass row / col etc */
        void printInMiddle(int starty, int startx, int width, std::string str, chtype color);
        void printAtTop(int starty, int startx, int width, std::string str, chtype color);
        void wMove(int y, int x);
        void clearToEOL();

        /* Important ! Will basically call destroyWin() then initWin() */
        void resizeWin(winSize newSz);
        void keyPad(bool b);

    private:
        /* We'll use the same pointer every time the window is resized (deleted then init) */
        WINDOW *win = nullptr;
        /* Save all parameters we got from the initialisation so we can resize it */
        int row, col, begy, begx;
        std::string winName;
};
