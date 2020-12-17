#include "../include/UI.hxx"

UI::UI()
    :controller{std::make_unique<Controller>()}
{
    initCurses();
    initColors();
    setWindowsSize();
    initMainWindows();
    updateDownloadsMenu();
    initStatusDriver();
    refreshMainWindows();        
}

/* Free memory and exit curses */
UI::~UI()
{
    stopStatusUpdate();
    menu->clearMenu();
    menu->clearItems();
    /* Pause all downloads -> we use pause to stop them since it does the same thing */
    controller->killAll();
    /* Delete main windows */
    main_windows.clear();
    /* TODO - save downloads list */
    endwin();
}

/* Initialize curses */
void UI::initCurses()
{
    setlocale(LC_ALL, "");
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
}

/* Initialize all the colors and color pairs */
void UI::initColors()
{
    start_color();
    use_default_colors();

    init_pair(1, COLOR_RED, COLOR_WHITE);
    init_pair(2, COLOR_GREEN, -1);
    init_pair(3, COLOR_BLUE, -1);
    init_pair(4, COLOR_CYAN, COLOR_WHITE);
    init_pair(7, COLOR_WHITE, -1);
    init_pair(8, COLOR_BLACK, COLOR_WHITE);

    /* Download status colors */
    init_pair(9, -1, COLOR_GREEN);
    init_pair(10, -1, COLOR_YELLOW);
    init_pair(11, -1, COLOR_RED);

    /* Highlighted download status colors */
    init_pair(12, COLOR_GREEN, COLOR_WHITE);
    init_pair(13, COLOR_YELLOW, COLOR_WHITE);
    init_pair(14, COLOR_RED, COLOR_WHITE);
    /* Progress bar color pair */
    init_pair(16, COLOR_GREEN, COLOR_GREEN);
}

void UI::setWindowsSize()
{
    /* Get terminal screen size in rows and columns */
    int nrow = 0, ncol = 0;

    getmaxyx(stdscr, nrow, ncol);
    /* Program min diemnsions */
    if (nrow >= 26) {
        row = nrow;
    }
    else {
        row = 26;
    }
    if (ncol >= 112) {
        col = ncol;
    }
    else {
        col = 112;
    }

    window_size_map["welSz"]      = {row - 8, col, 4, 0};
    window_size_map["helpSz"]     = {18, col - (col / 2), (row / 4), col / 4};
    window_size_map["topBarSz"]   = {1, col, 0, 0};
    window_size_map["labelsSz"]   = {1, col, 2, 0};
    window_size_map["mainWinSz"]  = {row - 4, col / 2, 4, 0};
    window_size_map["statusSz"]   = {row - 5, col / 2, 3, col / 2};
    window_size_map["pHelpSz"]    = {1, col / 2, row - 1, 0};
    window_size_map["infosSz"]    = {1, col / 2, row - 2, col / 2};
    window_size_map["addSz"]      = {row / 2 + 1, col - (col / 2), (row / 4), col / 4};
    window_size_map["detSz"]      = {row / 2, col - (col / 2), (row / 4), col / 4};
    window_size_map["progSz"]     = {4, (col - (col / 2)) -10, row / 2, col / 4 + 4};

}

void UI::resetStatusDriver()
{
    initStatusDriver();
}

void UI::statusDriver(int c)
{
    point p_max = main_windows.at(3)->getMaxyx();
    current_item_number = menu->getItemNo();

    if (c == KEY_DOWN) {
        std::lock_guard<std::mutex> guard(y_offset_mutex);
        /* Signals to scroll status window down if the menu is scrolled down */
        /* If curr item id is > the maxixmum number of items displayed in the win -> offset */
        if (current_item_number > bottom_item) {
            y_offset = (current_item_number - (p_max.y - 2)); 
            bottom_item++;
            top_item++;
        }
    }
    else if (c == KEY_UP) {
        /* Signals to scroll status window down if the menu is scrolled down */
        if (current_item_number < top_item) { top_item--;
            bottom_item--;
            y_offset--;
        }
    }
}

void UI::initStatusDriver()
{
    point p_max = main_windows.at(3)->getMaxyx();
    top_item = 0;
    bottom_item = p_max.y - 2;
    current_item_number = 0;
    y_offset = 0;
}

std::unique_ptr<CursesWindow> UI::initWin(WinSize& size, const std::string& name)
{
    return std::make_unique<CursesWindow>(size.row, size.col, size.begy, size.begx, name);
}

void UI::paintWelcomeWindow(std::unique_ptr<CursesWindow>& win)
{
    win->printInMiddle(row / 4, 0, col,  tinyDLMWelcome, COLOR_PAIR(7));
    win->printInMiddle(row / 4 + 2, 0, col,  tinyDLMVer, COLOR_PAIR(7));
    win->printInMiddle(row / 4 + 3, 0, col,  madeBy, COLOR_PAIR(7));
    win->printInMiddle(row / 4 + 5, 0, col,  tinyHelp, COLOR_PAIR(7));
}

int UI::firstStart()
{
    std::unique_ptr<CursesWindow> welcome_window = initWin(window_size_map["welSz"], "welcome");
    paintWelcomeWindow(welcome_window);
    welcome_window->refreshWin();

    int ch = 0;
    //bool done = false;
    bool resize = false;
    bool done = false;
    while ((ch = getch()) != KEY_F(1)) {
        switch(ch) {
            case KEY_RESIZE:
                {
                    resize = true;
                    break;
                }
            case 'a':
                {
                    /* Open a window and exit the function */
                    addNewDownload();
                    done = true; 
                    break;
                }
            case 'h':
                {
                    showHelp();
                    done = true;
                    break;
                }
            default:
                {
                    break;
                }
        }
        if (resize) {
            resizeUI();
            welcome_window->resizeWin(window_size_map["welSz"]);
            paintWelcomeWindow(welcome_window);
            welcome_window->refreshWin();
            resize = false;
        }
        if (done) {
            startStatusUpdate();
            break;
        }
    }
    /* If done == false - signal to main that we should terminate the program */
    refreshMainWindows();
    return done;
}

void UI::refreshMainWindows()
{
    main_windows.at(top_bar_window_index)->touchWin();
    main_windows.at(top_bar_window_index)->refreshWin();

    main_windows.at(labels_window_index)->touchWin();
    main_windows.at(labels_window_index)->refreshWin();

    main_windows.at(main_window_index)->touchWin();
    main_windows.at(main_window_index)->refreshWin();

    main_windows.at(status_window_index)->touchWin();
    main_windows.at(status_window_index)->refreshWin();

    main_windows.at(show_help_window_index)->touchWin();
    main_windows.at(show_help_window_index)->refreshWin();

    main_windows.at(infos_window_index)->touchWin();
    main_windows.at(infos_window_index)->refreshWin();

}

/* Resize program when terminal window size is detected */
void UI::resizeUI()
{
    setWindowsSize();

    main_windows.at(top_bar_window_index)->resizeWin(window_size_map["topBarSz"]);
    paintTopWindow(main_windows.at(top_bar_window_index));

    main_windows.at(labels_window_index)->resizeWin(window_size_map["labelsSz"]);
    paintLabelsWindow(main_windows.at(labels_window_index));

    /* Refresh the downloads list */
    menu->clearMenu();
    menu->clearItems();
    main_windows.at(main_window_index)->resizeWin(window_size_map["mainWinSz"]);

    main_windows.at(status_window_index)->resizeWin(window_size_map["statusSz"]);

    main_windows.at(show_help_window_index)->resizeWin(window_size_map["pHelpSz"]);
    paintShowHelpWindow(main_windows.at(show_help_window_index));

    main_windows.at(infos_window_index)->resizeWin(window_size_map["infosSz"]);

    refreshMainWindows();
}

void UI::paintTopWindow(std::unique_ptr<CursesWindow>& topWin)
{
    /* Print a whole black on white row at the top of the main window that will be the size of the terminal
     * window width */
    char *titleMain = (char*)malloc((col + 1)*sizeof(char));
    int i = 0;

    for (i = 0; i < (int)liteDL_label.length(); ++i) {
        titleMain[i] = liteDL_label.at(i);
    }
    for (; i < col; ++i) {
        titleMain[i] = ' ';
    }
    titleMain[i] = '\0';

    topWin->printInMiddle(0, 0, col, titleMain, COLOR_PAIR(8));

    free(titleMain);
}

/* Init main windows */
void UI::initMainWindows()
{

    /* Window containing top bar title */
    main_windows.emplace_back(initWin(window_size_map["topBarSz"], "topBar"));
    paintTopWindow(main_windows.at(top_bar_window_index));

    /* Top window containing labels such as "name", "url","speed" etc  */
    main_windows.emplace_back(initWin(window_size_map["labelsSz"], "labels"));
    paintLabelsWindow(main_windows.at(labels_window_index));

    /* Window containing downloads list as a menu */
    main_windows.emplace_back(initWin(window_size_map["mainWinSz"], "main"));

    /* Window containing ownloads statuses - updated in a separate thread and connected to mainWin */
    main_windows.emplace_back(initWin(window_size_map["statusSz"], "status"));

    /* Winodow displaying help message */ 
    main_windows.emplace_back(initWin(window_size_map["pHelpSz"], "help"));
    paintShowHelpWindow(main_windows.at(show_help_window_index));

    /* Not used for now */
    main_windows.emplace_back(initWin(window_size_map["infosSz"], "infos"));
}

void UI::paintLabelsWindow(std::unique_ptr<CursesWindow>& win)
{
    win->printInMiddle(0, 0, col / 2, labelName, COLOR_PAIR(7));
    win->printInMiddle(0, col / 2, col / 8, labelProgress, COLOR_PAIR(7));
    win->printInMiddle(0, 5 * col / 8, col / 8, labelSpeed, COLOR_PAIR(7));
    win->printInMiddle(0, 3 * col / 4, col / 8, labelETA, COLOR_PAIR(7));
    win->printInMiddle(0, 7 * col / 8, col / 8, labelStatus, COLOR_PAIR(7));
}

//void UI::paintMainWindow(std::unique_ptr<CursesWindow>& win) {}
//void UI::paintDlsStatusWin(std::unique_ptr<CursesWindow>& win) {}
//void UI::paintDownloadsInfosWindow(std::unique_ptr<CursesWindow>& win) {}

void UI::paintShowHelpWindow(std::unique_ptr<CursesWindow>& win)
{
    /* Display keys and their associated functions at the bottom of the window */
    win->printInMiddle(0, 0, col / 4, msgHelp, COLOR_PAIR(7));
}

/* Populate the status window with downloads informations such as their status / speed / progress */
void UI::populateStatusWin(const std::vector<downloadWinInfo>& vec)
{
    int y = 1;
    size_t offset;
    size_t curr;
    point p = main_windows.at(status_window_index)->getMaxyx();
    chtype color = COLOR_PAIR(7);
    /* Download status color */
    chtype stcolor;

    /* Iterate over the list of downloads we obtained from controller */
    /* Start iterating at the offset determined by the highlighted item in the menu */
    {
        std::lock_guard<std::mutex> guard(y_offset_mutex);
        offset = y_offset;
        curr = current_item_number;
    }
    for (offset = y_offset; offset < vec.size(); ++offset) {
        /* Highlight selected item */
        if (curr == offset) {
            color = COLOR_PAIR(8);

            /* Highlight selected completed download in green on white */
            if (vec.at(offset).status == statusStrCd) {
                stcolor = COLOR_PAIR(12);
            }
            /* Highlight selected paused download in yellow on white */
            else if (vec.at(offset).status == statusStrPd) {
                stcolor = COLOR_PAIR(13);
            }
            /* Highlight selected problematic download in red on white */
            else if (vec.at(offset).status == statusStrEr) {
                stcolor = COLOR_PAIR(14);
            }
            else {
                stcolor = COLOR_PAIR(8);
            }
        }

        else   {
            color = COLOR_PAIR(7);
            /* Highlight completed downloads status in green */
            if (vec.at(offset).status == statusStrCd) {
                stcolor = COLOR_PAIR(9);
            }
            /* Highlight paused downloads status in yellow */
            else if (vec.at(offset).status == statusStrPd) {
                stcolor = COLOR_PAIR(10);
            }
            /* Highlight problematic downloads status in red */
            else if (vec.at(offset).status == statusStrEr) {
                stcolor = COLOR_PAIR(11);
            }
            else {
                stcolor = COLOR_PAIR(7);
            }
        }    

        main_windows.at(status_window_index)->printInMiddle(y, 0, p.x / 4, vec.at(offset).progress, color);
        /* Display percent sign */
        main_windows.at(status_window_index)->printInMiddle(y, 0, p.x / 4 + vec.at(offset).progress.length() + 1," %", color);
        main_windows.at(status_window_index)->printInMiddle(y, p.x / 4, p.x / 4, vec.at(offset).speed, color);
        main_windows.at(status_window_index)->printInMiddle(y, p.x / 4, p.x / 4 + vec.at(offset).speed.length() + 2," MBs", color);
        //mainWindow.at(status_window_index)->printInMiddledlsStatusWiny, 2 * width / 4, width / 4, dl.eta, color);
        main_windows.at(status_window_index)->printInMiddle(y, 3 * p.x / 4, p.x / 4, vec.at(offset).status, stcolor);
        y++;
    }
}

void UI::setDownloadsMenu()
{
    point p_max = main_windows.at(main_window_index)->getMaxyx();
    menu->menuOptsOn(O_SHOWDESC);
    menu->setMenuSub(main_windows.at(main_window_index));

    /* Fill the entire window with items. (-2) corresponds to top and bottom borders that we ignore */
    menu->setMenuFormat(p_max.y - 2, 0);
    menu->setMenuMark(" * ");
    menu->postMenu();
}

/* Display downloads as a menu */
std::unique_ptr<CursesMenu> UI::initMenu(std::vector<std::string> items_data)
{
    return std::make_unique<CursesMenu>(items_data);
}

/* Consistantly refresh the speed / progress /status of the current downloads */ 
void UI::updateDownloadsStatusWindow()
{
    while (true) {
        {
            std::lock_guard<std::mutex> guard(downloads_infos_window_mutex);
            if (!update_status) {
                break;
            }
        }
        /* Sleep 100ms before refreshing the window again or the while loop will execute endlessly 
         * so it doesn't monopolize time / ressources */ 
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        /* y represents the y postion of the infos to print on the screen - it matches the location
         * of the corresponding download item on the future_update_downloads_status part of the screen */

        /* Refresh status window */
        {
            std::lock_guard<std::mutex> guard(downloads_infos_window_mutex);
            main_windows.at(status_window_index)->eraseWin();
            populateStatusWin(controller->getAllDownloadsInfos());
            main_windows.at(status_window_index)->refreshWin();
        }
    }
}

void UI::startStatusUpdate()
{
    /* If no downloads - erase the window and break out of here */
    if (!controller->isActive()) {
        std::lock_guard<std::mutex> guard(downloads_infos_window_mutex);
        main_windows.at(status_window_index)->refreshWin();
    }
    else {
        update_status = true;
        future_update_downloads_status = std::async(std::launch::async, &UI::updateDownloadsStatusWindow, this);
    }
}

/* Signals the downloads status update thread to stop */
int UI::stopStatusUpdate()
{
    {
        std::lock_guard<std::mutex> guard(downloads_infos_window_mutex);
        if (!update_status) { return 1; }
        update_status = false;
    }

    /* Wait for the thread to stop before moving on */
    if (!(futureIsReady(future_update_downloads_status))) {
        while (!futureIsReady(future_update_downloads_status)) {
            //wait unitl execution is done
            ;
        }
    } 
    return 0;
}

int UI::showHelp()
{
    help_window = initHelpWindow();
    paintHelpWindow(help_window);
    help_window->refreshWin();
    return navigateHelpWindow();
}

int UI::navigateHelpWindow()
{
    int ch = 0;
    bool done = false;
    bool resize = false;
    bool update_menu = false;
    while ((ch = getch())) {
        switch(ch) {
            case KEY_RESIZE:
                {
                    update_menu = true;
                    resize = true;
                    break;
                }
            case 10:
                {
                    done = true;
                    break;
                }
            default: 
                {
                    break;
                }
        }
        if (done) {
            break;
        }
        if (resize) {
            resizeUI();
            help_window->resizeWin(window_size_map["helpSz"]);
            paintHelpWindow(help_window);
            help_window->refreshWin();
            resize = false;
        }
    }
    return update_menu;
}

std::unique_ptr<CursesWindow> UI::initHelpWindow()
{
    return std::make_unique<CursesWindow>(18, col - (col / 2), (row / 4), col / 4, "help");
}

void UI::paintHelpWindow(std::unique_ptr<CursesWindow>& win)
{
    const int begy = 1;
    const point maxyx = win->getMaxyx();
    win->printInMiddle(begy, 0, maxyx.x, msgHelpMenu, COLOR_PAIR(7));
    win->addStr(begy + 2, 0, msgHelpAdd);
    win->addStr(begy + 3, 0, msgHelpArrowKeys);
    win->addStr(begy + 4, 0, msgHelpReturn);
    win->addStr(begy + 5, 0, msgHelpPause);
    win->addStr(begy + 6, 0, msgHelpPauseAll);
    win->addStr(begy + 7, 0, msgHelpResume);
    win->addStr(begy + 8, 0, msgHelpResumeAll);
    win->addStr(begy + 9, 0, msgHelpClear);
    win->addStr(begy + 10, 0, msgHelpKill);
    win->addStr(begy + 11, 0, msgHelpKillAll);
    win->addStr(begy + 12, 0, msgHelpExit);
    win->printInMiddle(begy + 14, 0, maxyx.x, msgHelpCloseWin, COLOR_PAIR(7));
    win->drawBox(0, 0);
}

/* Init a subwindow containg infos about the selected download */
int UI::showDownloadDetails(const std::string& item_name)
{
    /* Important: We begin by assigning a new form to det_form unique_ptr and then assigning a new window to 
     * det_win unique_ptr -> if we reassign the det_win pointer first and then try to reassign det_form, since 
     * det_form has to free some memory corresponding to the old window (now deleted), we end up with a segfault */
    det_form = initForm(2);

    det_win = initWin(window_size_map["detSz"], "det");
    setDetForm();
    paintDetWin(item_name);

    det_form->populateField(REQ_FIRST_FIELD, controller->getURL(item_name));
    det_form->populateField(REQ_LAST_FIELD, item_name);
    det_win->refreshWin();

    /* Disable cursor */
    curs_set(0);
    /* Navigate through the details window */

    int r = navigateDownloadDetailsWindow(item_name);
    return r;
}

int UI::addNewDownload()
{
    /* Important: We begin by assigning a new form to add_dl_form unique_ptr and then assigning a new window to 
     * add_dl_win unique_ptr -> if we reset the add_dl_win pointer first and then try to reset add_dl_form, since 
     * add_dl_form has to free some memory corresponding to the old window (now deleted), we end up with a 
     * segfault */

    add_dl_form = initForm(2);
    add_dl_win = initWin(window_size_map["addSz"], "add");

    std::vector<std::string> tmp_items = {"Start", "Schedule", "Close"};
    point p_max = add_dl_win->getMaxyx();
    //point pBeg = add_dl_win->getBegyx();

    // Init a subwin for the menu
    add_dl_win->setDerwin(1, 34, p_max.y - 2, (p_max.x - 34) / 2);
    // init downloads menu == init any menu. TODO - Create initMenu() 
    add_dl_menu = initMenu(tmp_items);

    setAddDlForm();
    setAddDlMenu();
    paintAddDlWin();

    add_dl_win->touchWin();
    add_dl_win->refreshWin();

    int r = navigateAddDownloadWindow();

    add_dl_menu->clearMenu();
    add_dl_menu->clearItems();

    return r;
}

/* Remark: */ 
void UI::setAddDlMenu()
{
    //point p_max = add_dl_win->getMaxyx();
    //point pBeg = add_dl_win->getBegyx();
    add_dl_menu->menuOptsOn(O_SHOWDESC);
    //add_dl_menu->setMenuWin(add_dl_win);
    add_dl_menu->setMenuDer(add_dl_win);
    //add_dl_menu->setMenuSubDer(add_dl_win, 1, 34, p_max.y - 2, (p_max.x - 34) / 2);

    add_dl_menu->setMenuFormat(1, 3);
    add_dl_menu->setMenuMark(" * ");
    add_dl_menu->postMenu();
}

void UI::paintAddDlWin()
{
    char *title_add = (char*)malloc(((col / 2) + 1)*sizeof(char));
    title_add[col / 2] = '\0';

    std::string add_nl = addNewLabel;
    point maxyx = add_dl_win->getMaxyx();

    int i = 0;
    for (i = 0; i < (col / 2 - 18) / 2; ++i)
        title_add[i] = ' ';

    if (add_nl.length() >= strlen(title_add)) {
        ;
    }
    else {

        for (size_t j = 0; j < add_nl.length(); ++j) {
            title_add[i++] = add_nl[j];
        }
        for (; i < col / 2 ; ++i) {
            title_add[i] = ' ';
        }
        add_dl_win->printInMiddle(1, 0, maxyx.x , title_add, COLOR_PAIR(8));

    }

    add_dl_win->addStr(3, 3, addURL);
    add_dl_win->addStr(7, 3, addSaveAs);
    add_dl_win->drawBox(0, 0);

    free(title_add);
}

std::unique_ptr<CursesForm> UI::initForm(size_t numFields)
{
    return std::make_unique<CursesForm>(numFields);
}

/* Init details window form in order to populate */
void UI::setDetForm()
{
    point maxyx = det_win->getMaxyx();

    /* Set field size and location */
    det_form->setField(0, 1, maxyx.x - 10, 3, 4, 0, 0);
    det_form->setField(1, 1, maxyx.x - 10, 6, 4, 0, 0);

    /* Set field options */
    det_form->setFieldBack(0, A_UNDERLINE);
    det_form->setFieldBack(0, O_AUTOSKIP);
    det_form->fieldOptsOff(0, O_STATIC);
    det_form->setFieldBack(1, A_UNDERLINE);
    det_form->setFieldBack(1, O_AUTOSKIP);
    det_form->fieldOptsOff(1, O_STATIC);

    /* Initialize det_form */
    det_form->initForm();
    det_form->setFormSubwin(det_win);

    det_form->postForm();
}

void UI::setAddDlForm()
{
    point maxyx = add_dl_win->getMaxyx();

    /* Set field size and location */
    add_dl_form->setField(0, 1, maxyx.x - 10, 4, 4, 0, 0);
    add_dl_form->setField(1, 1, maxyx.x - 10, 8, 4, 0, 0);

    /* Set field options */
    add_dl_form->setFieldBack(0, A_UNDERLINE);
    add_dl_form->setFieldBack(0, O_AUTOSKIP);
    add_dl_form->fieldOptsOff(0, O_STATIC);
    add_dl_form->setFieldBack(1, A_UNDERLINE);
    add_dl_form->setFieldBack(1, O_AUTOSKIP);
    add_dl_form->fieldOptsOff(1, O_STATIC);

    /* Initialize add_dl_form */
    add_dl_form->initForm();
    add_dl_form->setFormSubwin(add_dl_win);

    add_dl_form->postForm();
}

int UI::resizeDownloadDetailsWindow(const std::string& filename)
{
    stopProgressBarThread();

    //endwin();
    refresh();
    resizeUI();

    det_form = initForm(2);
    det_win->resizeWin(window_size_map["detSz"]);
    setDetForm();
    paintDetWin(filename);
    det_form->populateField(REQ_FIRST_FIELD, controller->getURL(filename));
    det_form->populateField(REQ_LAST_FIELD, filename);

    det_win->refreshWin();
    startProgressBarThread(filename);
    return 0;
}

void UI::resizeAddDownloadWindow(std::string url, std::string filename)
{
    refresh();
    resizeUI();

    add_dl_form = initForm(2);
    add_dl_win = initWin(window_size_map["addSz"], "add");

    std::vector<std::string> tmp_items = {"Start", "Schedule", "Close"};
    point p_max = add_dl_win->getMaxyx();
    //point pBeg = add_dl_win->getBegyx();

    // Init a subwin for the menu
    add_dl_win->setDerwin(1, 34, p_max.y - 2, (p_max.x - 34) / 2);
    // init downloads menu == init any menu 
    add_dl_menu = initMenu(tmp_items);

    setAddDlForm();
    setAddDlMenu();
    paintAddDlWin();

    url.push_back('\0');
    filename.push_back('\0');
    url = trimSpaces(url);
    filename = trimSpaces(filename);

    add_dl_form->populateField(REQ_FIRST_FIELD, url);
    add_dl_form->populateField(REQ_LAST_FIELD, filename);

    add_dl_win->touchWin();
    add_dl_win->refreshWin();
}

/* Navigate through the 'Add a download' window */
int UI::navigateAddDownloadWindow()
{
    int ch = 0;

    bool done = false;
    bool update_menu = false;
    bool resizeAdd = false;
    bool urlErr = false, fileErr = false;

    int currField = 0;

    // TODO - cursor pos
    int curPos = 0;
    // curPos 0 -> first field
    // curPos 1 -> second field
    // curPos 2 -> Start button
    // curPos 3 -> Schedule button
    // curPos 4 -> Close button

    curs_set(1);   
    add_dl_win->wMove(4, 4);
    add_dl_win->refreshWin();

    while ((ch = getch())) {
        point maxyx = add_dl_win->getMaxyx();

        switch (ch) {
            case KEY_RESIZE:
                {
                    if (add_dl_form->formDriver(REQ_VALIDATION) != E_OK) {
                        ;
                    }
                    /* currField == first_field (518) + offset */
                    currField = REQ_FIRST_FIELD + add_dl_form->curFieldIdx();
                    resizeAdd = true;
                    update_menu = true;
                    break;
                }

            case KEY_UP:
                {
                    // first field  
                    if (curPos == 0) {
                        curs_set(0);
                        add_dl_menu->menuDriver(REQ_LAST_ITEM);
                        curPos = 4;
                    }
                    // second field
                    else if (curPos == 1) {
                        add_dl_form->formDriver(REQ_PREV_FIELD);
                        add_dl_form->formDriver(REQ_END_LINE);
                        curPos = 0;
                    } 
                    // any menu itme
                    else {
                        curs_set(1);
                        add_dl_form->formDriver(REQ_LAST_FIELD);
                        add_dl_form->formDriver(REQ_END_LINE);
                        curPos = 1;
                    }
                    break;
                }

            case KEY_DOWN:
                {
                    // first field
                    if (curPos == 0) {
                        add_dl_form->formDriver(REQ_NEXT_FIELD);
                        add_dl_form->formDriver(REQ_END_LINE);
                        curPos = 1;
                    }
                    // second field
                    else if (curPos == 1) {
                        curs_set(0);
                        add_dl_menu->menuDriver(REQ_FIRST_ITEM);
                        curPos = 2;
                    }
                    // any menu items
                    else {
                        curs_set(1);
                        add_dl_form->formDriver(REQ_FIRST_FIELD);
                        add_dl_form->formDriver(REQ_END_LINE);
                        curPos = 0;
                    }
                    break;
                }
            case KEY_LEFT:
                {
                    // any field
                    if (curPos == 0 || curPos == 1) {
                        add_dl_form->formDriver(REQ_PREV_CHAR);
                    }
                    // Move to the right
                    else if (curPos == 2) {
                        add_dl_menu->menuDriver(REQ_LAST_ITEM);
                        curPos = 4;
                    }
                    else {
                        add_dl_menu->menuDriver(REQ_PREV_ITEM);
                        curPos -= 1;
                    }
                    break;
                }

            case KEY_RIGHT:
                {
                    // any field
                    if (curPos == 0 || curPos == 1) {
                        add_dl_form->formDriver(REQ_NEXT_CHAR);
                    }
                    // Move to the left
                    else if (curPos == 4) {
                        add_dl_menu->menuDriver(REQ_FIRST_ITEM);
                        curPos = 2;
                    }
                    else {
                        add_dl_menu->menuDriver(REQ_NEXT_ITEM);
                        curPos += 1;
                    }
                    break;
                }
            case 10:
                {
                    // add a space to the current field
                    if (curPos == 0 || curPos == 1) {
                        add_dl_form->formDriver(' ');
                    }
                    else if (curPos == 3) {
                        // TODO - schedule
                    }
                    else if (curPos == 4) {
                        done = true;
                    }

                    // Start button
                    // TODO - Split all this into functions
                    else if (curPos == 2) {
                        if (add_dl_form->formDriver(REQ_VALIDATION) != E_OK) {
                            //check error 
                            ;
                        }
                        else {
                            /* Get user input from the fields */
                            std::string url = add_dl_form->getFieldBuffer(0);
                            std::string filename = add_dl_form->getFieldBuffer(1);
                            url = trimSpaces(url);                        
                            /* TODO - after having trimmed spaces, count hown many links we have */
                            std::string urlField = url;
                            urlField.push_back(' ');
                            // '\n' is replaced by ' ' 
                            std::string delimiter = " ";
                            std::string token;
                            std::vector<std::string> urls;
                            size_t pos = 0;
                            while ((pos = urlField.find(delimiter)) != std::string::npos) {
                                token = urlField.substr(0, pos);
                                urls.push_back(token);
                                // Remove found token from buffer 
                                urlField.erase(0, pos + delimiter.length());
                            }

                            // TODO - same for filename
                            std::string filenameField;
                            filename = trimSpaces(filename);
                            /* tmp */

                            /* Must be at least 7 char long -> http:// */
                            if (checkURL(url)) {
                                add_dl_win->printInMiddle(6, 0, maxyx.x, msgInvalidURL, COLOR_PAIR(1));
                                urlErr = true;
                            }
                            else {
                                /* Erase error msg -> fill with white space */
                                add_dl_win->printInMiddle(6, 0, maxyx.x, "                   ", COLOR_PAIR(7));
                                url.push_back('\0');
                                urlErr = false;
                            }

                            if (!checkFilename(filename)){
                                add_dl_win->printInMiddle(10, 0, maxyx.x, msgInvalidFilename, COLOR_PAIR(1));
                                fileErr = true;
                            }
                            else {
                                /* Erase error msg -> fill with white space */
                                add_dl_win->printInMiddle(10, 0, maxyx.x, "                        ", COLOR_PAIR(7));
                                filename.push_back('\0');
                                fileErr = false;
                            }

                            if (fileErr || urlErr) {
                                if (fileErr) {
                                    add_dl_form->formDriver(REQ_LAST_FIELD);
                                    add_dl_form->formDriver(REQ_END_LINE);
                                }
                                if (urlErr) {
                                    add_dl_form->formDriver(REQ_FIRST_FIELD);
                                    add_dl_form->formDriver(REQ_END_LINE);
                                }

                                curs_set(1);
                                /* Break out of loop and reenter it */
                                break;
                            }

                            for (auto el : urls) {
                                /* controller returns the final filename after verifying there wasn't a duplicate.*/
                                std::string f = controller->createNewDl(downloads_folder, filename, el,  0, 0);
                                if (f != "NULL") {
                                    /* Start dl if everything ok */
                                    controller->startDl(f);
                                    /* Update downloads list in the menu */
                                }
                            }
                            updateDownloadsMenu();
                            done = true;
                            /* Signals to reset menu window */
                            update_menu = true;
                        }
                    }
                    // if 'Enter' (actually '\n') happens in a field -> treat it as a char 
                    break;
                }
                /* DEL backspace (macOS) */
            case 127:
                {
                    add_dl_form->formDriver(REQ_DEL_PREV);
                    break;
                }

                /* ASCII backspace (Linux) */
            case 8:
                {
                    add_dl_form->formDriver(REQ_DEL_PREV);
                    break;
                }

            case KEY_DC:
                {
                    add_dl_form->formDriver(REQ_DEL_CHAR);
                    break;
                }

            case CTRL('x'):
                {
                    /* Close add_dl_window */
                    done = true;
                    break;
                }

            case CTRL('l'):
                {
                    /* TODO - schedule ! */
                    done = true;
                    break;
                }

            default:
                {
                    add_dl_form->formDriver(ch);
                    break;
                }
        }

        if (resizeAdd) {
            resizeAddDownloadWindow(add_dl_form->getFieldBuffer(0), add_dl_form->getFieldBuffer(1));
            /* Restore errors */
            if (fileErr) {
                add_dl_win->printInMiddle(10, 0, maxyx.x, msgInvalidFilename, COLOR_PAIR(1));
            }
            if (urlErr) {
                add_dl_win->printInMiddle(6, 0, maxyx.x, msgInvalidURL, COLOR_PAIR(1));
            }
            /* Restore cursor position */
            add_dl_form->formDriver(currField); 
            add_dl_form->formDriver(REQ_END_LINE); 
            resizeAdd = false;
        }

        if (done) {
            break;
        }
        add_dl_win->touchWin();
        add_dl_win->refreshWin();
        //addMenuWin->touchWin();
        //addMenuWin->refreshWin();
    }
    curs_set(0);

    return update_menu;
}

void UI::updateDownloadsMenu()
{
    menu = initMenu(controller->getDownloadsList());
    setDownloadsMenu();
}


void UI::paintDetWin(const std::string& itemName)
{
    point maxyx = det_win->getMaxyx();
    det_win->drawBox(0, 0);

    det_win->printInMiddle(maxyx.y - 2, 0, maxyx.x/ 4, msgClose, COLOR_PAIR(8));
    det_win->printInMiddle(maxyx.y - 2, maxyx.x / 4, maxyx.x / 4, msgPause, COLOR_PAIR(8));
    det_win->printInMiddle(maxyx.y - 2, 2 * maxyx.x / 4, maxyx.x / 4, msgResume, COLOR_PAIR(8));
    det_win->printInMiddle(maxyx.y - 2, 3 * maxyx.x / 4, maxyx.x / 4, msgKill, COLOR_PAIR(8));
    det_win->printInMiddle(1, 0, maxyx.x, initDownloadDetailsTitle(itemName), COLOR_PAIR(8));
}

const std::string UI::initDownloadDetailsTitle(const std::string& itemName)
{
    std::string it = itemName;
    it.erase(it.find('\0'));
    const std::string label_truncated = " ... - Details ";
    const std::string label = " - Details ";
    it.reserve(it.length() + 1);
    it.insert(it.begin(), ' ');
    if (it.length() > 20) {
        it.resize(20);
        it.append(label_truncated);
    }
    else {
        it.append(label);
    }
    it.push_back('\0');

    return it;
}

void UI::startProgressBarThread(const std::string& filename)
{
    /* Initialize progress bar according to its parent win (details win) dimensions */
    progress_win = initWin(window_size_map["progSz"], "prog");
    progress_win->drawBox(0, 0);
    progress_win->touchWin();
    progress_win->refreshWin();
    refresh_progress = true;
    future_progress_bar = std::async(std::launch::async, &UI::progressBar, this, filename);
}

/* Stop progress subwindow */
int UI::stopProgressBarThread()
{
    /* Signal to stop progress bar thread once we exit the details window */
    {
        std::lock_guard<std::mutex> guard(download_progress_window_mutex);
        if (!refresh_progress) { return 1; }
        refresh_progress = false;
    }

    /* Wait for the thread to stop before moving on */
    if (!(futureIsReady(future_progress_bar))) {
        while (!futureIsReady(future_progress_bar)) {
            //wait unitl execution is done
            ;
        }
    } 
    return 0;
}

/* Navigate through a download details subwindow */
int UI::navigateDownloadDetailsWindow(const std::string& filename)
{
    int ch = 0;
    bool done = false;
    bool update_menu = false;
    bool resizeDet = false;

    startProgressBarThread(filename);

    while ((ch = getch())) {
        switch (ch) {
            case KEY_RESIZE:
                {
                    update_menu = true;
                    resizeDet = true;
                    break;
                }

            case 'r':
                {
                    controller->resume(filename);
                }
                break;

            case 'p':
                {
                    controller->pause(filename);
                }
                break;

            case 'x':
                {
                    /* Close subwindow */
                    done = true;
                    break;
                }

            case 'k':
                {
                    stopProgressBarThread();
                    controller->stop(filename);
                    update_menu = true;
                    done = true;
                    break;
                }

            default:
                {
                    break;
                }

        }
        if (done) {
            break;
        }
        if (resizeDet) {
            resizeDownloadDetailsWindow(filename);    
            resizeDet = false;
        }
    } 
    stopProgressBarThread();

    return update_menu;
}

/* Display a subwindow containing details about the selected download */ 
void UI::progressBar(const std::string& filename)
{
    point maxyx = progress_win->getMaxyx();

    int progBarWidth = maxyx.x - 4;
    int i = 0;

    float progCounter = controller->getProgress(filename);

    if (progCounter == 100.0) {
        std::string progStr;
        for (i = 0; i < progBarWidth; ++i) {
            progStr.push_back(' ');
        }

        {
            std::lock_guard<std::mutex> guard(download_progress_window_mutex);
            progress_win->printInMiddle(1, 0, maxyx.x, hundredPer, COLOR_PAIR(2));
            progress_win->winAttrOn(COLOR_PAIR(16));
            progress_win->addStr(2, 2, progStr);
            progress_win->winAttrOff(COLOR_PAIR(16));
            progress_win->refreshWin();
        }
    }
    else {
        while (true) {
            {
                std::lock_guard<std::mutex> guard(download_progress_window_mutex);
                if (!refresh_progress) {
                    break;
                }
            }

            int curProg = progCounter * progBarWidth / 100.0;
            const std::string percent = stringifyNumber(controller->getProgress(filename), 2); 
            std::string progStr;
            for (i = 0; i < curProg; ++i) {
                progStr.push_back(' ');
            }

            {
                std::lock_guard<std::mutex> guard(download_progress_window_mutex);
                progress_win->printInMiddle(1, 0, maxyx.x, percent, COLOR_PAIR(2));
                progress_win->winAttrOn(COLOR_PAIR(16));
                progress_win->addStr(2, 2, progStr);
                progress_win->winAttrOff(COLOR_PAIR(16));
                progress_win->refreshWin();
            }
            /* Sleep 100ms before refreshing the window again or the while loop will execute endlessly 
             * so it doesn't monopolize time / ressources */ 
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            /* y represents the y postion of the infos to print on the screen - it matches the location
             * of the corresponding download item on the future_update_downloads_status part of the screen */

            progCounter = controller->getProgress(filename);
            /* Refresh status window */
        }
    }
}

