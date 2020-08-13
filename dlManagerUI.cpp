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
    initDownloadsMenu();
    refreshMainWins();        
    /* Display a nice message at first start */
    firstStart();
    /* Start downloads status thread */
    startStatusUpdate();
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

/* TODO */
void dlManagerUI::resizeUI()
{
    /* Update window sizes */
    // TODO - will return a struct of winSizes objects */
    setWinsSize();

    mainWindows.at(topWinIdx)->resizeWin(topBarSz);
    paintTopWin(mainWindows.at(topWinIdx));

    mainWindows.at(labelsWinIdx)->resizeWin(labelsSz);
    paintLabelsWin(mainWindows.at(labelsWinIdx));

    /* TODO - properly update menu */
    /* Refresh the downloads list */
    mainWindows.at(dlsWinIdx)->resizeWin(mainWinSz);
    paintMainWinWin(mainWindows.at(dlsWinIdx));

    mainWindows.at(dlsStatusWinIdx)->resizeWin(dlStatusSz);
    paintDlsStatusWin(mainWindows.at(dlsStatusWinIdx));

    mainWindows.at(keyActWinIdx)->resizeWin(keyActSz);
    paintKeyActWin(mainWindows.at(keyActWinIdx));

    refreshMainWins();
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

/* TODO - firststart() */
void dlManagerUI::firstStart()
{
    std::unique_ptr<cursesWindow> welWin = welcomeWinInit();
    paintWelWin(welWin);
    welWin->refreshWin();
    /* Move to current active subwins so we know we have to resize them */
    //activeSubwins.emplace_back(std::move(welWin));
    /* Disable cursor because I can't print to the screen without moving it */
    curs_set(0);
    bool done = false;
    int ch = 0;
    while ((ch = getch())) {
        switch(ch) {
            case KEY_RESIZE:
                resizeUI();
                /* TODO - move to resizeUI */
                //welWin->resizeWin(welWinSz);
                //paintWelWin(welWin);
                //welWin->refreshWin();
                break;
            case 'a':
                /* TODO - erase welcome message here */
                /* TODOint - create a window to display it */
                done = true;
                break;
            case 'h':
                break;
            default:
                break;
        }
        if (done) {
            welWin->eraseWin();
            welWin->refreshWin();
            /* Open an window and exiting the constructor - program will continue in browser() */
            addNewDl();
            break;
        }
    }
    refreshMainWins();
    activeSubwins.clear();
}

void dlManagerUI::refreshMainWins()
{
    for (size_t i = 0; i < mainWindows.size(); ++i) {
        mainWindows.at(i)->touchWin();
        mainWindows.at(i)->refreshWin();
    }
}

/* Free memory and exit curses */
dlManagerUI::~dlManagerUI()
{
    stopStatusUpdate();
    /* Pause all downloads -> we use pause to stop them since it does the same thing */
    dlManagerControl->pauseAll();

    /* Delete active subwindows */
    // TODO
    /* Delete main windows */
    mainWindows.clear();


    /* TODO - save downloads list */
    /* Empty the downloads list menu */
    //    downloadsClearMenu(my_items, &menu, freeMenuDls);
    endwin();
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
    if (!(futureIsReady(ft))) {
        while (!futureIsReady(ft))
            //wait unitl execution is done
            ;
    } 
    return 0;
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
    if (!(futureIsReady(ftP))) {
        while (!futureIsReady(ftP))
            //wait unitl execution is done
            ;
    } 
    return 0;
}

void dlManagerUI::startStatusUpdate()
{
    /* If no downloads - erase the window and break out of here */
    if (!dlManagerControl->isActive()) {
        std::lock_guard<std::mutex> guard(dlManagerUI::dlsInfoMutex);
        mainWindows.at(dlsStatusWinIdx)->refreshWin();
    }
    else {
        updateStatus = true;
        ft = std::async(std::launch::async, &dlManagerUI::updateDownloadsStatusWindow, this);
    }
}

/* TODO - move to main ? */
/* Navigate the download manager menus */
void dlManagerUI::Browser()
{
    /* TODO - see what variables can be defined here */
    int ch = 0;
    bool EXIT = false;
    bool PAUSEDALL = false;
    /* TODO - create a function for each key */
    while ((ch = getch()) != KEY_F(1)) {
        switch(ch) {
            case KEY_RESIZE:
                {
                    /* TODO - reset menu */
                    resizeUI();
                    updateDownloadsMenu(dlManagerControl->getDownloadsList());
                    break;
                }

            case KEY_DOWN:
                {
                    if (dlManagerControl->isActive()) {
                        menu->menuDriver(REQ_DOWN_ITEM);
                    }
                    break;
                }

            case KEY_UP:
                {
                    if (dlManagerControl->isActive()) {
                        menu->menuDriver(REQ_UP_ITEM);
                    }
                    break;
                }

                /* Enter - user has selected an item */
            case 10:
                {
                    /* Make sure that the menu is not be empty */
                    if (!dlManagerControl->isActive()) {
                        break;
                    }
                    stopStatusUpdate();
                    /* TODO - move to getItemInfo() */
                    /* Select the current item and open a subwindow showing its details */
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
                    std::string tmp = menu->getItemName();
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
                    /* updateKeyActWinMessage() */
                    mainWindows.at(keyActWinIdx)->eraseWin();
                    mainWindows.at(keyActWinIdx)->printInMiddle(0, 0, col / 2, msgKeyInfoR, COLOR_PAIR(8));
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
                    /* updateKeyActWinMessage() */
                    mainWindows.at(keyActWinIdx)->eraseWin();
                    mainWindows.at(keyActWinIdx)->printInMiddle(0, 0, col / 2, msgKeyInfoP, COLOR_PAIR(8));
                    startStatusUpdate();
                    break;
                }

                /* TODO - Clear inactive downloads - see where the bug comes from */
            case 'c':
                {
                    stopStatusUpdate();
                    dlManagerControl->clearInactive();
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

void dlManagerUI::updateKeyActWinMessage()
{
    /* TODO - updateKeyActMessage() */
    mainWindows.at(keyActWinIdx)->eraseWin();
    mainWindows.at(keyActWinIdx)->printInMiddle(0, 0, col / 2, msgKeyInfoR, COLOR_PAIR(8));
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

    //mainWindows.emplace_back(mainWinDlInfosInit());
    //paintTopWin(mainWindows.at(5));
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

void dlManagerUI::setDownloadsMenu()
{
    point pMax = mainWindows.at(dlsWinIdx)->getMaxyx();
    const std::string mark = " * ";
    menu->menuOptsOn(O_SHOWDESC);
    menu->setMenuWin(mainWindows.at(dlsWinIdx));
    menu->setMenuSub(mainWindows.at(dlsWinIdx), pMax.y - 1, pMax.x - 4, 1, 2);
    menu->setMenuFormat(pMax.y - 6, 0);
    menu->setMenuMark(mark.c_str());
    menu->postMenu();
}

/* toDO - see how I can save the current menu and only add one item without reposting the entire menu */
/* display downloads as a menu */
void dlManagerUI::initDownloadsMenu()
{
    menu = std::make_shared<cursesMenu>();
}

void dlManagerUI::initDownloadsMenu(std::vector<std::string> itemsData)
{
    menu = std::make_shared<cursesMenu>(itemsData);
}

/* init the downloads status window */
std::unique_ptr<cursesWindow> dlManagerUI::mainWinDownloadsStatusInit()
{
    return std::make_unique<cursesWindow>(row - 4, col / 2, 3, col / 2);
}

void dlManagerUI::paintDlsStatusWin(std::unique_ptr<cursesWindow>& dlsStatusWin)
{
    // do something
}

/* consistantly refresh the speed / progress /status of the current downloads */ 
/* toDO - y offset corresponding to the active menu page */
void dlManagerUI::updateDownloadsStatusWindow()
{
    std::map<std::string, int>::iterator it;
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
         * of the corresponding download item on the left part of the screen */
        std::vector<downloadWinInfo> vec;
        dlManagerControl->getAllDownloadsInfos(vec); 
        populateStatusWin(vec);
        /* Refresh status window */
        {
            std::lock_guard<std::mutex> guard(dlManagerUI::dlsInfoMutex);
            mainWindows.at(dlsStatusWinIdx)->touchWin();
            mainWindows.at(dlsStatusWinIdx)->refreshWin();
        }
    }
}

/* Populate the status window with downloads informations about their status / speed / progress */
void dlManagerUI::populateStatusWin(std::vector<downloadWinInfo>& vec)
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

/* init the global download info subwindow in a separated thread - displays number of downloads + speed */ 
std::unique_ptr<cursesWindow> dlManagerUI::mainWinDlInfosInit()
{
    return std::make_unique<cursesWindow>(1, col / 2, row - 1, col / 2);
}
/* end main windows */

/* Add a new download */
void dlManagerUI::addNewDl()
{
    curs_set(1);   
    std::shared_ptr<cursesWindow> addDlWin = addDlInitWin();
    std::shared_ptr<cursesForm> addForm = initForm(2);
    setAddDlForm(addForm, addDlWin);
    paintAddDlWin(addDlWin);
    addDlWin->refreshWin();
    addDlNav(addDlWin, addForm);
    /* No need to call free() for win and form -> class destructor will take care of it */
    curs_set(0);   
}

/* Init "Add a download" window */
std::unique_ptr<cursesWindow> dlManagerUI::addDlInitWin()
{
    return std::make_unique<cursesWindow>(row / 2, col - (col / 2), (row / 4), col / 4);
}

void dlManagerUI::paintAddDlWin(std::shared_ptr<cursesWindow> addDlWin)
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

std::shared_ptr<cursesForm> dlManagerUI::initFormShared(int numFields)
{
    return std::make_unique<cursesForm>(numFields);
}


std::unique_ptr<cursesForm> dlManagerUI::initForm(int numFields)
{
    return std::make_unique<cursesForm>(numFields);
}

void dlManagerUI::setAddDlForm(std::shared_ptr<cursesForm> form, std::shared_ptr<cursesWindow> win)
{
    point maxyx = win->getMaxyx();

    /* Set field size and location */
    form->setField(0, 1, maxyx.x - 10, 3, 4, 0, 0);
    form->setField(1, 1, maxyx.x - 10, 6, 4, 0, 0);

    /* Set field options */
    form->setFieldBack(0, A_UNDERLINE);
    form->setFieldBack(0, O_AUTOSKIP);
    form->fieldOptsOff(0, O_STATIC);
    form->setFieldBack(1, A_UNDERLINE);
    form->setFieldBack(1, O_AUTOSKIP);
    form->fieldOptsOff(1, O_STATIC);

    /* Initialize form */
    form->initForm();
    form->setFormWin(win);
    form->setFormSubwin(win, 20, 10, row / 2, 2);

    form->postForm();
}

/* Navigate through the 'Add a download' window */
void dlManagerUI::addDlNav(std::shared_ptr<cursesWindow> win, std::shared_ptr<cursesForm> form)
{
    point maxyx = win->getMaxyx();
    int ch = 0;
    bool done = false;
    while ((ch = getch())) {
        switch (ch) {
            case KEY_RESIZE:
                /* TODO - save all user input then update */
                {
                    resizeUI();
                    /* TODO - move to resizeUI() */
                    win->resizeWin(dlAddSz);
                    paintAddDlWin(win);
                    /* TODO */
                    form->saveFieldBuffer();
                    //form->restoreFieldBuffer();
                    win->refreshWin();
                }
                break;

            case KEY_UP:
                form->formDriver(REQ_PREV_FIELD);
                form->formDriver(REQ_END_LINE);
                break;

            case KEY_DOWN:
                form->formDriver(REQ_NEXT_FIELD);
                form->formDriver(REQ_END_LINE);
                break;

            case KEY_LEFT:
                form->formDriver(REQ_PREV_CHAR);
                break;

            case KEY_RIGHT:
                form->formDriver(REQ_NEXT_CHAR);
                break;

            case 127:
                form->formDriver(REQ_DEL_PREV);
                break;

            case KEY_DC:
                form->formDriver(REQ_DEL_CHAR);
                break;

            case CTRL('x'):
                /* Close window */
                done = true;
                break;

            case CTRL('l'):
                /* TODO - schedule ! */
                done = true;
                break;

            case 10:
                {
                    if (form->formDriver(REQ_VALIDATION) != E_OK) {
                        //check error 
                        ;
                    }
                    else {
                        /* TODO - create a checUserInput class / function that will call everyone */
                        /* Get user input from the fields */
                        std::string url = form->getFieldBuffer(0);
                        std::string filename = form->getFieldBuffer(1);
                        std::string URLcleaned = trimSpaces(url);                        
                        std::string filenameCleaned =trimSpaces(filename);
                        /* Must be at least 7 char long -> http:// */
                        if (checkURL(url) == 1) {
                            win->printInMiddle(maxyx.y - 4, 0, maxyx.x, msgInvalidURL, COLOR_PAIR(1));
                            break;
                        }
                        if (!checkFilename(filename)){
                            win->printInMiddle(maxyx.y - 4, 0, maxyx.x, msgInvalidFilename, COLOR_PAIR(1));
                            break;
                        }
                        URLcleaned.push_back('\0');
                        filenameCleaned.push_back('\0');
                        /* Start dl if everything ok */
                        dlManagerControl->createNewDl(dlFolder, filename, url,  0, 0);
                        dlManagerControl->startDl(filename);
                        /* Update downloads list in the menu */
                        updateDownloadsMenu(dlManagerControl->getDownloadsList());
                        /* Restart status thread */
                        done = true;
                        break;
                    }
                }
                break;
            default:
                form->formDriver(ch);
                break;
        }
        if (done) {
            /* Exit subwin if done */
            break;
        }
        /* No mutex needed since all threads have stopped */
        win->refreshWin();
        /* TODO - get rid of them */
    }
}

void dlManagerUI::updateDownloadsMenu(std::vector<std::string> vec)
{
    initDownloadsMenu(vec);
    setDownloadsMenu();
}

/* Init a subwindow containg infos about the selected download */
void dlManagerUI::showDetails(std::string itemName)
{
    std::shared_ptr<cursesWindow> detWin = initDetWin();
    std::shared_ptr<cursesForm> detForm = initForm(2);
    setDetForm(detWin, detForm);
    paintDetWin(detWin, itemName);

    detForm->populateField(REQ_FIRST_FIELD, dlManagerControl->getURL(itemName));
    detForm->populateField(REQ_LAST_FIELD, itemName);
    detWin->refreshWin();

    /* TODO - detWin is only used to get progressWin dimensions - not used in the actual thread */
    startProgressBarThread(detWin, itemName);

    /* Navigate through the details window */
    detNav(detWin, detForm, itemName);

    stopProgressBarThread();
}

std::shared_ptr<cursesWindow> dlManagerUI::initDetWin(/* pass winsize */)
{
    return std::make_shared<cursesWindow>(row / 2, col - (col / 2), (row / 4), col / 4);
}

void dlManagerUI::paintDetWin(std::shared_ptr<cursesWindow> detWin, std::string& itemName)
{
    point maxyx = detWin->getMaxyx();
    detWin->drawBox(0, 0);

    detWin->printInMiddle(maxyx.y - 2, 0, maxyx.x/ 4, msgClose, COLOR_PAIR(8));
    detWin->printInMiddle(maxyx.y - 2, maxyx.x / 4, maxyx.x / 4, msgPause, COLOR_PAIR(8));
    detWin->printInMiddle(maxyx.y - 2, 2 * maxyx.x / 4, maxyx.x / 4, msgResume, COLOR_PAIR(8));
    detWin->printInMiddle(maxyx.y - 2, 3 * maxyx.x / 4, maxyx.x / 4, msgKill, COLOR_PAIR(8));
    detWin->printInMiddle(1, 0, maxyx.x, initDetailsTitle(itemName), COLOR_PAIR(8));
}

std::string dlManagerUI::initDetailsTitle(std::string itemName)
{
    const std::string labelTruncated = " ... - Details ";
    const std::string label = " - Details ";
    itemName.reserve(itemName.length() + 1);
    itemName.insert(itemName.begin(), ' ');
    if (itemName.length() > 20) {
        itemName.resize(20);
        itemName.append(labelTruncated);
    }
    else {
        itemName.append(label);
    }
    return itemName;
}

/* Init details window form in order to populate */
void dlManagerUI::setDetForm(std::shared_ptr<cursesWindow> win, std::shared_ptr<cursesForm> form)
{
    point maxyx = win->getMaxyx();

    /* Set field size and location */
    form->setField(0, 1, maxyx.x - 10, 3, 4, 0, 0);
    form->setField(1, 1, maxyx.x - 10, 6, 4, 0, 0);

    /* Set field options */
    form->setFieldBack(0, A_UNDERLINE);
    form->setFieldBack(0, O_AUTOSKIP);
    form->fieldOptsOff(0, O_STATIC);
    form->setFieldBack(1, A_UNDERLINE);
    form->setFieldBack(1, O_AUTOSKIP);
    form->fieldOptsOff(1, O_STATIC);

    /* Initialize form */
    form->initForm();
    form->setFormWin(win);
    form->setFormSubwin(win, 20, 10, row / 2, 2);

    form->postForm();
}

/* shared_ptr that will be moved to progressWin own thread */
std::shared_ptr<cursesWindow> dlManagerUI::initProgressWin(point begyx, point maxyx)
{
    return std::make_shared<cursesWindow>(4, maxyx.x-10, maxyx.y, begyx.x+4);
}

void dlManagerUI::startProgressBarThread(std::shared_ptr<cursesWindow> detWin, std::string& filename)
{
    /* Initialize progress bar according to its parent win (details win) dimensions */
    point begyx = detWin->getBegyx();
    point maxyx = detWin->getMaxyx();
    auto progressWin = initProgressWin(begyx, maxyx);
    progressWin->drawBox(0, 0);
    ftP = std::async(std::launch::async, &dlManagerUI::progressBar, this, std::move(progressWin), filename);
}

void dlManagerUI::resetDetWin(std::shared_ptr<cursesWindow> win, std::shared_ptr<cursesForm> form, 
        std::string filename)
{
    stopProgressBarThread();
    resizeUI();

    win = initDetWin();
    form = initForm(2);
    setDetForm(win, form);
    paintDetWin(win, filename);
    form->populateField(REQ_FIRST_FIELD, dlManagerControl->getURL(filename));
    form->populateField(REQ_LAST_FIELD, filename);
    win->refreshWin();

    startProgressBarThread(win, filename);
}

/* Navigate through a download details subwindow */
void dlManagerUI::detNav(std::shared_ptr<cursesWindow> win, std::shared_ptr<cursesForm> form, 
        std::string filename)
{
    int ch = 0;
    bool done = false;
    curs_set(0);

    /* Make sure ch gets updated for every loop iteration ! */
    while ((ch = getch())) {
        switch (ch) {
            case KEY_RESIZE:
                {
                    resetDetWin(win, form, filename);    
                    break;
                }
            case 'r':
                /* Resume transfer - if the transfer is ongoing - does nothing */
                {
                    std::lock_guard<std::mutex> guard(dlManagerUI::dlsInfoMutex);
                    /* TODO - dlManagerController here */
                    dlManagerControl->resume(filename);
                }
                break;
            case 'p':
                /* Pause transfer - if the transfer is already paused - does nothing */
                {
                    std::lock_guard<std::mutex> guard(dlManagerUI::dlsInfoMutex);
                    /* TODO - dlManagerController here */
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
void dlManagerUI::progressBar(std::shared_ptr<cursesWindow> progressWin, std::string filename)
{
    point maxyx = progressWin->getMaxyx();
    /* Pass entire function to thread ! */
    progRef = true;
    /* TODO - remove hardcoded values */
    int progBarWidth = col - 14;
    int i = 0;

    /* TODO - dlManagerController here */
    float progCounter = dlManagerControl->getProgress(filename);

    if (progCounter == 100.0) {
        /* TODO - paintFullProgressBar() */
        std::string progStr;
        for (i = 0; i < progBarWidth; ++i)
            progStr.push_back(' ');
        {
            /* TODO - temporary */
            std::lock_guard<std::mutex> guard(dlProgMutex);
            /* TODO - updateProgWin() */
            progressWin->printInMiddle(1, 0, maxyx.x, hundredPer, COLOR_PAIR(2));
            progressWin->winAttrOn(COLOR_PAIR(16));
            progressWin->addStr(2, 2, progStr);
            progressWin->winAttrOff(COLOR_PAIR(16));
            progressWin->refreshWin();
        }
    }

    else {
        while (progCounter != 100.0) {
            /* Write char until value of progress */
            std::string progStr;
            int curProg = progCounter * progBarWidth / 100;
            /* floorf */
            std::string percent = stringifyNumber(dlManagerControl->getProgress(filename), 2); 
            for (i = 0; i < curProg ; ++i)
                progStr.push_back(' ');
            {
                std::lock_guard<std::mutex> guard(dlProgMutex);
                progressWin->printInMiddle(1, 0, maxyx.x, percent, COLOR_PAIR(2));
                progressWin->winAttrOn(COLOR_PAIR(16));
                progressWin->addStr(2, 2, progStr);
                progressWin->winAttrOff(COLOR_PAIR(16));
                progressWin->refreshWin();
            }
            if (!progRef)
                break;
            progCounter = dlManagerControl->getProgress(filename);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}

