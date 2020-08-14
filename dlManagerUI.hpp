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

        /* Display a welcome message at first start */
        void firstStart();
        /* Window holding the welcome message at first start */
        std::unique_ptr<cursesWindow> welcomeWinInit();

        void resizeUI();

        /* Initialize program's main windows */
        void mainWinsInit();
        std::unique_ptr<cursesWindow> mainWinTopBarInit();
        std::unique_ptr<cursesWindow> mainWinLabelsInit();
        std::unique_ptr<cursesWindow> mainWinMainInit();
        std::unique_ptr<cursesWindow> mainWinKeyActInit();
        std::unique_ptr<cursesWindow> mainWinDlInfosInit();
        std::unique_ptr<cursesWindow> mainWinDownloadsStatusInit();

        std::unique_ptr<cursesWindow> detWin; 
        void initDetWin();

        /* Draw what needs to be drawn in main windows */
        void paintWelWin(std::unique_ptr<cursesWindow>& welWin);
        void paintTopWin(std::unique_ptr<cursesWindow>& topWin);
        void paintLabelsWin(std::unique_ptr<cursesWindow>& labelsWin);
        void paintMainWinWin(std::unique_ptr<cursesWindow>& mainWinWin);
        void paintDlsStatusWin(std::unique_ptr<cursesWindow>& dlsStatusWin);
        void populateStatusWin(std::vector<downloadWinInfo>& vec);
        void paintKeyActWin(std::unique_ptr<cursesWindow>& keyActWin);

        void refreshMainWins();

        /* TODO - move to the designated function */
        int row = 0, col = 0;

        /* Initialized by the constructor */
        std::unique_ptr<dlManagerController> dlManagerControl;
        /* Main 5 windows */
        std::vector<std::unique_ptr<cursesWindow>> mainWindows;

        /* Start / stop downloads status update */
        void startStatusUpdate();
        int stopStatusUpdate();

        /* TODO - unique_ptr */
        void startProgressBarThread(std::string& filename);       
        int stopProgressBarThread();

        /* Menu holding downloads items */
        void initDownloadsMenu();
        void initDownloadsMenu(std::vector<std::string> itemsData);
        std::unique_ptr<cursesMenu> menu;
        void setDownloadsMenu();

        /* Update message displayed in the key act window */
        void updateKeyActWinMessage();

        /* Update status window in a designated thread */
        void updateDownloadsStatusWindow();
        void updateDownloadsMenu(std::vector<std::string> vec);

        void updateKeyActWinMessage(bool& p);
        /* "Add a new download" routine */
        /* Function that will call all the needed functions to create a new "Add a new dl" window */
        /* Program's subwindows */
        void addNewDl();
        std::unique_ptr<cursesWindow> addDlInitWin();
        void paintAddDlWin();
        void setAddDlForm();
        /* Navigation */
        void addDlNav();

        /* Details window routine */
        void showDetails(std::string itemName);
        void setDetForm();
        std::unique_ptr<cursesForm> detForm;
        std::unique_ptr<cursesForm> addDlForm;
        std::unique_ptr<cursesWindow> addDlWin;
        void populateForm(std::string filename);
        void paintDetWin(std::string& itemName);
        void resizeAddDlNav(std::string url, std::string filename);
        std::unique_ptr<cursesForm> initAddDlForm(int numFields);
        void initDetForm(int numFields);
        std::string initDetailsTitle(std::string itemName);
        void detNav(std::string filename);
        void resetDetWin(std::string filename);
        std::unique_ptr<cursesWindow> initProgressWin(point begyx, point maxyx);
        /* Download progress bar updated in a separate thread */
        void progressBar(std::shared_ptr<cursesWindow> progressWin, std::string filename);

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

        /* Bool signaling if the status window should be updated -> set to true when naviagating the download
         * list window, otherwise set to false */
        bool updateStatus = false;

        /* future that will execute the status update function */
        std::future<void> futureUpdateDlsStatus;
        std::future<void> futureProgressBar;

        /* Signals to stop refreshing progress subwindow */
        bool progRef = false;

        /* Mutex to keep each thread accessing wrefresh() at the same time and causing trouble */
        static std::mutex dlsInfoMutex;
        static std::mutex dlProgMutex;

        /* TODO - move to msg / const / helper */
        const int lowSpeedLim = 56;
        const int lowSpeedTim = 60;

        const int topWinIdx = 0;
        const int labelsWinIdx = 1;
        const int dlsWinIdx = 2;
        const int dlsStatusWinIdx = 3;
        const int keyActWinIdx = 4;
};

