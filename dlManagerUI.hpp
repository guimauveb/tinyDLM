#include "dlManagerController/dlManagerController.hpp"
/* cursesMenu.hpp includes cursesWindow and cursesForm */
/* TODO - use header protections (?) */
#include "UIHelpers/cursesMenu.hpp"

/* TODO - is there a way to avoid a macro in this case ? */
#define CTRL(c) ((c) & 037)

/* TODO - check if I can split window kinds into classes */
/* Class defining a dlManager curses interface */
class dlManagerUI 
{
    public:
        /* TODO - define what to initialize with the constructor */
        /* Generate main windows + subwindows */
        dlManagerUI();

        /* TODO - define what to free when the destructor is called */
        ~dlManagerUI();

        /* Initialize what corresponds to our main function in the previous version */
        void Browser();

    private:
        /* Set the ground for curses */
        void initCurses();
        void initColors();
        void setWinsSize();

        /* Window holding the welcome message at first start */
        std::unique_ptr<cursesWindow> welcomeWinInit();
        void paintWelWin(std::unique_ptr<cursesWindow>& welWin);
        /* Display a welcome message at first start */
        void firstStart();

        /* Initialize program's main windows */
        void mainWinsInit();
        void refreshMainWins();
        void resizeUI();
        std::unique_ptr<cursesWindow> mainWinTopBarInit();
        void paintTopWin(std::unique_ptr<cursesWindow>& topWin);
        std::unique_ptr<cursesWindow> mainWinLabelsInit();
        void paintLabelsWin(std::unique_ptr<cursesWindow>& labelsWin);
        std::unique_ptr<cursesWindow> mainWinMainInit();
        void paintMainWinWin(std::unique_ptr<cursesWindow>& mainWinWin);
        std::unique_ptr<cursesWindow> mainWinKeyActInit();
        void paintKeyActWin(std::unique_ptr<cursesWindow>& keyActWin);
        
        /* Update message displayed in the key act window */
        void updateKeyActWinMessage(bool& p);
        std::unique_ptr<cursesWindow> mainWinDownloadsStatusInit();

        /* Menu holding downloads items */
        std::unique_ptr<cursesMenu> menu;
        std::unique_ptr<cursesMenu> initDownloadsMenu(std::vector<std::string> itemsData);
        void setDownloadsMenu();
        void paintDlsStatusWin(std::unique_ptr<cursesWindow>& dlsStatusWin);
        void populateStatusWin(const std::vector<downloadWinInfo> vec);
        /* Update status window in a designated thread */
        void updateDownloadsStatusWindow();
        void updateDownloadsMenu(std::vector<std::string> vec);
        //std::unique_ptr<cursesWindow> mainWinDlInfosInit();

        /* Main windows are then moved into a vector */
        std::vector<std::unique_ptr<cursesWindow>> mainWindows;

        /* Initialized by the constructor */
        std::unique_ptr<dlManagerController> dlManagerControl;

        /* TODO - move to the designated function */
        int row = 0, col = 0;

        /* Start / stop downloads status update thread */
        void startStatusUpdate();
        int stopStatusUpdate();

        /* Program's subwindows - 'Add a download' window / 'Details' window - 'Progress bar' window  */

        /* "Add a new download" routine */
        /* Function that will call all the needed functions to create a new "Add a new dl" window */
        std::unique_ptr<cursesWindow> addDlWin;
        std::unique_ptr<cursesWindow> addDlInitWin();
        std::unique_ptr<cursesForm> addDlForm;
        std::unique_ptr<cursesForm> initAddDlForm(int numFields);
        void addNewDl();
        void paintAddDlWin();
        void setAddDlForm();

        /* Navigate through the 'Add a download' subwindow */
        void addDlNav();
        void populateForm(const std::string filename);
        void resizeAddDlNav(std::string url, std::string filename);

        /* Details window routine */
        void showDetails(const std::string itemName);
        std::unique_ptr<cursesWindow> detWin; 
        std::unique_ptr<cursesWindow> initDetWin();
        void paintDetWin(const std::string itemName);
        std::unique_ptr<cursesForm> detForm;
        std::unique_ptr<cursesForm> initDetForm(int numFields);
        void setDetForm();

        const std::string initDetailsTitle(const std::string itemName);
        void detNav(const std::string filename);
        int resizeDetWin(const std::string filename);

        /* Download progress bar updated in a separate thread */
        std::unique_ptr<cursesWindow> initProgressWin(point bx, point mx);
        void progressBar(const std::string filename);
        void startProgressBarThread(const std::string filename);       
        void displayProgress(point& maxyx, const std::string percent, const std::string bar);
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

        /* Mutex to keep each thread accessing wrefresh() at the same time and causing trouble */
        static std::mutex dlsInfoMutex;
        static std::mutex dlProgMutex;

        /* TODO - move to msg / const / helper */
        const int lowSpeedLim = 56;
        const int lowSpeedTim = 60;

        /* TODO - enum class */
        const int topWinIdx = 0;
        const int labelsWinIdx = 1;
        const int dlsWinIdx = 2;
        const int dlsStatusWinIdx = 3;
        const int keyActWinIdx = 4;
};

