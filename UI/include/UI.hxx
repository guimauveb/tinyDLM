#include "../../helper/include/futureState.hxx"
#include "../../helper/include/userInputCheck.hxx"
#include "../../dialogs/include/dialogs.hxx"

#include "../../Controller/include/Controller.hxx"

/* CursesMenu header includes CursesWindow and CursesForm */
#include "../../Curses/include/CursesMenu.hxx"

#define CTRL(c) ((c) & 037)

/* Class defining a download manager curses interface */
class UI 
{
    public:
        /* Generate main windows + subwindows */
        UI();
        ~UI();

        /* Initialized by the constructor */
        std::unique_ptr<Controller> controller;
        std::unique_ptr<Settings> settings;

        /* Main windows are stored into a vector */
        std::vector<std::unique_ptr<CursesWindow>> main_windows;
        void resizeUI();
        void setWindowsSize();
        void refreshMainWindows();

        /* Display a welcome message at first start */
        int firstStart();

        /* Start / stop downloads status update thread */
        void startStatusUpdate();
        int stopStatusUpdate();
        void statusDriver(int c);
        void resetStatusDriver();

        /* y_offset refers to the download item at which the status window should begin when the menu is 
         * split into pages */
        int y_offset;
        /* Refers to the current highlighted item in the downloads status window */
        int current_item_number;
        int top_item;
        int bottom_item;

        /* Enter settings menu */
        int showSettings();

        /* Display help window */
        int showHelp();
        /* 'Add a new download' routine */
        int addNewDownload();
        /* Details window routine */
        int showDownloadDetails(const std::string& item_name);

        /* Menu holding downloads items */
        std::unique_ptr<CursesMenu> menu;

        void updateDownloadsMenu();

        /* TODO - enum class */
        const int top_bar_window_index = 0;
        const int labels_window_index = 1;
        const int main_window_index = 2;
        const int status_window_index = 3;
        const int show_help_window_index = 4;
        const int infos_window_index = 5;

        /* Mutexes to keep each thread from accessing wrefresh() at the same time and causing trouble */
        std::mutex downloads_infos_window_mutex;
        std::mutex download_progress_window_mutex;
        std::mutex y_offset_mutex;

    private:
        /* Initialize user settings */
        void initSettings();

        /* Set the ground for curses */
        void initCurses();
        void initColors();
        void initStatusDriver();

        /* Display tinyDLM help */
        std::unique_ptr<CursesWindow> help_window;
        void paintHelpWindow(std::unique_ptr<CursesWindow>& win);
        int navigateHelpWindow();

        /* Display settings */
        std::unique_ptr<CursesWindow> settings_window;
        std::unique_ptr<CursesForm> settings_form;
        std::unique_ptr<CursesMenu> settings_menu;
        int navigateSettings();
        void setSettingsForm();
        void setSettingsMenu();

        /* Window holding the welcome message at first start */
        void paintWelcomeWindow(std::unique_ptr<CursesWindow>& win);

        /* Initialize program's main windows */
        void initMainWindows();

        /* All windows are initialized by initWin() */
        std::unique_ptr<CursesWindow> initWin(WinSize& size, const std::string& name);

        void paintTopWindow(std::unique_ptr<CursesWindow>& win);
        void paintLabelsWindow(std::unique_ptr<CursesWindow>& win);
        //void paintMainWindow(std::unique_ptr<CursesWindow>& win);
        void paintShowHelpWindow(std::unique_ptr<CursesWindow>& win);
        void paintSettingsWindow(std::unique_ptr<CursesWindow>& win);
        //void paintDownloadsInfosWindow(std::unique_ptr<CursesWindow>& win);

        std::unique_ptr<CursesMenu> initMenu(std::vector<std::string> items_data);
        void setDownloadsMenu();
        //void paintDownloadsStatusWin(std::unique_ptr<CursesWindow>& win);
        void populateStatusWin(const std::vector<DownloadWinInfo>& vec);
        /* Update status window in a designated thread */
        void updateDownloadsStatusWindow();
        /* Bool signaling if the status window should be updated -> set to true when naviagating the download
         * list window, otherwise set to false. Used by the status window thread and the main thread so it's
         * protected by a mutex. */
        bool update_status = false;

        /* TODO - move to the designated function */
        int row = 0, col = 0;

        /* Program's subwindows - 'Add a download' window / 'Details' window - 'Progress bar' window  */
        std::unique_ptr<CursesForm> initForm(size_t num_fields);

        /* "Add a new download" routine */
        std::unique_ptr<CursesWindow> add_dl_win;
        std::unique_ptr<CursesWindow> add_dl_menu_win;
        std::unique_ptr<CursesForm> add_dl_form;
        std::unique_ptr<CursesMenu> add_dl_menu;
        void paintAddDownloadWin();
        void setAddDownloadForm();
        void setAddDownloadMenu();

        /* Navigate through the 'Add a download' subwindow */
        int navigateAddDownloadWindow();
        void populateForm(const std::string filename);
        void resizeAddDownloadWindow(std::string url, std::string filename);

        /* Details window routine */
        std::unique_ptr<CursesWindow> det_win; 
        void paintDetWin(const std::string& item_name);
        std::unique_ptr<CursesForm> det_form;
        void setDetForm();

        const std::string initDownloadDetailsTitle(const std::string& item_name);
        int navigateDownloadDetailsWindow(const std::string& filename);
        int resizeDownloadDetailsWindow(const std::string& filename);

        /* Download progress bar updated in a separate thread */
        std::unique_ptr<CursesWindow> progress_win;
        void progressBar(const std::string& filename);
        void startProgressBarThread(const std::string& filename);       
        int stopProgressBarThread();
        /* Signals to stop refreshing progress subwindow */
        bool refresh_progress = false;

        /* future that will execute the status update function */
        std::future<void> future_update_downloads_status;
        /* future that will execute the progress bar update function */
        std::future<void> future_progress_bar;

        /* Map holding window sizes */
        std::map<const std::string, WinSize> window_size_map;

        /* TODO - make it selectionable by the user */
        const int low_speed_limit = 56;
        const int low_speed_time_limit = 60;
};

