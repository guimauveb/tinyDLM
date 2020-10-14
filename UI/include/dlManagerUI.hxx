#include "../../helper/include/futureState.hxx"
#include "../../helper/include/userInputCheck.hxx"
#include "../../dialogs/include/dialogs.hxx"

#include "../../dlManagerController/include/dlManagerController.hxx"

/* cursesMenu.hxx includes cursesWindow and cursesForm */
#include "../../curses/include/cursesMenu.hxx"

/* TODO - is there a way to avoid a macro in this case ? */
#define CTRL(c) ((c) & 037)

/* Class defining a dlManager curses interface */
class dlManagerUI 
{
    public:
        /* Generate main windows + subwindows */
        dlManagerUI();

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

        /* yOffset refers to the download item at which the status window should begin when the menu is 
         * split into pages */
        int yOffset;
        /* Refers to the current highlighted item in the downloads status window */
        int currItNo;

        int topItem;
        int botItem;

        /* Display help window */
        int showHelp();
        /* 'Add a new download' routine */
        int addNewDl();
        /* Details window routine */
        int showDetails(const std::string& itemName);

        /* Menu holding downloads items */
        std::unique_ptr<cursesMenu> menu;

        void updateDownloadsMenu();

        /* TODO - enum class */
        const int topBarIdx = 0;
        const int labelsIdx = 1;
        const int mainIdx = 2;
        const int statusIdx = 3;
        const int pHelpIdx = 4;
        const int infosIdx = 5;

        /* Mutexes to keep each thread from accessing wrefresh() at the same time and causing trouble */
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
        int helpNav();

        /* Window holding the welcome message at first start */
        std::unique_ptr<cursesWindow> helpWin;
        void paintWelWin(std::unique_ptr<cursesWindow>& welWin);

        /* Initialize program's main windows */
        void initMainWins();

        /* All windows are initialized by initWin() */
        std::unique_ptr<cursesWindow> initWin(winSize& sz, const std::string& name);

        void paintTopWin(std::unique_ptr<cursesWindow>& topWin);
        void paintLabelsWin(std::unique_ptr<cursesWindow>& labelsWin);
        void paintMainWinWin(std::unique_ptr<cursesWindow>& mainWinWin);
        void paintKeyActWin(std::unique_ptr<cursesWindow>& keyActWin);
        void paintDlsInfosWin(std::unique_ptr<cursesWindow>& dlsInfosWin);

        std::unique_ptr<cursesMenu> initDownloadsMenu(std::vector<std::string> itemsData);
        void setDownloadsMenu();
        void paintDlsStatusWin(std::unique_ptr<cursesWindow>& dlsStatusWin);
        void populateStatusWin(const std::vector<downloadWinInfo>& vec);
        /* Update status window in a designated thread */
        void updateDownloadsStatusWindow();
        /* Bool signaling if the status window should be updated -> set to true when naviagating the download
         * list window, otherwise set to false. Used by the status window thread and the main thread so it's
         * protected by a mutex. */
        bool updateStatus = false;

        /* TODO - move to the designated function */
        int row = 0, col = 0;

        /* Program's subwindows - 'Add a download' window / 'Details' window - 'Progress bar' window  */

        std::unique_ptr<cursesForm> initForm(size_t numFields);

        /* "Add a new download" routine */
        std::unique_ptr<cursesWindow> addDlWin;
        std::unique_ptr<cursesForm> addDlForm;
        std::unique_ptr<cursesMenu> addDlMenu;
        void paintAddDlWin();
        void setAddDlForm();
        void setAddDlMenu();

        /* Navigate through the 'Add a download' subwindow */
        int addDlNav();
        void populateForm(const std::string filename);
        void resizeAddDlNav(std::string url, std::string filename);

        /* Details window routine */
        std::unique_ptr<cursesWindow> detWin; 
        void paintDetWin(const std::string& itemName);
        std::unique_ptr<cursesForm> detForm;
        void setDetForm();

        const std::string initDetailsTitle(const std::string& itemName);
        int detNav(const std::string& filename);
        int resizeDetWin(const std::string& filename);

        /* Download progress bar updated in a separate thread */
        std::unique_ptr<cursesWindow> progressWin;
        void progressBar(const std::string& filename);
        void startProgressBarThread(const std::string& filename);       
        /* Signals to stop refreshing progress subwindow */
        bool progRef = false;
        int stopProgressBarThread();

        /* future that will execute the status update function */
        std::future<void> futureUpdateDlsStatus;
        /* future that will execute the progress bar update function */
        std::future<void> futureProgressBar;

        /* Map holding window sizes */
        std::map<const std::string, winSize> winSizeMap;

        /* TODO - make it selectionable by the user */
        const int lowSpeedLim = 56;
        const int lowSpeedTim = 60;
};

