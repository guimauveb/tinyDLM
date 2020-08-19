#include "dlManagerUI.hpp"

/* Mutexes used to avoid concurrent uses of wrefresh() */
std::mutex dlManagerUI::dlsInfoMutex;
std::mutex dlManagerUI::dlProgMutex;

dlManagerUI::dlManagerUI()
    :dlManagerControl{std::make_unique<dlManagerController>()}
{
    initCurses();
    initColors();
    setWinsSize();
    mainWinsInit();
    refreshMainWins();        
    /* Display a nice message at first start */
    firstStart();
    /* Start downloads status thread */
    startStatusUpdate();
}

/* Free memory and exit curses */
dlManagerUI::~dlManagerUI()
{
    stopStatusUpdate();
    stopProgressBarThread();
    /* Pause all downloads -> we use pause to stop them since it does the same thing */
    dlManagerControl->pauseAll();

    /* Delete main windows */
    mainWindows.clear();

    /* TODO - save downloads list */
    /* Empty the downloads list menu */
    endwin();
    std::cout << "Goodbye." << std::endl;
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
    mainWinSz = {row - 4, col / 2, 3, 0};
    dlStatusSz = {row - 4, col / 2, 3, col / 2};
    keyActSz = {1, col / 2, row - 1, 0};
    dlInfosSz = {1, col / 2, row - 1, col / 2};

    dlAddSz = {row / 2, col - (col / 2), (row / 4), col / 4};
    dlDetSz = {row / 2, col - (col / 2), (row / 4), col / 4};
    dlProgSz = {4, (col - (col / 2)) -10, row / 2, col / 4 + 4};

    //return struct of winSizes
}

/* Window displaying a welcome message*/
std::unique_ptr<cursesWindow> dlManagerUI::welcomeWinInit()
{
    return std::make_unique<cursesWindow>(row - 8, col, 4, 0);
}

void dlManagerUI::paintWelWin(std::unique_ptr<cursesWindow>& welWin)
{
    welWin->printInMiddle(row / 4, 0, col,  tinyDLMWelcome, COLOR_PAIR(7));
    welWin->printInMiddle(row / 4 + 1, 0, col,  tinyDLMVer, COLOR_PAIR(7));
    welWin->printInMiddle(row / 4 + 2, 0, col,  tinyHelp, COLOR_PAIR(7));
}

void dlManagerUI::firstStart()
{
    std::unique_ptr<cursesWindow> welWin = welcomeWinInit();
    paintWelWin(welWin);
    welWin->refreshWin();
    /* Move to current active subwins so we know we have to resize them */

    /* Disable cursor because I can't print to the screen without moving it */
    curs_set(0);
    bool done = false;
    int ch = 0;
    while ((ch = getch())) {
        switch(ch) {
            case KEY_RESIZE:
                resizeUI();
                welWin->resizeWin(welWinSz);
                paintWelWin(welWin);
                welWin->refreshWin();
                break;
            case 'a':
                /* Open a window and exit the function  - program will continue in Browser() */
                addNewDl();
                done = true;
                break;
            case 'h':
                break;
            default:
                break;
        }
        if (done) {
            break;
        }
    }
    refreshMainWins();
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
}

void dlManagerUI::refreshMainWins()
{
    for (size_t i = 0; i < mainWindows.size(); ++i) {
        mainWindows.at(i)->touchWin();
        mainWindows.at(i)->refreshWin();
    }
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

    refreshMainWins();
}

/* Window containing top bar title */
std::unique_ptr<cursesWindow> dlManagerUI::mainWinTopBarInit()
{
    return std::make_unique<cursesWindow>(1, col, 0, 0);
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
    return std::make_unique<cursesWindow>(1, col, 2, 0);
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
    return std::make_unique<cursesWindow>(row - 4, col / 2, 3, 0);
}

void dlManagerUI::paintMainWinWin(std::unique_ptr<cursesWindow>& mainWInWin)
{
    // do somehting
}

/* Init the download progress subwindow */ 
std::unique_ptr<cursesWindow> dlManagerUI::mainWinKeyActInit()
{
    return std::make_unique<cursesWindow>(1, col / 2, row - 1, 0);
}

void dlManagerUI::paintKeyActWin(std::unique_ptr<cursesWindow>& keyActWin)
{
    /* Display keys and their associated functions at the bottom of the window */
    keyActWin->printInMiddle(0, 0, col / 2, msgKeyInfoP, COLOR_PAIR(8));
}

void dlManagerUI::updateKeyActWinMessage(bool& p)
{
    mainWindows.at(keyActWinIdx)->eraseWin();
    if (p) {
        /* Display 'resume' if paused */
        mainWindows.at(keyActWinIdx)->printInMiddle(0, 0, col / 2, msgKeyInfoR, COLOR_PAIR(8));
    }
    else {
        /* Display 'pause' if not paused */
        mainWindows.at(keyActWinIdx)->printInMiddle(0, 0, col / 2, msgKeyInfoP, COLOR_PAIR(8));
    }
}
/* init the global download info subwindow in a separated thread - displays number of downloads + speed */ 
//std::unique_ptr<cursesWindow> dlManagerUI::mainWinDlInfosInit()
//{
//    return std::make_unique<cursesWindow>(1, col / 2, row - 1, col / 2);
//}

/* init the downloads status window */
std::unique_ptr<cursesWindow> dlManagerUI::mainWinDownloadsStatusInit()
{
    return std::make_unique<cursesWindow>(row - 4, col / 2, 3, col / 2);
}

void dlManagerUI::paintDlsStatusWin(std::unique_ptr<cursesWindow>& dlsStatusWin)
{
    // do something
}

/* Populate the status window with downloads informations such as their status / speed / progress */
void dlManagerUI::populateStatusWin(const std::vector<downloadWinInfo> vec)
{
    int y = 1;
    /* Iterate over the list of downloads we obtained from dlManagerControl */
    for (auto& dl : vec) {
        /* TODO - paintStatusWin() */
        mainWindows.at(dlsStatusWinIdx)->printInMiddle(y, 0, row / 4, dl.progress, COLOR_PAIR(7));
        mainWindows.at(dlsStatusWinIdx)->printInMiddle(y, row / 4, row / 4, dl.speed, COLOR_PAIR(7));
        //mainWindow.at(dlsStatusWinIdx)->printInMiddledlsStatusWiny, 2 * width / 4, width / 4, dl.eta, COLOR_PAIR(7));
        mainWindows.at(dlsStatusWinIdx)->printInMiddle(y, 3 * row / 4,row / 4, dl.status, COLOR_PAIR(7));
        y++;
    }
}

void dlManagerUI::setDownloadsMenu()
{
    point pMax = mainWindows.at(dlsWinIdx)->getMaxyx();
    menu->menuOptsOn(O_SHOWDESC);
    menu->setMenuWin(mainWindows.at(dlsWinIdx));
    menu->setMenuSub(mainWindows.at(dlsWinIdx), pMax.y - 1, pMax.x - 4, 1, 2);
    menu->setMenuFormat(pMax.y - 6, 0);
    menu->setMenuMark(" * ");
    menu->postMenu();
}

/* toDO - see how I can save the current menu and only add one item without reposting the entire menu */
/* display downloads as a menu */
std::unique_ptr<cursesMenu> dlManagerUI::initDownloadsMenu(std::vector<std::string> itemsData)
{
    return std::make_unique<cursesMenu>(itemsData);
}

/* consistantly refresh the speed / progress /status of the current downloads */ 
/* toDO - y offset corresponding to the active menu page */
void dlManagerUI::updateDownloadsStatusWindow()
{
    while (true) {
        {
            std::lock_guard<std::mutex> guard(dlManagerUI::dlsInfoMutex);
            if (!updateStatus) {
                break;
            }
        }
        /* Sleep 100ms before refreshing the window again or the while loop will execute endlessly 
         * so it doesn't monopolize time / ressources */ 
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        /* y represents the y postion of the infos to print on the screen - it matches the location
         * of the corresponding download item on the futureUpdateDlsStatus part of the screen */
        populateStatusWin(dlManagerControl->getAllDownloadsInfos());

        /* Refresh status window */
        {
            std::lock_guard<std::mutex> guard(dlManagerUI::dlsInfoMutex);
            mainWindows.at(dlsStatusWinIdx)->touchWin();
            mainWindows.at(dlsStatusWinIdx)->refreshWin();
        }
    }
}
/* end main windows */

void dlManagerUI::startStatusUpdate()
{
    /* If no downloads - erase the window and break out of here */
    if (!dlManagerControl->isActive()) {
        std::lock_guard<std::mutex> guard(dlManagerUI::dlsInfoMutex);
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
        std::lock_guard<std::mutex> guard(dlManagerUI::dlsInfoMutex);
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


/* Navigate the download manager menus */
void dlManagerUI::Browser()
{
    bool EXIT = false;
    bool PAUSEDALL = false;
    int ch = 0;
    while ((ch = getch()) != KEY_F(1)) {
        switch(ch) {
            case KEY_RESIZE:
                {
                    stopStatusUpdate();
                    resizeUI();
                    updateDownloadsMenu(dlManagerControl->getDownloadsList());
                    startStatusUpdate();
                    break;
                }

            case KEY_DOWN:
                {
                    /* Make sure that the menu is not be empty */
                    if (dlManagerControl->isActive()) {
                        menu->menuDriver(REQ_DOWN_ITEM);
                    }
                    break;
                }

            case KEY_UP:
                {
                    /* Make sure that the menu is not be empty */
                    if (dlManagerControl->isActive()) {
                        menu->menuDriver(REQ_UP_ITEM);
                    }
                    break;
                }

            case 10:
                /* Enter - user has selected an item */
                {
                    /* Make sure that the menu is not empty */
                    if (!dlManagerControl->isActive()) {
                        break;
                    }
                    stopStatusUpdate();
                    showDetails(menu->getItemName());
                    updateDownloadsMenu(dlManagerControl->getDownloadsList());
                    startStatusUpdate(); 
                    break;
                }
                /* Opens a subwindow in which we'll enter the download item info */
            case 'a':
                {
                    /* Stop status thread */
                    stopStatusUpdate();
                    /* Open the window containing a form in which the user enters the download infos */
                    addNewDl();
                    updateDownloadsMenu(dlManagerControl->getDownloadsList());
                    startStatusUpdate();
                    break;
                }

                /* TODO - Display help window */
            case 'h':
                {
                    break;
                }

                /* Pause all active downloads */
            case 'p':
                {
                    if (!dlManagerControl->isActive()) {
                        break;
                    }
                    const std::string tmp = menu->getItemName();
                    dlManagerControl->stop(tmp);
                    break;
                }
            case 'P':
                {
                    if (!dlManagerControl->isActive()) {
                        break;
                    }
                    /* Break if already paused */
                    if (PAUSEDALL)
                        break;
                    stopStatusUpdate();
                    dlManagerControl->pauseAll();
                    PAUSEDALL = true;
                    /* Update key actions window */
                    updateKeyActWinMessage(PAUSEDALL);
                    startStatusUpdate();
                    break;
                }

            case 'r':
                {
                    if (!dlManagerControl->isActive()) {
                        break;
                    }
                    stopStatusUpdate();
                    dlManagerControl->resumeAll();
                    PAUSEDALL = false;
                    /* Update key actions window */
                    updateKeyActWinMessage(PAUSEDALL);
                    startStatusUpdate();
                    break;
                }

            case 'c':
                /* TODO - Clear inactive downloads - see where the bug comes from */
                /* TODO - update menu */
                {
                    stopStatusUpdate();
                    dlManagerControl->clearInactive();
                    mainWindows.at(dlsStatusWinIdx)->resetWin();
                    updateDownloadsMenu(dlManagerControl->getDownloadsList());
                    startStatusUpdate();
                    break;
                }

                /* Kill all downloads */
            case 'k':
                {
                    stopStatusUpdate();
                    dlManagerControl->killAll();
                    updateDownloadsMenu(dlManagerControl->getDownloadsList());
                    PAUSEDALL = false;
                    /* Erase all windows and start from scratch */
                    /* TODO - routine here */
                    mainWindows.at(dlsStatusWinIdx)->resetWin();
                    mainWindows.at(keyActWinIdx)->eraseWin();
                    mainWindows.at(keyActWinIdx)->printInMiddle(0, 0, 0, msgKeyInfoP, COLOR_PAIR(8));
                    mainWindows.at(keyActWinIdx)->refreshWin();
                    startStatusUpdate();
                    break;
                }

                /* Terminates the program */
            case CTRL('t'):
                {
                    stopStatusUpdate();
                    EXIT = true;
                    break;
                }

                /* TODO - catch sigint - exit gracefully */
            case CTRL('z'):
                {
                    //stopStatusUpdate();
                    break;
                }

            default:
                {
                    break;
                }
        }

        /* Consistantly update main windows */
        {
            std::lock_guard<std::mutex> guard(dlManagerUI::dlsInfoMutex);
            refreshMainWins();        
        }

        if (EXIT) {
            break;
        }
    }
}

/* Add a new download */
void dlManagerUI::addNewDl()
{
    curs_set(1);   
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
    addDlNav();
    /* No need to call free() for win and form -> class destructor will take care of it */
    curs_set(0);   
}

/* Init "Add a download" window */
std::unique_ptr<cursesWindow> dlManagerUI::addDlInitWin()
{
    return std::make_unique<cursesWindow>(row / 2, col - (col / 2), (row / 4), col / 4);
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
    addDlWin->printInMiddle(1, 0, maxyx.x , titleAdd, COLOR_PAIR(8));
    addDlWin->printInMiddle(maxyx.y - 2, 0, maxyx.x / 3, msgStart, COLOR_PAIR(8));
    addDlWin->printInMiddle(maxyx.y - 2, 0, maxyx.x, msgScheduleCtrl, COLOR_PAIR(8));
    addDlWin->printInMiddle(maxyx.y - 2, 2 * maxyx.x / 3, maxyx.x / 3, msgCloseCtrl, COLOR_PAIR(8));
    addDlWin->drawBox(0, 0);
}

std::unique_ptr<cursesForm> dlManagerUI::initAddDlForm(int numFields)
{
    return std::make_unique<cursesForm>(numFields);
}

std::unique_ptr<cursesForm> dlManagerUI::initDetForm(int numFields)
{
    return std::make_unique<cursesForm>(numFields);
}

void dlManagerUI::setAddDlForm()
{
    point maxyx = addDlWin->getMaxyx();

    /* Set field size and location */
    addDlForm->setField(0, 1, maxyx.x - 10, 3, 4, 0, 0);
    endwin();
    std::cout << maxyx.y << '\n';
    addDlForm->setField(1, 1, maxyx.x - 10, 6, 4, 0, 0);

    /* Set field options */
    addDlForm->setFieldBack(0, A_UNDERLINE);
    addDlForm->setFieldBack(0, O_AUTOSKIP);
    addDlForm->fieldOptsOff(0, O_STATIC);
    addDlForm->setFieldBack(1, A_UNDERLINE);
    addDlForm->setFieldBack(1, O_AUTOSKIP);
    addDlForm->fieldOptsOff(1, O_STATIC);

    /* Initialize addDlForm */
    addDlForm->initForm();
    addDlForm->setFormWin(addDlWin);
    addDlForm->setFormSubwin(addDlWin, 20, 10, maxyx.y / 2, 2);

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
void dlManagerUI::addDlNav()
{
    point maxyx = addDlWin->getMaxyx();
    int ch = 0;
    bool done = false;
    while ((ch = getch())) {
        switch (ch) {
            case KEY_RESIZE:
                /* TODO - save all user input then update */
                {
                    if (addDlForm->formDriver(REQ_VALIDATION) != E_OK) {
                        ;
                    }
                    const std::string url = addDlForm->getFieldBuffer(0);
                    const std::string filename = addDlForm->getFieldBuffer(1);
                    resizeAddDlNav(url, filename);
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
                        /* 50 char max for the filename TODO */
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
                        dlManagerControl->createNewDl(dlFolder, filenameCleaned, URLcleaned,  0, 0);
                        dlManagerControl->startDl(filenameCleaned);
                        /* Update downloads list in the menu */
                        updateDownloadsMenu(dlManagerControl->getDownloadsList());
                        /* Restart status thread */
                        done = true;
                        break;
                    }
                }
                break;
            default:
                addDlForm->formDriver(ch);
                break;
        }
        if (done) {
            /* Exit subaddDlWin if done */
            break;
        }
        /* No mutex needed since all threads have stopped */
        addDlWin->refreshWin();
        /* TODO - get rid of them */
    }
}

void dlManagerUI::updateDownloadsMenu(std::vector<std::string> vec)
{
    menu = initDownloadsMenu(vec);
    setDownloadsMenu();
}

/* Init a subwindow containg infos about the selected download */
void dlManagerUI::showDetails(const std::string itemName)
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

    curs_set(0);

    /* detWin is only used to get progressWin dimensions - not used in the actual thread */
    startProgressBarThread(itemName);
    /* Disable cursor */
    /* Navigate through the details window */
    detNav(itemName);
    stopProgressBarThread();
}

std::unique_ptr<cursesWindow> dlManagerUI::initDetWin(/* pass winsize */)
{
    return std::make_unique<cursesWindow>(row / 2, col - (col / 2), (row / 4), col / 4);
}

void dlManagerUI::paintDetWin(const std::string itemName)
{
    point maxyx = detWin->getMaxyx();
    detWin->drawBox(0, 0);

    detWin->printInMiddle(maxyx.y - 2, 0, maxyx.x/ 4, msgClose, COLOR_PAIR(8));
    detWin->printInMiddle(maxyx.y - 2, maxyx.x / 4, maxyx.x / 4, msgPause, COLOR_PAIR(8));
    detWin->printInMiddle(maxyx.y - 2, 2 * maxyx.x / 4, maxyx.x / 4, msgResume, COLOR_PAIR(8));
    detWin->printInMiddle(maxyx.y - 2, 3 * maxyx.x / 4, maxyx.x / 4, msgKill, COLOR_PAIR(8));
    detWin->printInMiddle(1, 0, maxyx.x, initDetailsTitle(itemName), COLOR_PAIR(8));
}

const std::string dlManagerUI::initDetailsTitle(const std::string itemName)
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
    detForm->setFormSubwin(detWin, 20, 10, row / 2, 2);

    detForm->postForm();
}

/* shared_ptr that will be moved to progressWin own thread */
std::unique_ptr<cursesWindow> dlManagerUI::initProgressWin(point begyx, point maxyx)
{
    return std::make_unique<cursesWindow>(4, maxyx.x-10, maxyx.y, begyx.x+4);
}

void dlManagerUI::startProgressBarThread(const std::string filename)
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
        std::lock_guard<std::mutex> guard(dlManagerUI::dlProgMutex);
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

int dlManagerUI::resizeDetWin(const std::string filename)
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
void dlManagerUI::detNav(const std::string filename)
{
    int ch = 0;
    bool done = false;

    while ((ch = getch())) {
        switch (ch) {
            case KEY_RESIZE:
                {
                    resizeDetWin(filename);    
                    /* Sleep for a while to avoid uniterrupted calls to resizeDetWin() */
                    /* On Linux I can't seem to figure out why the program crashes when we resize the terminal
                     * window more than once in a few seconds interval while it works perfectly fine on
                     * macOS (if we don't resize the window like a maniac ... 
                     * Sleeping for a few milliseconds seems to avoid the crash on Linux... */
                    //std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    break;
                }

            case 'r':
                /* Resume transfer - if the transfer is ongoing - does nothing */
                {
                    std::lock_guard<std::mutex> guard(dlManagerUI::dlsInfoMutex);
                    dlManagerControl->resume(filename);
                }
                break;

            case 'p':
                /* Pause transfer - if the transfer is already paused - does nothing */
                {
                    std::lock_guard<std::mutex> guard(dlManagerUI::dlsInfoMutex);
                    dlManagerControl->pause(filename);
                }
                break;

            case 'x':
                /* Close subwindow */
                done = true;
                break;
            default:
                break;
        }
        if (done) {
            break;
        }
    } 
}

/* Display a subwindow containing details about the selected download */ 
void dlManagerUI::progressBar(const std::string filename)
{
    point maxyx = progressWin->getMaxyx();

    {
        std::lock_guard<std::mutex> guard(dlManagerUI::dlProgMutex);
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

            {
                std::lock_guard<std::mutex> guard(dlManagerUI::dlProgMutex);
                if (!progRef)
                    break;
            }


            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}

