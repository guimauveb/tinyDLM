#include "../../helper/include/futureState.hpp"
#include "../../helper/include/userInputCheck.hpp"
#include "../../dialogs/include/dialogs.hpp"

#include "../../dlManagerController/include/dlManagerController.hpp"

/* cursesMenu.hpp includes cursesWindow and cursesForm */
#include "../../curses/include/cursesMenu.hpp"

/* TODO - is there a way to avoid a macro in this case ? */
#define CTRL(c) ((c) & 037)

/* Class defining a dlManager curses interface */
class dlManagerUI 
{
    public:
        /* TODO - define what to initialize with the constructor */
        /* Generate main windows + subwindows */
        dlManagerUI();

        /* TODO - define what to free when the destructor is called */
        ~dlManagerUI();

        /* Initialized by the constructor */
        std::unique_ptr<dlManagerController> dlManagerControl;

        /* Main windows are stored into a vector */
        std::vector<std::unique_ptr<cursesWindow>> mainWindows;
        void resizeUI();
        void setWinsSize();
        void refreshMainWins();

        /* Display a welcome message at first start */
        int firstStart();

        /* Start / stop downloads status update thread */
        void startStatusUpdate();
        int stopStatusUpdate();
        void statusDriver(int c);
        void resetStatusDriver();

        /* yOffset corresponds to the download item at which the status window should begin when the menu is 
         * split into pages */
        int yOffset;
        /* Corresponds to the current highlighted item */
        int currItNo;

        /* Display help window */
        void showHelp();
        /* 'Add a new download' routine */
        int addNewDl();
        /* Details window routine */
        int showDetails(const std::string& itemName);

        /* Menu holding downloads items */
        std::unique_ptr<cursesMenu> menu;
        
        void updateDownloadsMenu();

        /* TODO - enum class */
        const int topWinIdx = 0;
        const int labelsWinIdx = 1;
        const int dlsWinIdx = 2;
        const int dlsStatusWinIdx = 3;
        const int keyActWinIdx = 4;
        const int dlsInfosWinIdx = 5;
    
        /* Mutexes to keep each thread accessing wrefresh() at the same time and causing trouble */
        std::mutex dlsInfoMutex;
        std::mutex dlProgMutex;
        std::mutex yOffsetMutex;
        
    private:
        /* Set the ground for curses */
        void initCurses();
        void initColors();
        void initStatusDriver();

        /* Display tinyDLM help */
        std::unique_ptr<cursesWindow> initHelpWin();
        void paintHelpWin(std::unique_ptr<cursesWindow>& win);
        void helpNav(std::unique_ptr<cursesWindow>& win);

        /* Window holding the welcome message at first start */
        std::unique_ptr<cursesWindow> welcomeWinInit();
        void paintWelWin(std::unique_ptr<cursesWindow>& welWin);

        /* Initialize program's main windows */
        void mainWinsInit();
        std::unique_ptr<cursesWindow> mainWinTopBarInit();
        void paintTopWin(std::unique_ptr<cursesWindow>& topWin);
        std::unique_ptr<cursesWindow> mainWinLabelsInit();
        void paintLabelsWin(std::unique_ptr<cursesWindow>& labelsWin);
        std::unique_ptr<cursesWindow> mainWinMainInit();
        void paintMainWinWin(std::unique_ptr<cursesWindow>& mainWinWin);
        std::unique_ptr<cursesWindow> mainWinKeyActInit();
        void paintKeyActWin(std::unique_ptr<cursesWindow>& keyActWin);
        std::unique_ptr<cursesWindow> mainWinDlInfosInit();
        void paintDlsInfosWin(std::unique_ptr<cursesWindow>& dlsInfosWin);
        
        std::unique_ptr<cursesWindow> mainWinDownloadsStatusInit();

        std::unique_ptr<cursesMenu> initDownloadsMenu(std::vector<std::string> itemsData);
        void setDownloadsMenu();
        void paintDlsStatusWin(std::unique_ptr<cursesWindow>& dlsStatusWin);
        void populateStatusWin(const std::vector<downloadWinInfo>& vec);
        /* Update status window in a designated thread */
        void updateDownloadsStatusWindow();

        /* TODO - move to the designated function */
        int row = 0, col = 0;

        /* Program's subwindows - 'Add a download' window / 'Details' window - 'Progress bar' window  */

        /* "Add a new download" routine */
        std::unique_ptr<cursesWindow> addDlWin;
        std::unique_ptr<cursesWindow> addDlInitWin();
        std::unique_ptr<cursesForm> addDlForm;
        std::unique_ptr<cursesForm> initAddDlForm(size_t numFields);
        void paintAddDlWin();
        void setAddDlForm();

        /* Navigate through the 'Add a download' subwindow */
        int addDlNav();
        void populateForm(const std::string filename);
        void resizeAddDlNav(std::string url, std::string filename);

        
        std::unique_ptr<cursesWindow> detWin; 
        std::unique_ptr<cursesWindow> initDetWin();
        void paintDetWin(const std::string& itemName);
        std::unique_ptr<cursesForm> detForm;
        std::unique_ptr<cursesForm> initDetForm(size_t numFields);
        void setDetForm();

        const std::string initDetailsTitle(const std::string& itemName);
        int detNav(const std::string& filename);
        int resizeDetWin(const std::string& filename);

        /* Download progress bar updated in a separate thread */
        std::unique_ptr<cursesWindow> initProgressWin(const point& bx, const point& mx);
        void progressBar(const std::string& filename);
        void startProgressBarThread(const std::string& filename);       
        int stopProgressBarThread();

        /* Bool signaling if the status window should be updated -> set to true when naviagating the download
         * list window, otherwise set to false. Used by the status window thread and the main thread so it's
         * protected by a mutex. */
        bool updateStatus = false;

        /* future that will execute the status update function */
        std::future<void> futureUpdateDlsStatus;
        /* future that will execute the progress bar update function */
        std::future<void> futureProgressBar;
        std::vector<std::future<void>> futureVec;
        std::unique_ptr<cursesWindow> progressWin;

        /* TODO - Create a struct on the stack */
        winSize welWinSz;
        winSize topBarSz;
        winSize labelsSz;
        winSize mainWinSz;
        winSize dlStatusSz;
        winSize keyActSz;
        winSize dlInfosSz;
        winSize dlAddSz;
        winSize dlDetSz;
        winSize dlProgSz;

        /* Signals to stop refreshing progress subwindow */
        bool progRef = false;

        /* TODO - make it selectionable by the user */
        const int lowSpeedLim = 56;
        const int lowSpeedTim = 60;

        int topItem, botItem;

};

