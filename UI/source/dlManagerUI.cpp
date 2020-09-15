#include "../include/dlManagerUI.hpp"

/* Mutexes used to avoid concurrent uses of wrefresh() */
dlManagerUI::dlManagerUI()
    :dlManagerControl{std::make_unique<dlManagerController>()}
{
    initCurses();
    initColors();
    setWinsSize();
    mainWinsInit();
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
    /* Empty the downloads list menu */
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
    /* Progress bar color pair */
    init_pair(16, COLOR_GREEN, COLOR_GREEN);
}

void dlManagerUI::setWinsSize()
{
    /* Get terminal screen size in columns and rows */
    getmaxyx(stdscr, row, col);

    welWinSz = {row - 8, col, 4, 0};

    topBarSz = {1, col, 0, 0};
    labelsSz = {1, col, 2, 0};
    mainWinSz = {row - 4, col / 2, 4, 0};
    dlStatusSz = {row - 5, col / 2, 3, col / 2};
    keyActSz = {1, col / 2, row - 1, 0};
    dlInfosSz = {1, col / 2, row - 2, col / 2};

    dlAddSz = {row / 2, col - (col / 2), (row / 4), col / 4};
    dlDetSz = {row / 2, col - (col / 2), (row / 4), col / 4};
    dlProgSz = {4, (col - (col / 2)) -10, row / 2, col / 4 + 4};

    //return struct of winSizes
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

/* Window displaying a welcome message*/
std::unique_ptr<cursesWindow> dlManagerUI::welcomeWinInit()
{
    return std::make_unique<cursesWindow>(row - 8, col, 4, 0, "welcome");
}

void dlManagerUI::paintWelWin(std::unique_ptr<cursesWindow>& welWin)
{
    welWin->printInMiddle(row / 4, 0, col,  tinyDLMWelcome, COLOR_PAIR(7));
    welWin->printInMiddle(row / 4 + 1, 0, col,  tinyDLMVer, COLOR_PAIR(7));
    welWin->printInMiddle(row / 4 + 2, 0, col,  tinyHelp, COLOR_PAIR(7));
}

int dlManagerUI::firstStart()
{
    std::unique_ptr<cursesWindow> welWin = welcomeWinInit();
    paintWelWin(welWin);
    welWin->refreshWin();
    /* Move to current active subwins so we know we have to resize them */

    /* Disable cursor because I can't print to the screen without moving it */
    curs_set(0);
    int ch = 0;
    while ((ch = getch()) != KEY_F(1)) {
        switch(ch) {
            /* TODO -do not resize under 108 * 24 */
            case KEY_RESIZE:
                resizeUI();
                welWin->resizeWin(welWinSz);
                paintWelWin(welWin);
                welWin->refreshWin();
                break;
            case 'a':
                /* Open a window and exit the function */
                addNewDl();
                refreshMainWins();
                startStatusUpdate();
                return 0;
            case 'h':
                showHelp();
                refreshMainWins();
                startStatusUpdate();
                return 0;
                break;
            default:
                break;
        }
    }
    /* Signal to main that we should terminate the program */
    return 1;
}

/* Init the main windows */
void dlManagerUI::mainWinsInit()
{
    mainWindows.emplace_back(mainWinTopBarInit());
    paintTopWin(mainWindows.at(topWinIdx));

    mainWindows.emplace_back(mainWinLabelsInit());
    paintLabelsWin(mainWindows.at(labelsWinIdx));

    mainWindows.emplace_back(mainWinMainInit());
    paintMainWinWin(mainWindows.at(dlsWinIdx));

    mainWindows.emplace_back(mainWinDownloadsStatusInit());
    paintDlsStatusWin(mainWindows.at(dlsStatusWinIdx));

    mainWindows.emplace_back(mainWinKeyActInit());
    paintKeyActWin(mainWindows.at(keyActWinIdx));

    mainWindows.emplace_back(mainWinDlInfosInit());
}

void dlManagerUI::refreshMainWins()
{
    mainWindows.at(topWinIdx)->touchWin();
    mainWindows.at(topWinIdx)->refreshWin();

    mainWindows.at(labelsWinIdx)->touchWin();
    mainWindows.at(labelsWinIdx)->refreshWin();

    /* Refresh the downloads list */
    mainWindows.at(dlsWinIdx)->touchWin();
    mainWindows.at(dlsWinIdx)->refreshWin();

    mainWindows.at(dlsStatusWinIdx)->touchWin();
    mainWindows.at(dlsStatusWinIdx)->refreshWin();

    mainWindows.at(keyActWinIdx)->touchWin();
    mainWindows.at(keyActWinIdx)->refreshWin();

    mainWindows.at(dlsInfosWinIdx)->touchWin();
    mainWindows.at(dlsInfosWinIdx)->refreshWin();

}


/* Resize program when terminal window size is detected */
void dlManagerUI::resizeUI()
{
    // TODO - will return a struct of winSizes objects */
    setWinsSize();

    mainWindows.at(topWinIdx)->resizeWin(topBarSz);
    paintTopWin(mainWindows.at(topWinIdx));

    mainWindows.at(labelsWinIdx)->resizeWin(labelsSz);
    paintLabelsWin(mainWindows.at(labelsWinIdx));

    /* Refresh the downloads list */
    mainWindows.at(dlsWinIdx)->resizeWin(mainWinSz);
    paintMainWinWin(mainWindows.at(dlsWinIdx));

    mainWindows.at(dlsStatusWinIdx)->resizeWin(dlStatusSz);
    paintDlsStatusWin(mainWindows.at(dlsStatusWinIdx));

    mainWindows.at(keyActWinIdx)->resizeWin(keyActSz);
    paintKeyActWin(mainWindows.at(keyActWinIdx));

    mainWindows.at(dlsInfosWinIdx)->resizeWin(dlInfosSz);
    paintDlsInfosWin(mainWindows.at(dlsInfosWinIdx));

    refreshMainWins();
}

/* Window containing top bar title */
std::unique_ptr<cursesWindow> dlManagerUI::mainWinTopBarInit()
{
    return std::make_unique<cursesWindow>(1, col, 0, 0, "top");
}

void dlManagerUI::paintTopWin(std::unique_ptr<cursesWindow>& topWin)
{
    /* Print a whole black on white row at the top of the main window that will be the size of the terminal
     * window width*/
    /* TODO - use malloc instead */
    char titleMain[col];
    size_t i;

    /* TODO - create a function for each */
    for (i = 0; i < liteDL_label.length(); ++i)
        titleMain[i] = liteDL_label.at(i);
    for (; i < (col - workInProg.length()) / 2; ++i)
        titleMain[i] = ' ';
    for (size_t j = 0; j < workInProg.length(); ++j)
        titleMain[i++] = workInProg.at(j);
    for (; i < (size_t)col - 2; ++i)
        titleMain[i] = ' ';
    titleMain[i] = '\0';

    /* TODO - check if conversion to string doesn't break titleMain */
    topWin->printInMiddle(0, 0, col, titleMain, COLOR_PAIR(8));
}

/* Top window containing labels such as "name", "url","speed" etc  */
std::unique_ptr<cursesWindow> dlManagerUI::mainWinLabelsInit()
{   
    return std::make_unique<cursesWindow>(1, col, 2, 0, "labels");
}

void dlManagerUI::paintLabelsWin(std::unique_ptr<cursesWindow>& labelsWin)
{
    /* TODO - use a struct holding all that instead */
    labelsWin->printInMiddle(0, 0, col / 2, labelName, COLOR_PAIR(7));
    labelsWin->printInMiddle(0, col / 2, col / 8, labelProgress, COLOR_PAIR(7));
    labelsWin->printInMiddle(0, 5 * col / 8, col / 8, labelSpeed, COLOR_PAIR(7));
    labelsWin->printInMiddle(0, 3 * col / 4, col / 8, labelETA, COLOR_PAIR(7));
    labelsWin->printInMiddle(0, 7 * col / 8, col / 8, labelStatus, COLOR_PAIR(7));
}

/* downloads list window */
std::unique_ptr<cursesWindow> dlManagerUI::mainWinMainInit()
{
    return std::make_unique<cursesWindow>(row - 4, col / 2, 4, 0, "menu");
}

void dlManagerUI::paintMainWinWin(std::unique_ptr<cursesWindow>& mainWinWin)
{
    //    mainWinWin->drawBox(0, 0);
}

/* Init the download progress subwindow */ 
std::unique_ptr<cursesWindow> dlManagerUI::mainWinKeyActInit()
{
    return std::make_unique<cursesWindow>(1, col / 2, row - 1, 0, "key");
}

void dlManagerUI::paintKeyActWin(std::unique_ptr<cursesWindow>& keyActWin)
{
    /* Display keys and their associated functions at the bottom of the window */
    keyActWin->printInMiddle(0, 0, col / 4, msgHelp, COLOR_PAIR(7));
}

/* init the global download info subwindow in a separated thread - displays number of downloads + speed */ 
std::unique_ptr<cursesWindow> dlManagerUI::mainWinDlInfosInit()
{
    return std::make_unique<cursesWindow>(1, col / 2, row - 2, col / 2, "dlsinfos");
}

/* init the downloads status window */
std::unique_ptr<cursesWindow> dlManagerUI::mainWinDownloadsStatusInit()
{
    return std::make_unique<cursesWindow>(row - 5, col / 2, 3, col / 2, "status");
}

void dlManagerUI::paintDlsStatusWin(std::unique_ptr<cursesWindow>& dlsStatusWin){}
void dlManagerUI::paintDlsInfosWin(std::unique_ptr<cursesWindow>& dlsInfosWin){}

/* Populate the status window with downloads informations such as their status / speed / progress */
void dlManagerUI::populateStatusWin(const std::vector<downloadWinInfo>& vec)
{
    int y = 1;
    size_t offset;
    size_t curr;
    point p = mainWindows.at(dlsStatusWinIdx)->getMaxyx();
    chtype color; 

    /* Iterate over the list of downloads we obtained from dlManagerControl */
    /* Start iterating at the offset determined by the highlighted item in the menu */
    {
        std::lock_guard<std::mutex> guard(yOffsetMutex);
        offset = yOffset;
        curr = currItNo;
    }
    for (offset = yOffset; offset < vec.size(); ++offset) {
        if (curr == offset) {
            /* Highlight selected item */
            color = COLOR_PAIR(8);
        }
        else {
            color = COLOR_PAIR(7);
        }
        mainWindows.at(dlsStatusWinIdx)->printInMiddle(y, 0, p.x / 4, vec.at(offset).progress, color);
        mainWindows.at(dlsStatusWinIdx)->printInMiddle(y, 0, p.x / 4 + vec.at(offset).progress.length() + 1,
                " %", color);
        mainWindows.at(dlsStatusWinIdx)->printInMiddle(y, p.x / 4, p.x / 4, vec.at(offset).speed, color);
        mainWindows.at(dlsStatusWinIdx)->printInMiddle(y, p.x / 4, p.x / 4 + vec.at(offset).speed.length() + 2,
                " MBs", color);
        //mainWindow.at(dlsStatusWinIdx)->printInMiddledlsStatusWiny, 2 * width / 4, width / 4, dl.eta, color);
        mainWindows.at(dlsStatusWinIdx)->printInMiddle(y, 3 * p.x / 4, p.x / 4, vec.at(offset).status, color);
        y++;
    }
}

void dlManagerUI::setDownloadsMenu()
{
    point pMax = mainWindows.at(dlsWinIdx)->getMaxyx();
    menu->menuOptsOn(O_SHOWDESC);
    menu->setMenuSub(mainWindows.at(dlsWinIdx));

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
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        /* y represents the y postion of the infos to print on the screen - it matches the location
         * of the corresponding download item on the futureUpdateDlsStatus part of the screen */

        /* Refresh status window */
        {
            std::lock_guard<std::mutex> guard(dlsInfoMutex);
            if (resize) {
                mainWindows.at(dlsStatusWinIdx)->resizeWin(dlStatusSz);
                mainWindows.at(dlsStatusWinIdx)->touchWin();
                mainWindows.at(dlsStatusWinIdx)->refreshWin();
                resize = false;
            }
            mainWindows.at(dlsStatusWinIdx)->eraseWin();
            populateStatusWin(dlManagerControl->getAllDownloadsInfos());
            mainWindows.at(dlsStatusWinIdx)->refreshWin();
        }
    }
}

void dlManagerUI::startStatusUpdate()
{
    /* If no downloads - erase the window and break out of here */
    if (!dlManagerControl->isActive()) {
        std::lock_guard<std::mutex> guard(dlsInfoMutex);
        mainWindows.at(dlsStatusWinIdx)->refreshWin();
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

void dlManagerUI::showHelp()
{
    std::unique_ptr<cursesWindow> helpWin = initHelpWin();
    paintHelpWin(helpWin);
    helpWin->refreshWin();
    helpNav(helpWin);
}

void dlManagerUI::helpNav(std::unique_ptr<cursesWindow>& win)
{
    getch();
}

std::unique_ptr<cursesWindow> dlManagerUI::initHelpWin()
{
    return std::make_unique<cursesWindow>(row / 2 + 2, col - (col / 2), (row / 4), col / 4, "help");
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

/* Add a new download */
int dlManagerUI::addNewDl()
{
    /* Important: We begin by assigning a new form to addDlForm unique_ptr and then assigning a new window to 
     * addDlWin unique_ptr -> if we reset the addDlWin pointer first and then try to reset addDlForm, since 
     * addDlForm has to free some memory corresponding to the old window (now deleted), we end up with a 
     * segfault */
    addDlForm = initAddDlForm(2);
    addDlWin = addDlInitWin();
    setAddDlForm();
    paintAddDlWin();
    addDlWin->touchWin();
    addDlWin->refreshWin();
    /* No need to call free() for win and form -> class destructor will take care of it */
    return addDlNav();
}

/* Init "Add a download" window */
std::unique_ptr<cursesWindow> dlManagerUI::addDlInitWin()
{
    return std::make_unique<cursesWindow>(row / 2, col - (col / 2), (row / 4), col / 4, "addDl");
}

void dlManagerUI::paintAddDlWin()
{
    char titleAdd[col / 2];
    int i;
    /* TODO - create a function for each */
    for (i = 0; i < (col / 2 - 18) / 2; ++i)
        titleAdd[i] = ' ';
    for (size_t j = 0; j < addNewLabel.length(); ++j)
        titleAdd[i++] = addNewLabel[j];
    for (; i < col / 2 ; ++i)
        titleAdd[i] = ' ';
    titleAdd[i] = '\0';

    point maxyx = addDlWin->getMaxyx();
    addDlWin->addStr(3, 3, addURL);
    addDlWin->addStr(7, 3, addSaveAs);
    addDlWin->printInMiddle(1, 0, maxyx.x , titleAdd, COLOR_PAIR(8));
    addDlWin->printInMiddle(maxyx.y - 2, 0, maxyx.x / 3, msgStart, COLOR_PAIR(8));
    addDlWin->printInMiddle(maxyx.y - 2, 0, maxyx.x, msgScheduleCtrl, COLOR_PAIR(8));
    addDlWin->printInMiddle(maxyx.y - 2, 2 * maxyx.x / 3, maxyx.x / 3, msgCloseCtrl, COLOR_PAIR(8));
    addDlWin->drawBox(0, 0);
}

std::unique_ptr<cursesForm> dlManagerUI::initAddDlForm(size_t numFields)
{
    return std::make_unique<cursesForm>(numFields);
}

std::unique_ptr<cursesForm> dlManagerUI::initDetForm(size_t numFields)
{
    return std::make_unique<cursesForm>(numFields);
}

void dlManagerUI::setAddDlForm()
{
    point maxyx = addDlWin->getMaxyx();
    setWinsSize();

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

void dlManagerUI::resizeAddDlNav(std::string url, std::string filename)
{
    resizeUI();
    addDlForm = initAddDlForm(2);
    addDlWin = addDlInitWin();
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
    curs_set(1);   
    point maxyx = addDlWin->getMaxyx();
    int ch = 0;
    bool done = false;
    bool updateMenu = false;

    while ((ch = getch())) {
        switch (ch) {
            /* TODO -do not resize under 108 * 24 */
            case KEY_RESIZE:
                {
                    if (addDlForm->formDriver(REQ_VALIDATION) != E_OK) {
                        ;
                    }
                    const std::string url = addDlForm->getFieldBuffer(0);
                    const std::string filename = addDlForm->getFieldBuffer(1);
                    resizeAddDlNav(url, filename);
                    updateMenu = true;
                }
                break;

            case KEY_UP:
                addDlForm->formDriver(REQ_PREV_FIELD);
                addDlForm->formDriver(REQ_END_LINE);
                break;

            case KEY_DOWN:
                addDlForm->formDriver(REQ_NEXT_FIELD);
                addDlForm->formDriver(REQ_END_LINE);
                break;

            case KEY_LEFT:
                addDlForm->formDriver(REQ_PREV_CHAR);
                break;

            case KEY_RIGHT:
                addDlForm->formDriver(REQ_NEXT_CHAR);
                break;

            case 127:
                addDlForm->formDriver(REQ_DEL_PREV);
                break;

            case KEY_DC:
                addDlForm->formDriver(REQ_DEL_CHAR);
                break;

            case CTRL('x'):
                /* Close addDlWindow */
                done = true;
                break;

            case CTRL('l'):
                /* TODO - schedule ! */
                done = true;
                break;

            case 10:
                {
                    if (addDlForm->formDriver(REQ_VALIDATION) != E_OK) {
                        //check error 
                        ;
                    }
                    else {
                        /* TODO - create a checkUserInput function that will call everyone */

                        /* Get user input from the fields */
                        std::string url = addDlForm->getFieldBuffer(0);
                        std::string filename = addDlForm->getFieldBuffer(1);
                        std::string URLcleaned = trimSpaces(url);                        
                        std::string filenameCleaned = trimSpaces(filename);

                        /* Must be at least 7 char long -> http:// */
                        if (checkURL(url) == 1) {
                            addDlWin->printInMiddle(maxyx.y - 4, 0, maxyx.x, msgInvalidURL, COLOR_PAIR(1));
                            break;
                        }
                        if (!checkFilename(filename)){
                            addDlWin->printInMiddle(maxyx.y - 4, 0, maxyx.x, msgInvalidFilename, COLOR_PAIR(1));
                            break;
                        }

                        URLcleaned.push_back('\0');
                        filenameCleaned.push_back('\0');
                        /* Start dl if everything ok */
                        std::string f = dlManagerControl->createNewDl(dlFolder, filenameCleaned, URLcleaned,  0, 0);
                        dlManagerControl->startDl(f);
                        /* Update downloads list in the menu */
                        updateDownloadsMenu();
                        done = true;
                        /* Signals to reset downloads menu */
                        updateMenu = true;
                        break;
                    }
                }
                break;

            default:
                {
                addDlForm->formDriver(ch);
                break;
                }
        }

        if (done) {
            break;
        }
        addDlWin->refreshWin();
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

/* Init a subwindow containg infos about the selected download */
int dlManagerUI::showDetails(const std::string& itemName)
{
    /* Important: We begin by assigning a new form to detForm unique_ptr and then assigning a new window to 
     * detWin unique_ptr -> if we reassign the detWin pointer first and then try to reassign detForm, since 
     * detForm has to free some memory corresponding to the old window (now deleted), we end up with a segfault */
    detForm = initDetForm(2);
    detWin = initDetWin();
    setDetForm();
    paintDetWin(itemName);

    detForm->populateField(REQ_FIRST_FIELD, dlManagerControl->getURL(itemName));
    detForm->populateField(REQ_LAST_FIELD, itemName);
    detWin->refreshWin();

    /* Disable cursor */
    curs_set(0);
    /* Navigate through the details window */
    return (detNav(itemName));
}

std::unique_ptr<cursesWindow> dlManagerUI::initDetWin(/* pass winsize */)
{
    return std::make_unique<cursesWindow>(row / 2, col - (col / 2), (row / 4), col / 4, "detwin");
}

void dlManagerUI::paintDetWin(const std::string& itemName)
{
    point maxyx = detWin->getMaxyx();
    detWin->drawBox(0, 0);

    detWin->printInMiddle(maxyx.y - 2, 0, maxyx.x/ 4, msgClose, COLOR_PAIR(8));
    detWin->printInMiddle(maxyx.y - 2, maxyx.x / 4, maxyx.x / 4, msgPause, COLOR_PAIR(8));
    detWin->printInMiddle(maxyx.y - 2, 2 * maxyx.x / 4, maxyx.x / 4, msgResume, COLOR_PAIR(8));
    detWin->printInMiddle(maxyx.y - 2, 3 * maxyx.x / 4, maxyx.x / 4, msgKill, COLOR_PAIR(8));
    detWin->printInMiddle(1, 0, maxyx.x, initDetailsTitle(itemName), COLOR_PAIR(7));
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
    return itemName;
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
    detForm->setFormWin(detWin);
    detForm->setFormSubwin(detWin);

    detForm->postForm();
}

/* shared_ptr that will be moved to progressWin own thread */
std::unique_ptr<cursesWindow> dlManagerUI::initProgressWin(const point& bx, const point& mx)
{
    return std::make_unique<cursesWindow>(4, mx.x-10, mx.y, bx.x+4, "prog");
}

void dlManagerUI::startProgressBarThread(const std::string& filename)
{
    /* Initialize progress bar according to its parent win (details win) dimensions */
    point begyx = detWin->getBegyx();
    point maxyx = detWin->getMaxyx();
    progressWin = initProgressWin(begyx, maxyx);
    progressWin->drawBox(0, 0);
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

int dlManagerUI::resizeDetWin(const std::string& filename)
{
    stopProgressBarThread();
    resizeUI();
    detForm = initDetForm(2);
    detWin = initDetWin();
    setDetForm();
    paintDetWin(filename);
    detForm->populateField(REQ_FIRST_FIELD, dlManagerControl->getURL(filename));
    detForm->populateField(REQ_LAST_FIELD, filename);
    detWin->refreshWin();

    startProgressBarThread(filename);
    return 0;
}

/* Navigate through a download details subwindow */
int dlManagerUI::detNav(const std::string& filename)
{
    int ch = 0;
    bool done = false;
    bool updateMenu = false;

    startProgressBarThread(filename);

    while ((ch = getch())) {
        switch (ch) {
            /* TODO -do not resize under 108 * 24 */
            case KEY_RESIZE:
                {
                    resizeDetWin(filename);    
                    updateMenu = true;
                    /* Sleeping here fixes an issue where the old progress bar thread wouldn't have time 
                     * to terminate before a new one would start, causing a segfault / double free. 
                     * Ugly fix but I don't have time to investigate further */
                    //std::this_thread::sleep_for(std::chrono::milliseconds(100)); 
                    /* Exit detNav and come back to the main window for now */
                    //resizeUI();
                    //done = true;
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
    } 
    stopProgressBarThread();
    if (updateMenu) {
        return 1;
    }
    return 0;
}

/* Display a subwindow containing details about the selected download */ 
void dlManagerUI::progressBar(const std::string& filename)
{
    point maxyx = progressWin->getMaxyx();

    {
        std::lock_guard<std::mutex> guard(dlProgMutex);
        progRef = true;
    }

    /* TODO - remove hardcoded values */
    int progBarWidth = maxyx.x - 4;
    int i = 0;

    float progCounter = dlManagerControl->getProgress(filename);

    if (progCounter == 100.0) {
        std::string progStr;
        for (i = 0; i < progBarWidth; ++i) {
            progStr.push_back(' ');
        }

        {
            progressWin->printInMiddle(1, 0, maxyx.x, hundredPer, COLOR_PAIR(2));
            progressWin->winAttrOn(COLOR_PAIR(16));
            progressWin->addStr(2, 2, progStr);
            progressWin->winAttrOff(COLOR_PAIR(16));
            progressWin->refreshWin();
        }
    }

    else {
        while (true) {
            int curProg = progCounter * progBarWidth / 100.0;
            const std::string percent = stringifyNumber(dlManagerControl->getProgress(filename), 2); 
            std::string progStr;
            for (i = 0; i < curProg + 1; ++i) {
                progStr.push_back(' ');
            }

            {
                progressWin->printInMiddle(1, 0, maxyx.x, percent, COLOR_PAIR(2));
                progressWin->winAttrOn(COLOR_PAIR(16));
                progressWin->addStr(2, 2, progStr);
                progressWin->winAttrOff(COLOR_PAIR(16));
                progressWin->refreshWin();
            }
            progCounter = dlManagerControl->getProgress(filename);
            if (progCounter == 100) {
                break;
            }
            if (resizeDet) {
                progressWin->resizeWin(dlProgSz);
                progressWin->touchWin();
                progressWin->refreshWin();
                resizeDet = false;
            }
            {
                std::lock_guard<std::mutex> guard(dlProgMutex);
                if (!progRef)
                    break;
            }
            /* Sleep here to avoid uninterupted loop */
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
    }
}

