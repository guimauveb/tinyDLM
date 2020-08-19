#include "winSize.cpp"
#include "point.cpp"

#include <ncurses.h>

#include <string>

class cursesWindow 
{
    public:
        /* What can we do with a window ? */
        cursesWindow(int r, int c, int by, int bx);
        ~cursesWindow();

        WINDOW *getRawPtr();
        /* Basic wrappers around curses functions */
        void touchWin();
        void eraseWin();
        void refreshWin();
        void resetWin();

        point getBegyx();
        point getMaxyx();
        int getMaxy();
        int getMaxx();

        void winAttrOn(chtype ct);
        void winAttrOff(chtype ct);

        /* mvaddstr */
        void addStr(int y, int x, std::string str);
        void drawBox(int y, int x);
        /* Pass row / col etc */
        void printInMiddle(int starty, int startx, int width, std::string str, chtype color);
        void printAtTop(int starty, int startx, int width, std::string str, chtype color);

        /* Important ! Will basically call destroyWin() then initWin() */
        void resizeWin(winSize& newSz);
        void keyPad(bool b);

    private:
        /* We'll use the same pointer every time the window is resized (deleted then init) */
        WINDOW *win = nullptr;
        /* Save all parameters we got from the initialisation so we can resize it */
        int row, col, begy, begx;
        std::string label;
};

