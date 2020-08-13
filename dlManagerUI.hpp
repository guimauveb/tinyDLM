#include "dlManagerController/dlManagerController.hpp"
/* cursesMenu.hpp includes cursesWindow and cursesForm */
/* TODO - use header protections (?) */
#include "UIHelpers/cursesMenu.hpp"
#include <iostream>

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
        /* TODO - return a struct of winSizes */
        void resizeUI();

        /* Initialized by the constructor */
        std::unique_ptr<dlManagerController> dlManagerControl;
        /* Main 5 windows */
        std::vector<std::unique_ptr<cursesWindow>> mainWindows;
        /* Holds current active subwins -> used to know what window needs to be resized if need */
        std::vector<std::unique_ptr<cursesWindow>> activeSubwins;
        
        /* Window holding the welcome message at first start */
        std::unique_ptr<cursesWindow> welcomeWinInit();

        void startStatusUpdate();
        int stopStatusUpdate();

        void startProgressBarThread(std::shared_ptr<cursesWindow> detWin, std::string& filename);       
        int stopProgressBarThread();

        /* Initialize program's main windows */
        void mainWinsInit();
        void refreshMainWins();
        std::unique_ptr<cursesWindow> mainWinTopBarInit();
        std::unique_ptr<cursesWindow> mainWinLabelsInit();
        std::unique_ptr<cursesWindow> mainWinMainInit();
        std::unique_ptr<cursesWindow> mainWinKeyActInit();
        std::unique_ptr<cursesWindow> mainWinDlInfosInit();
        std::unique_ptr<cursesWindow> mainWinDownloadsStatusInit();

        /* Program's subwindows */
        std::unique_ptr<cursesWindow> addDlInitWin();
        std::shared_ptr<cursesWindow> initDetWin();

        /* Draw what needs to be drawn in main windows */
        void paintWelWin(std::unique_ptr<cursesWindow>& welWin);
        void paintTopWin(std::unique_ptr<cursesWindow>& topWin);
        void paintLabelsWin(std::unique_ptr<cursesWindow>& labelsWin);
        void paintMainWinWin(std::unique_ptr<cursesWindow>& mainWinWin);
        void paintDlsStatusWin(std::unique_ptr<cursesWindow>& dlsStatusWin);
        void populateStatusWin(std::vector<downloadWinInfo>& vec);
        void paintKeyActWin(std::unique_ptr<cursesWindow>& keyActWin);

        /* Draw what needs to be drawn in subwindows */
        void paintDetWin(std::shared_ptr<cursesWindow> detWin, std::string& itemName);
        std::string initDetailsTitle(std::string itemName);
        void paintAddDlWin(std::shared_ptr<cursesWindow> addDlWin);

        /* Menu holding downloads items */
        void initDownloadsMenu();
        void initDownloadsMenu(std::vector<std::string> itemsData);
        std::shared_ptr<cursesMenu> menu;
        void setDownloadsMenu();

       /* Update message displayed in the key act window */
        void updateKeyActWinMessage();
        /* Update status window in a designated thread */
        void updateDownloadsStatusWindow();
        void updateDownloadsMenu(std::vector<std::string> vec);
        /* "Add a new download" routine */
        /* Function that will call all the needed functions to create a new "Add a new dl" window */
        void addNewDl();
        void setAddDlForm(std::shared_ptr<cursesForm> form, std::shared_ptr<cursesWindow> win);
        /* Navigation */
        void addDlNav(std::shared_ptr<cursesWindow> win, std::shared_ptr<cursesForm> form);

        /* Details window routine */
        void showDetails(std::string itemName);
        void setDetForm(std::shared_ptr<cursesWindow> win, std::shared_ptr<cursesForm> form);
        void populateForm(std::unique_ptr<cursesForm> form, std::string filename);
        void detNav(std::shared_ptr<cursesWindow> win, std::shared_ptr<cursesForm> form, std::string filename);
        void resetDetWin(std::shared_ptr<cursesWindow> win, std::shared_ptr<cursesForm> form,
        std::string filename);

        /* Initialize a form object by passing the number of fields desired */
        std::shared_ptr<cursesForm> initFormShared(int numFields);
        std::unique_ptr<cursesForm> initForm(int numFields);
        std::shared_ptr<cursesWindow> initProgressWin(point begyx, point maxyx);
                
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

        /* Progress bar updated in a separate thread */
        void progressBar(std::shared_ptr<cursesWindow> progressWin, std::string filename);

        void updateDownloadsList();

        /* Bool signaling if the status window should be updated -> set to true when naviagating the download
         * list window, otherwise set to false */
        bool updateStatus = false;

        /* future that will execute the status update function */
        std::future<void> ft;
        std::future<void> ftP;

        /* Vector containing the threading updating the dl status window */
        /* TODO - use a future instead */
        std::vector<std::thread> progVec;

        /* Save cursor x and y locations */
        /* TODO - move to the designated function */
        int row = 0, col = 0;

        /* Signals to stop refreshing progress subwindow */
        /* See if the fact that the progress thread is checking the state of this bool while the nav 
         * can change it at the same time is a problem */
        bool progRef = false;

        /* Signals that transfer is complete and stop refreshing progress subwindow */
        /* Mutex to keep each thread accessing wrefresh() at the same time and causing trouble */
        /* Make it static since it there will be only one instance of the curses interface */
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

