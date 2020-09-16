#include "../UI/include/dlManagerUI.hpp"

/* Initialize our dlManager user interface and browse it */
int main()
{
    std::unique_ptr<dlManagerUI> dlmc = std::make_unique<dlManagerUI>();
    /* Display a message when starting the program. If the user exits while in firstStart(), terminate the 
     * program */
    if (dlmc->firstStart()) {
        return 1;
    }

    /* Navigate through the tinyDLM UI */
    {
        int ch = 0;
        bool resizeUI = false;

        while ((ch = getch()) != KEY_F(1)) {
            switch(ch) {
                /* TODO - do not resize under 108 * 24 */
                case KEY_RESIZE:
                    {
                        resizeUI = true;
                        break;
                    }

                case KEY_DOWN:
                    {
                        /* Make sure that the menu is not empty */
                        if (dlmc->dlManagerControl->isActive()) {
                            dlmc->menu->menuDriver(REQ_DOWN_ITEM);
                            dlmc->statusDriver(KEY_DOWN);
                        }
                        break;
                    }

                case KEY_UP:
                    {
                        /* Make sure that the menu is not empty */
                        if (dlmc->dlManagerControl->isActive()) {
                            dlmc->menu->menuDriver(REQ_UP_ITEM);
                            dlmc->statusDriver(KEY_UP); 
                        }
                        break;
                    }

                case 10:
                    /* Enter - user has selected an item */
                    {

                        /* Make sure that the menu is not empty */
                        if (!dlmc->dlManagerControl->isActive()) {
                            break;
                        }
                        dlmc->stopStatusUpdate();
                        
                        /* showDetails returns true if the download was killed and that we should update the
                         * menu */
                        if (dlmc->showDetails(dlmc->menu->getItemName())) {
                            dlmc->updateDownloadsMenu();
                            dlmc->resetStatusDriver();
                        }
                        
                        dlmc->startStatusUpdate(); 
                        break;
                    }

                case 'a':
                    /* Opens a subwindow in which we'll enter the new download item info */
                    {
                        dlmc->stopStatusUpdate();
                        if (dlmc->addNewDl()) {
                            dlmc->updateDownloadsMenu();
                            dlmc->resetStatusDriver();
                        }
                        dlmc->startStatusUpdate();
                        break;
                    }

                case 'h':
                    /* TODO - Display a help window */
                    {
                        dlmc->stopStatusUpdate();
                        dlmc->showHelp();
                        dlmc->startStatusUpdate();
                        break;
                    }

                case 'p':
                    /* Pause highlighted download */
                    {
                        if (!dlmc->dlManagerControl->isActive()) {
                            break;
                        }
                        dlmc->dlManagerControl->pause(dlmc->menu->getItemName());
                        break;
                    }

                case 'P':
                    /* Pause all active downloads */
                    {
                        if (!dlmc->dlManagerControl->isActive()) {
                            break;
                        }
                        dlmc->stopStatusUpdate();
                        dlmc->dlManagerControl->pauseAll();
                        dlmc->startStatusUpdate();
                        break;
                    }

                case 'r':
                    {
                        if (!dlmc->dlManagerControl->isActive()) {
                            break;
                        }
                        dlmc->stopStatusUpdate();
                        dlmc->dlManagerControl->resume(dlmc->menu->getItemName());
                        dlmc->startStatusUpdate();
                        break;
                    }
                case 'R':
                    {
                        if (!dlmc->dlManagerControl->isActive()) {
                            break;
                        }
                        dlmc->stopStatusUpdate();
                        dlmc->dlManagerControl->resumeAll();
                        dlmc->startStatusUpdate();
                        break;
                    }

                case 'c':
                    /* TODO - clearInactive() doesn't work properly when a download is in progress */
                    {
                        if (!dlmc->dlManagerControl->isActive()) {
                            break;
                        }
                        dlmc->stopStatusUpdate();
                        dlmc->dlManagerControl->clearInactive();
                        dlmc->mainWindows.at(dlmc->dlsStatusWinIdx)->resetWin();
                        dlmc->updateDownloadsMenu();
                        dlmc->resetStatusDriver();
                        dlmc->startStatusUpdate();
                        break;
                    }

                case 'k':
                    /* Kill selected download */
                    {
                        if (!dlmc->dlManagerControl->isActive()) {
                            break;
                        }
                        dlmc->stopStatusUpdate();
                        dlmc->dlManagerControl->stop(dlmc->menu->getItemName()); 
                        dlmc->updateDownloadsMenu();
                        dlmc->resetStatusDriver();
                        dlmc->mainWindows.at(dlmc->dlsStatusWinIdx)->resetWin();
                        dlmc->startStatusUpdate();
                        break;
                    }

                case 'K':
                    /* Kill all downloads */
                    {
                        if (!dlmc->dlManagerControl->isActive()) {
                            break;
                        }
                        dlmc->stopStatusUpdate();
                        dlmc->dlManagerControl->killAll();
                        dlmc->updateDownloadsMenu();
                        dlmc->mainWindows.at(dlmc->dlsStatusWinIdx)->resetWin();
                        dlmc->startStatusUpdate();
                        break;
                    }

                case CTRL('z'):
                    /* TODO - catch sigint - exit gracefully */
                    {
                        break;
                    }

                default:
                    {
                        break;
                    }
            }
            if (resizeUI) {
                dlmc->stopStatusUpdate();
                {
                    //std::lock_guard<std::mutex> guard(dlmc->dlsInfoMutex);
                    endwin();
                    refresh();
                    dlmc->menu->clearMenu();
                    dlmc->menu->clearItems();
                    dlmc->resizeUI();
                    dlmc->resize = true;
                    dlmc->updateDownloadsMenu();
                    dlmc->resetStatusDriver();
                }
                dlmc->startStatusUpdate();
                resizeUI = false;
            }

            /* Consistantly update main windows */
            {
                std::lock_guard<std::mutex> guard(dlmc->dlsInfoMutex);
                dlmc->refreshMainWins();        
            }
        }
    }
    return 0;
}

