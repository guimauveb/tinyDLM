#include "../include/dlManagerUI.hxx"

dlManagerUI::dlManagerUI()
    :dlManagerControl{std::make_unique<dlManagerController>()}
{
    initCurses();
    initColors();
    setWinsSize();
    initMainWins();
    updateDownloadsMenu();
    initStatusDriver();
    refreshMainWins();        
}

/* Free memory and exit curses */
dlManagerUI::~dlManagerUI()
{
    stopStatusUpdate();
    //stopProgressBarThread();
    menu->clearMenu();
    menu->clearItems();
    /* Pause all downloads -> we use pause to stop them since it does the same thing */
    dlManagerControl->killAll();
    /* Delete main windows */
    mainWindows.clear();
    /* TODO - save downloads list */
    endwin();
}

/* Initialize curses */
void dlManagerUI::initCurses()
{
    setlocale(LC_ALL, "");
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
}

/* Initialize all the colors and color pairs */
void dlManagerUI::initColors()
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

void dlManagerUI::setWinsSize()
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

    winSizeMap["welSz"]      = {row - 8, col, 4, 0};
    winSizeMap["helpSz"]     = {18, col - (col / 2), (row / 4), col / 4};
    winSizeMap["topBarSz"]   = {1, col, 0, 0};
    winSizeMap["labelsSz"]   = {1, col, 2, 0};
    winSizeMap["mainWinSz"]  = {row - 4, col / 2, 4, 0};
    winSizeMap["statusSz"]   = {row - 5, col / 2, 3, col / 2};
    winSizeMap["pHelpSz"]     = {1, col / 2, row - 1, 0};
    winSizeMap["infosSz"]    = {1, col / 2, row - 2, col / 2};
    winSizeMap["addSz"]      = {row / 2 + 1, col - (col / 2), (row / 4), col / 4};
    winSizeMap["detSz"]      = {row / 2, col - (col / 2), (row / 4), col / 4};
    winSizeMap["progSz"]     = {4, (col - (col / 2)) -10, row / 2, col / 4 + 4};

}

void dlManagerUI::resetStatusDriver()
{
    initStatusDriver();
}

void dlManagerUI::statusDriver(int c)
{
    point pMax = mainWindows.at(3)->getMaxyx();
    currItNo = menu->getItemNo();

    if (c == KEY_DOWN) {
        std::lock_guard<std::mutex> guard(yOffsetMutex);
        /* Signals to scroll status window down if the menu is scrolled down */
        /* If curr item id is > the maxixmum number of items displayed in the win -> offset */
        if (currItNo > botItem) {
            yOffset = (currItNo - (pMax.y - 2)); 
            botItem++;
            topItem++;
        }
    }
    else if (c == KEY_UP) {
        /* Signals to scroll status window down if the menu is scrolled down */
        if (currItNo < topItem) { topItem--;
            botItem--;
            yOffset--;
        }
    }
}

void dlManagerUI::initStatusDriver()
{
    point pMax = mainWindows.at(3)->getMaxyx();
    topItem = 0;
    botItem = pMax.y - 2;
    currItNo = 0;
    yOffset = 0;
}

std::unique_ptr<cursesWindow> dlManagerUI::initWin(winSize& sz, const std::string& name)
{
    return std::make_unique<cursesWindow>(sz.row, sz.col, sz.begy, sz.begx, name);
}

void dlManagerUI::paintWelWin(std::unique_ptr<cursesWindow>& welWin)
{
    welWin->printInMiddle(row / 4, 0, col,  tinyDLMWelcome, COLOR_PAIR(7));
    welWin->printInMiddle(row / 4 + 2, 0, col,  tinyDLMVer, COLOR_PAIR(7));
    welWin->printInMiddle(row / 4 + 3, 0, col,  madeBy, COLOR_PAIR(7));
    welWin->printInMiddle(row / 4 + 5, 0, col,  tinyHelp, COLOR_PAIR(7));
}

int dlManagerUI::firstStart()
{
    std::unique_ptr<cursesWindow> welWin = initWin(winSizeMap["welSz"], "welcome");
    paintWelWin(welWin);
    welWin->refreshWin();
    /* Move to current active subwins so we know we have to resize them */

    /* Disable cursor because I can't print to the screen without moving it */
    //curs_set(0);
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
                    addNewDl();
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
            welWin->resizeWin(winSizeMap["welSz"]);
            paintWelWin(welWin);
            welWin->refreshWin();
            resize = false;
        }
        if (done) {
            startStatusUpdate();
            break;
        }
    }
    /* If done == false - signal to main that we should terminate the program */
    refreshMainWins();
    return done;
}

void dlManagerUI::refreshMainWins()
{
    mainWindows.at(topBarIdx)->touchWin();
    mainWindows.at(topBarIdx)->refreshWin();

    mainWindows.at(labelsIdx)->touchWin();
    mainWindows.at(labelsIdx)->refreshWin();

    /* Refresh the downloads list */
    mainWindows.at(mainIdx)->touchWin();
    mainWindows.at(mainIdx)->refreshWin();

    mainWindows.at(statusIdx)->touchWin();
    mainWindows.at(statusIdx)->refreshWin();

    mainWindows.at(pHelpIdx)->touchWin();
    mainWindows.at(pHelpIdx)->refreshWin();

    mainWindows.at(infosIdx)->touchWin();
    mainWindows.at(infosIdx)->refreshWin();

}


/* Resize program when terminal window size is detected */
void dlManagerUI::resizeUI()
{
    setWinsSize();

    mainWindows.at(topBarIdx)->resizeWin(winSizeMap["topBarSz"]);
    paintTopWin(mainWindows.at(topBarIdx));

    mainWindows.at(labelsIdx)->resizeWin(winSizeMap["labelsSz"]);
    paintLabelsWin(mainWindows.at(labelsIdx));

    /* Refresh the downloads list */
    menu->clearMenu();
    menu->clearItems();
    mainWindows.at(mainIdx)->resizeWin(winSizeMap["mainWinSz"]);
    paintMainWinWin(mainWindows.at(mainIdx));

    mainWindows.at(statusIdx)->resizeWin(winSizeMap["statusSz"]);
    paintDlsStatusWin(mainWindows.at(statusIdx));

    mainWindows.at(pHelpIdx)->resizeWin(winSizeMap["pHelpSz"]);
    paintKeyActWin(mainWindows.at(pHelpIdx));

    mainWindows.at(infosIdx)->resizeWin(winSizeMap["infosSz"]);
    paintDlsInfosWin(mainWindows.at(infosIdx));

    refreshMainWins();
}

void dlManagerUI::paintTopWin(std::unique_ptr<cursesWindow>& topWin)
{
    /* Print a whole black on white row at the top of the main window that will be the size of the terminal
     * window width*/
    char *titleMain = (char*)malloc(col*sizeof(char));
    size_t i = 0;

    for (i = 0; i < liteDL_label.length(); ++i)
        titleMain[i] = liteDL_label.at(i);
    for (; i < col; ++i)
        titleMain[i] = ' ';
    //for (size_t j = 0; j < workInProg.length(); ++j)
    //   titleMain[i++] = workInProg.at(j);
    titleMain[i] = '\0';

    topWin->printInMiddle(0, 0, col, titleMain, COLOR_PAIR(8));

    free(titleMain);
}

/* Init main windows */
void dlManagerUI::initMainWins()
{

    /* Window containing top bar title */
    mainWindows.emplace_back(initWin(winSizeMap["topBarSz"], "topBar"));
    paintTopWin(mainWindows.at(topBarIdx));

    /* Top window containing labels such as "name", "url","speed" etc  */
    mainWindows.emplace_back(initWin(winSizeMap["labelsSz"], "labels"));
    paintLabelsWin(mainWindows.at(labelsIdx));

    /* Window containing downloads list as a menu */
    mainWindows.emplace_back(initWin(winSizeMap["mainWinSz"], "main"));
    paintMainWinWin(mainWindows.at(mainIdx));

    /* Window containing ownloads statuses - updated in a separate thread and connected to mainWin */
    mainWindows.emplace_back(initWin(winSizeMap["statusSz"], "status"));
    paintDlsStatusWin(mainWindows.at(statusIdx));

    /* Winodow displaying help message */ 
    mainWindows.emplace_back(initWin(winSizeMap["pHelpSz"], "help"));
    paintKeyActWin(mainWindows.at(pHelpIdx));

    /* Not used for now */
    mainWindows.emplace_back(initWin(winSizeMap["infosSz"], "infos"));
}

void dlManagerUI::paintLabelsWin(std::unique_ptr<cursesWindow>& labelsWin)
{
    labelsWin->printInMiddle(0, 0, col / 2, labelName, COLOR_PAIR(7));
    labelsWin->printInMiddle(0, col / 2, col / 8, labelProgress, COLOR_PAIR(7));
    labelsWin->printInMiddle(0, 5 * col / 8, col / 8, labelSpeed, COLOR_PAIR(7));
    labelsWin->printInMiddle(0, 3 * col / 4, col / 8, labelETA, COLOR_PAIR(7));
    labelsWin->printInMiddle(0, 7 * col / 8, col / 8, labelStatus, COLOR_PAIR(7));
}

void dlManagerUI::paintMainWinWin(std::unique_ptr<cursesWindow>& mainWinWin)
{
    //    mainWinWin->drawBox(0, 0);
}

void dlManagerUI::paintKeyActWin(std::unique_ptr<cursesWindow>& keyActWin)
{
    /* Display keys and their associated functions at the bottom of the window */
    keyActWin->printInMiddle(0, 0, col / 4, msgHelp, COLOR_PAIR(7));
}

void dlManagerUI::paintDlsStatusWin(std::unique_ptr<cursesWindow>& dlsStatusWin){}
void dlManagerUI::paintDlsInfosWin(std::unique_ptr<cursesWindow>& dlsInfosWin){}

/* Populate the status window with downloads informations such as their status / speed / progress */
void dlManagerUI::populateStatusWin(const std::vector<downloadWinInfo>& vec)
{
    int y = 1;
    size_t offset;
    size_t curr;
    point p = mainWindows.at(statusIdx)->getMaxyx();
    chtype color = COLOR_PAIR(7);
    /* Download status color */
    chtype stcolor;

    /* Iterate over the list of downloads we obtained from dlManagerControl */
    /* Start iterating at the offset determined by the highlighted item in the menu */
    {
        std::lock_guard<std::mutex> guard(yOffsetMutex);
        offset = yOffset;
        curr = currItNo;
    }
    for (offset = yOffset; offset < vec.size(); ++offset) {
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

        mainWindows.at(statusIdx)->printInMiddle(y, 0, p.x / 4, vec.at(offset).progress, color);
        /* Display percent sign */
        mainWindows.at(statusIdx)->printInMiddle(y, 0, p.x / 4 + vec.at(offset).progress.length() + 1," %", color);
        mainWindows.at(statusIdx)->printInMiddle(y, p.x / 4, p.x / 4, vec.at(offset).speed, color);
        mainWindows.at(statusIdx)->printInMiddle(y, p.x / 4, p.x / 4 + vec.at(offset).speed.length() + 2," MBs", color);
        //mainWindow.at(statusIdx)->printInMiddledlsStatusWiny, 2 * width / 4, width / 4, dl.eta, color);
        mainWindows.at(statusIdx)->printInMiddle(y, 3 * p.x / 4, p.x / 4, vec.at(offset).status, stcolor);
        y++;
    }
}

void dlManagerUI::setDownloadsMenu()
{
    point pMax = mainWindows.at(mainIdx)->getMaxyx();
    menu->menuOptsOn(O_SHOWDESC);
    menu->setMenuSub(mainWindows.at(mainIdx));

    /* Fill the entire window with items. (-2) corresponds to top and bottom borders that we ignore */
    menu->setMenuFormat(pMax.y - 2, 0);
    menu->setMenuMark(" * ");
    menu->postMenu();
}

/* Display downloads as a menu */
std::unique_ptr<cursesMenu> dlManagerUI::initDownloadsMenu(std::vector<std::string> itemsData)
{
    return std::make_unique<cursesMenu>(itemsData);
}

/* Consistantly refresh the speed / progress /status of the current downloads */ 
void dlManagerUI::updateDownloadsStatusWindow()
{
    while (true) {
        {
            std::lock_guard<std::mutex> guard(dlsInfoMutex);
            if (!updateStatus) {
                break;
            }
        }
        /* Sleep 100ms before refreshing the window again or the while loop will execute endlessly 
         * so it doesn't monopolize time / ressources */ 
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        /* y represents the y postion of the infos to print on the screen - it matches the location
         * of the corresponding download item on the futureUpdateDlsStatus part of the screen */

        /* Refresh status window */
        {
            std::lock_guard<std::mutex> guard(dlsInfoMutex);
            mainWindows.at(statusIdx)->eraseWin();
            populateStatusWin(dlManagerControl->getAllDownloadsInfos());
            mainWindows.at(statusIdx)->refreshWin();
        }
    }
}

void dlManagerUI::startStatusUpdate()
{
    /* If no downloads - erase the window and break out of here */
    if (!dlManagerControl->isActive()) {
        std::lock_guard<std::mutex> guard(dlsInfoMutex);
        mainWindows.at(statusIdx)->refreshWin();
    }
    else {
        updateStatus = true;
        futureUpdateDlsStatus = std::async(std::launch::async, &dlManagerUI::updateDownloadsStatusWindow, this);
    }
}

/* Signals to the downloads status update thread to stop */
int dlManagerUI::stopStatusUpdate()
{
    {
        std::lock_guard<std::mutex> guard(dlsInfoMutex);
        if (!updateStatus) { return 1; }
        updateStatus = false;
    }

    /* Wait for the thread to stop before moving on */
    if (!(futureIsReady(futureUpdateDlsStatus))) {
        while (!futureIsReady(futureUpdateDlsStatus)) {
            //wait unitl execution is done
            ;
        }
    } 
    return 0;
}

int dlManagerUI::showHelp()
{
    helpWin = initHelpWin();
    paintHelpWin(helpWin);
    helpWin->refreshWin();
    return helpNav();
}

int dlManagerUI::helpNav()
{
    int ch = 0;
    bool done = false;
    bool resize = false;
    bool updateMenu = false;
    while ((ch = getch())) {
        switch(ch) {
            case KEY_RESIZE:
                {
                    updateMenu = true;
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
            helpWin->resizeWin(winSizeMap["helpSz"]);
            paintHelpWin(helpWin);
            helpWin->refreshWin();
            resize = false;
        }
    }
    if (updateMenu) {
        return 1;
    }
    return 0;
}

std::unique_ptr<cursesWindow> dlManagerUI::initHelpWin()
{
    return std::make_unique<cursesWindow>(18, col - (col / 2), (row / 4), col / 4, "help");
}

void dlManagerUI::paintHelpWin(std::unique_ptr<cursesWindow>& win)
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
int dlManagerUI::showDetails(const std::string& itemName)
{
    /* Important: We begin by assigning a new form to detForm unique_ptr and then assigning a new window to 
     * detWin unique_ptr -> if we reassign the detWin pointer first and then try to reassign detForm, since 
     * detForm has to free some memory corresponding to the old window (now deleted), we end up with a segfault */
    detForm = initForm(2);

    detWin = initWin(winSizeMap["detSz"], "det");
    setDetForm();
    paintDetWin(itemName);

    detForm->populateField(REQ_FIRST_FIELD, dlManagerControl->getURL(itemName));
    detForm->populateField(REQ_LAST_FIELD, itemName);
    detWin->refreshWin();

    /* Disable cursor */
    curs_set(0);
    /* Navigate through the details window */
    return detNav(itemName);
}

/* Add a new download */
int dlManagerUI::addNewDl()
{
    /* Important: We begin by assigning a new form to addDlForm unique_ptr and then assigning a new window to 
     * addDlWin unique_ptr -> if we reset the addDlWin pointer first and then try to reset addDlForm, since 
     * addDlForm has to free some memory corresponding to the old window (now deleted), we end up with a 
     * segfault */

    addDlForm = initForm(2);
    addDlWin = initWin(winSizeMap["addSz"], "add");

    // TODO - create menu for buttons instead of keys - to allow \n input as a char and not as Enter key */
    std::vector<std::string> tmpItems = {"Start", "Schedule", "Close"};
    // init downloads menu == init any menu 
    point pMax = addDlWin->getMaxyx();
    //point pBeg = addDlWin->getBegyx();

    // Init a subwin for the menu
    addDlWin->setDerwin(1, 34, pMax.y - 2, (pMax.x - 34) / 2);
    addDlMenu = initDownloadsMenu(tmpItems);

    setAddDlForm();
    setAddDlMenu();
    paintAddDlWin();

    addDlWin->touchWin();
    addDlWin->refreshWin();

    int r = addDlNav();

    /* No need to call free() for win and form -> class destructor will take care of it */
    addDlMenu->clearMenu();
    addDlMenu->clearItems();

    return r;
}

/* Remark: */ 
void dlManagerUI::setAddDlMenu()
{
    //point pMax = addDlWin->getMaxyx();
    //point pBeg = addDlWin->getBegyx();
    addDlMenu->menuOptsOn(O_SHOWDESC);
    //addDlMenu->setMenuWin(addDlWin);
    addDlMenu->setMenuDer(addDlWin);
    //addDlMenu->setMenuSubDer(addDlWin, 1, 34, pMax.y - 2, (pMax.x - 34) / 2);

    addDlMenu->setMenuFormat(1, 3);
    addDlMenu->setMenuMark(" * ");
    addDlMenu->postMenu();
}

void dlManagerUI::paintAddDlWin()
{
    char *titleAdd = (char*)malloc((col / 2) + 1*sizeof(char));
    std::string addNL = addNewLabel;
    point maxyx = addDlWin->getMaxyx();

    int i = 0;
    for (i = 0; i < (col / 2 - 18) / 2; ++i)
        titleAdd[i] = ' ';

    if (addNL.length() >= strlen(titleAdd)) {
        ;
    }
    else {

        for (size_t j = 0; j < addNL.length(); ++j) {
            titleAdd[i++] = addNL[j];
        }
        for (; i < col / 2 ; ++i) {
            titleAdd[i] = ' ';
        }
        titleAdd[i] = '\0';
        addDlWin->printInMiddle(1, 0, maxyx.x , titleAdd, COLOR_PAIR(8));

    }

    addDlWin->addStr(3, 3, addURL);
    addDlWin->addStr(7, 3, addSaveAs);
    addDlWin->drawBox(0, 0);

    free(titleAdd);
}

std::unique_ptr<cursesForm> dlManagerUI::initForm(size_t numFields)
{
    return std::make_unique<cursesForm>(numFields);
}

/* Init details window form in order to populate */
void dlManagerUI::setDetForm()
{
    point maxyx = detWin->getMaxyx();

    /* Set field size and location */
    detForm->setField(0, 1, maxyx.x - 10, 3, 4, 0, 0);
    detForm->setField(1, 1, maxyx.x - 10, 6, 4, 0, 0);

    /* Set field options */
    detForm->setFieldBack(0, A_UNDERLINE);
    detForm->setFieldBack(0, O_AUTOSKIP);
    detForm->fieldOptsOff(0, O_STATIC);
    detForm->setFieldBack(1, A_UNDERLINE);
    detForm->setFieldBack(1, O_AUTOSKIP);
    detForm->fieldOptsOff(1, O_STATIC);

    /* Initialize detForm */
    detForm->initForm();
    detForm->setFormSubwin(detWin);

    detForm->postForm();
}

void dlManagerUI::setAddDlForm()
{
    point maxyx = addDlWin->getMaxyx();

    /* Set field size and location */
    addDlForm->setField(0, 1, maxyx.x - 10, 4, 4, 0, 0);
    addDlForm->setField(1, 1, maxyx.x - 10, 8, 4, 0, 0);

    /* Set field options */
    addDlForm->setFieldBack(0, A_UNDERLINE);
    addDlForm->setFieldBack(0, O_AUTOSKIP);
    addDlForm->fieldOptsOff(0, O_STATIC);
    addDlForm->setFieldBack(1, A_UNDERLINE);
    addDlForm->setFieldBack(1, O_AUTOSKIP);
    addDlForm->fieldOptsOff(1, O_STATIC);

    /* Initialize addDlForm */
    addDlForm->initForm();
    addDlForm->setFormSubwin(addDlWin);

    addDlForm->postForm();
}

int dlManagerUI::resizeDetWin(const std::string& filename)
{
    stopProgressBarThread();

    endwin();
    refresh();
    resizeUI();

    detForm = initForm(2);
    detWin->resizeWin(winSizeMap["detSz"]);
    setDetForm();
    paintDetWin(filename);
    detForm->populateField(REQ_FIRST_FIELD, dlManagerControl->getURL(filename));
    detForm->populateField(REQ_LAST_FIELD, filename);

    detWin->refreshWin();
    startProgressBarThread(filename);
    return 0;
}

void dlManagerUI::resizeAddDlNav(std::string url, std::string filename)
{
    endwin();
    refresh();
    resizeUI();

    addDlForm = initForm(2);
    addDlWin->resizeWin(winSizeMap["addSz"]);
    setAddDlForm();
    paintAddDlWin();

    url.push_back('\0');
    filename.push_back('\0');
    url = trimSpaces(url);
    filename = trimSpaces(filename);

    addDlForm->populateField(REQ_FIRST_FIELD, url);
    addDlForm->populateField(REQ_LAST_FIELD, filename);

    addDlWin->refreshWin();
}

/* Navigate through the 'Add a download' window */
int dlManagerUI::addDlNav()
{
    int ch = 0;

    bool done = false;
    bool updateMenu = false;
    bool resizeAdd = false;
    bool urlErr = false, fileErr = false;

    std::string urlField;
    std::string filenameField;
    int currField = 0;

    // TODO - cursor pos
    int curPos = 0;
    // curPos 0 -> first field
    // curPos 1 -> second field
    // curPos 2 -> Start button
    // curPos 3 -> Schedule button
    // curPos 4 -> Close button

    curs_set(1);   
    addDlWin->wMove(4, 4);
    addDlWin->refreshWin();

    while ((ch = getch())) {
        point maxyx = addDlWin->getMaxyx();

        switch (ch) {
            case KEY_RESIZE:
                {
                    if (addDlForm->formDriver(REQ_VALIDATION) != E_OK) {
                        ;
                    }
                    /* currField == first_field (518) + offset */
                    currField = REQ_FIRST_FIELD + addDlForm->curFieldIdx();
                    resizeAdd = true;
                    updateMenu = true;
                    break;
                }

            case KEY_UP:
                {
                    // first field  
                    if (curPos == 0) {
                        addDlMenu->menuDriver(REQ_LAST_ITEM);
                        curPos = 4;
                    }
                    // second field
                    else if (curPos == 1) {
                        addDlForm->formDriver(REQ_PREV_FIELD);
                        addDlForm->formDriver(REQ_END_LINE);
                        curPos = 0;
                    } 
                    // any menu itme
                    else {
                        addDlForm->formDriver(REQ_LAST_FIELD);
                        addDlForm->formDriver(REQ_END_LINE);
                        curPos = 1;
                    }
                    break;
                }

            case KEY_DOWN:
                {
                    // first field
                    if (curPos == 0) {
                        addDlForm->formDriver(REQ_NEXT_FIELD);
                        addDlForm->formDriver(REQ_END_LINE);
                        curPos = 1;
                    }
                    // second field
                    else if (curPos == 1) {
                        addDlMenu->menuDriver(REQ_FIRST_ITEM);
                        curPos = 2;
                    }
                    // any menu items
                    else {
                        addDlForm->formDriver(REQ_FIRST_FIELD);
                        addDlForm->formDriver(REQ_END_LINE);
                        curPos = 0;
                    }
                    break;
                }
            case KEY_LEFT:
                {
                    // any field
                    if (curPos == 0 || curPos == 1) {
                        addDlForm->formDriver(REQ_PREV_CHAR);
                    }
                    // any menu items
                    else {
                        addDlMenu->menuDriver(REQ_PREV_ITEM);
                        curPos -= 1;
                    }
                    break;
                }

            case KEY_RIGHT:
                {
                    // any field
                    if (curPos == 0 || curPos == 1) {
                        addDlForm->formDriver(REQ_NEXT_CHAR);
                    }
                    // any menu items
                    else {
                        addDlMenu->menuDriver(REQ_NEXT_ITEM);
                        curPos += 1;
                    }
                    break;
                }
            case 10:
                {
                    // add a space to the current field
                    if (curPos == 0 || curPos == 1) {
                        addDlForm->formDriver(' ');
                    }

                    // Start button
                    if (curPos == 2) {
                        if (addDlForm->formDriver(REQ_VALIDATION) != E_OK) {
                            //check error 
                            ;
                        }
                        else {
                            /* Get user input from the fields */
                            std::string url = addDlForm->getFieldBuffer(0);
                            std::string filename = addDlForm->getFieldBuffer(1);
                            url = trimSpaces(url);                        
                            /* TODO - after having trimmed spaces, count hown many links we have */
                            std::string urlField = url;
                            urlField.push_back(' ');
                            /* TODO - space + \n */
                            std::string delimiter = " ";
                            std::string token;
                            std::vector<std::string> urls;
                            size_t pos = 0;
                            while ((pos = urlField.find(delimiter)) != std::string::npos) {
                                token = urlField.substr(0, pos);
                                urls.push_back(token);
                                // remove found token from buffer 
                                urlField.erase(0, pos + delimiter.length());
                            }

                            filename = trimSpaces(filename);
                            /* tmp */
                            filename.push_back('\0');

                            //                        /* Must be at least 7 char long -> http:// */
                            //                        if (checkURL(url)) {
                            //                            addDlWin->printInMiddle(6, 0, maxyx.x, msgInvalidURL, COLOR_PAIR(1));
                            //                            urlErr = true;
                            //                        }
                            //                        else {
                            //                            /* Erase error msg -> fill with white space */
                            //                            addDlWin->printInMiddle(6, 0, maxyx.x, "                   ", COLOR_PAIR(7));
                            //                            url.push_back('\0');
                            //                            urlErr = false;
                            //                        }
                            //
                            //                        if (!checkFilename(filename)){
                            //                            addDlWin->printInMiddle(10, 0, maxyx.x, msgInvalidFilename, COLOR_PAIR(1));
                            //                            fileErr = true;
                            //                        }
                            //                        else {
                            //                            /* Erase error msg -> fill with white space */
                            //                            addDlWin->printInMiddle(10, 0, maxyx.x, "                        ", COLOR_PAIR(7));
                            //                            filename.push_back('\0');
                            //                            fileErr = false;
                            //                        }
                            //
                            //                        if (fileErr || urlErr) {
                            //                            if (fileErr) {
                            //                                addDlForm->formDriver(REQ_LAST_FIELD);
                            //                                addDlForm->formDriver(REQ_END_LINE);
                            //                            }
                            //                            if (urlErr) {
                            //                                addDlForm->formDriver(REQ_FIRST_FIELD);
                            //                                addDlForm->formDriver(REQ_END_LINE);
                            //                            }
                            //
                            //                            curs_set(1);
                            //                            /* Break out of loop and reenter it */
                            //                            break;
                            //                        }
                            //
                            //                      
                            //
                            for (auto el : urls) {
                                /* dlManagerControl returns the final filename after verifying there wasn't a duplicate.*/
                                std::string f = dlManagerControl->createNewDl(dlFolder, filename, el,  0, 0);
                                if (f != "NULL") {
                                    /* Start dl if everything ok */
                                    dlManagerControl->startDl(f);
                                    /* Update downloads list in the menu */
                                }
                            }
                            updateDownloadsMenu();
                            done = true;
                            /* Signals to reset menu window */
                            updateMenu = true;
                        }
                    }
                    // if enter (actually '\n' happens in a field treat it as a char 
                    break;
                }
                /* DEL backspace (macOS) */
            case 127:
                {
                    addDlForm->formDriver(REQ_DEL_PREV);
                    break;
                }

                /* ASCII backspace (Linux) */
            case 8:
                {
                    addDlForm->formDriver(REQ_DEL_PREV);
                    break;
                }

            case KEY_DC:
                {
                    addDlForm->formDriver(REQ_DEL_CHAR);
                    break;
                }

            case CTRL('x'):
                {
                    /* Close addDlWindow */
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
                    addDlForm->formDriver(ch);
                    break;
                }
        }

        if (resizeAdd) {
            resizeAddDlNav(addDlForm->getFieldBuffer(0), addDlForm->getFieldBuffer(1));
            /* Restore errors */
            if (fileErr) {
                addDlWin->printInMiddle(10, 0, maxyx.x, msgInvalidFilename, COLOR_PAIR(1));
            }
            if (urlErr) {
                addDlWin->printInMiddle(6, 0, maxyx.x, msgInvalidURL, COLOR_PAIR(1));
            }
            /* Restore cursor position */
            addDlForm->formDriver(currField); 
            addDlForm->formDriver(REQ_END_LINE); 
            resizeAdd = false;
        }

        if (done) {
            break;
        }
        addDlWin->touchWin();
        addDlWin->refreshWin();
        //addMenuWin->touchWin();
        //addMenuWin->refreshWin();
    }
    curs_set(0);
    if (updateMenu) {
        return 1;
    }
    return 0;
}

void dlManagerUI::updateDownloadsMenu()
{
    menu = initDownloadsMenu(dlManagerControl->getDownloadsList());
    setDownloadsMenu();
}


void dlManagerUI::paintDetWin(const std::string& itemName)
{
    point maxyx = detWin->getMaxyx();
    detWin->drawBox(0, 0);

    detWin->printInMiddle(maxyx.y - 2, 0, maxyx.x/ 4, msgClose, COLOR_PAIR(8));
    detWin->printInMiddle(maxyx.y - 2, maxyx.x / 4, maxyx.x / 4, msgPause, COLOR_PAIR(8));
    detWin->printInMiddle(maxyx.y - 2, 2 * maxyx.x / 4, maxyx.x / 4, msgResume, COLOR_PAIR(8));
    detWin->printInMiddle(maxyx.y - 2, 3 * maxyx.x / 4, maxyx.x / 4, msgKill, COLOR_PAIR(8));
    detWin->printInMiddle(1, 0, maxyx.x, initDetailsTitle(itemName), COLOR_PAIR(8));
}

const std::string dlManagerUI::initDetailsTitle(const std::string& itemName)
{
    std::string it = itemName;
    it.erase(it.find('\0'));
    const std::string labelTruncated = " ... - Details ";
    const std::string label = " - Details ";
    it.reserve(it.length() + 1);
    it.insert(it.begin(), ' ');
    if (it.length() > 20) {
        it.resize(20);
        it.append(labelTruncated);
    }
    else {
        it.append(label);
    }
    it.push_back('\0');

    return it;
}

void dlManagerUI::startProgressBarThread(const std::string& filename)
{
    /* Initialize progress bar according to its parent win (details win) dimensions */
    progressWin = initWin(winSizeMap["progSz"], "prog");
    progressWin->drawBox(0, 0);
    progressWin->touchWin();
    progressWin->refreshWin();
    progRef = true;
    futureProgressBar = std::async(std::launch::async, &dlManagerUI::progressBar, this, filename);
}

/* Stop progress subwindow */
int dlManagerUI::stopProgressBarThread()
{
    /* Signal to stop progress bar thread once we exit the details window */
    {
        std::lock_guard<std::mutex> guard(dlProgMutex);
        if (!progRef) { return 1; }
        progRef = false;
    }

    /* Wait for the thread to stop before moving on */
    if (!(futureIsReady(futureProgressBar))) {
        while (!futureIsReady(futureProgressBar)) {
            //wait unitl execution is done
            ;
        }
    } 
    return 0;
}

/* Navigate through a download details subwindow */
int dlManagerUI::detNav(const std::string& filename)
{
    int ch = 0;
    bool done = false;
    bool updateMenu = false;
    bool resizeDet = false;

    startProgressBarThread(filename);

    while ((ch = getch())) {
        switch (ch) {
            case KEY_RESIZE:
                {
                    updateMenu = true;
                    resizeDet = true;
                    break;
                }

            case 'r':
                /* Resume transfer - if the transfer is ongoing - does nothing */
                {
                    dlManagerControl->resume(filename);
                }
                break;

            case 'p':
                /* Pause transfer - if the transfer is already paused - does nothing */
                {
                    dlManagerControl->pause(filename);
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
                    dlManagerControl->stop(filename);
                    updateMenu = true;
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
            resizeDetWin(filename);    
            resizeDet = false;
        }
    } 
    stopProgressBarThread();

    return updateMenu;
}

/* Display a subwindow containing details about the selected download */ 
void dlManagerUI::progressBar(const std::string& filename)
{
    point maxyx = progressWin->getMaxyx();

    int progBarWidth = maxyx.x - 4;
    int i = 0;

    float progCounter = dlManagerControl->getProgress(filename);

    if (progCounter == 100.0) {
        std::string progStr;
        for (i = 0; i < progBarWidth; ++i) {
            progStr.push_back(' ');
        }

        {
            std::lock_guard<std::mutex> guard(dlProgMutex);
            progressWin->printInMiddle(1, 0, maxyx.x, hundredPer, COLOR_PAIR(2));
            progressWin->winAttrOn(COLOR_PAIR(16));
            progressWin->addStr(2, 2, progStr);
            progressWin->winAttrOff(COLOR_PAIR(16));
            progressWin->refreshWin();
        }
    }
    else {
        while (true) {
            {
                std::lock_guard<std::mutex> guard(dlProgMutex);
                if (!progRef) {
                    break;
                }
            }

            int curProg = progCounter * progBarWidth / 100.0;
            const std::string percent = stringifyNumber(dlManagerControl->getProgress(filename), 2); 
            std::string progStr;
            for (i = 0; i < curProg; ++i) {
                progStr.push_back(' ');
            }

            {
                std::lock_guard<std::mutex> guard(dlProgMutex);
                progressWin->printInMiddle(1, 0, maxyx.x, percent, COLOR_PAIR(2));
                progressWin->winAttrOn(COLOR_PAIR(16));
                progressWin->addStr(2, 2, progStr);
                progressWin->winAttrOff(COLOR_PAIR(16));
                progressWin->refreshWin();
            }
            /* Sleep 100ms before refreshing the window again or the while loop will execute endlessly 
             * so it doesn't monopolize time / ressources */ 
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            /* y represents the y postion of the infos to print on the screen - it matches the location
             * of the corresponding download item on the futureUpdateDlsStatus part of the screen */

            progCounter = dlManagerControl->getProgress(filename);
            /* Refresh status window */
        }
    }
}

