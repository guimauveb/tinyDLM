/* TODO in version 1.2:
 *  - Adding multiple downloads at the same time DOING
 *  - Modify some settings (among them what follows)
 *  - Chose number of simultaneous downloads (default to number of cores / threads ) 
 *  - Limit download speed 
 */
#include "tinyDLM.hxx"

/* Initialize our user interface and browse it */
int main()
{
    /* TODO - Load settings */
    std::unique_ptr<UI> ui = std::make_unique<UI>();

    /* Display a message when starting the program. If the user exits while in firstStart(), terminate the 
     * program */
    if (!ui->firstStart()) {
        return 1;
    }

    /* Navigate through tinyDLM UI */
    {
        int ch = 0;
        bool resizeUI = false;

        while ((ch = getch()) != KEY_F(1)) {
            switch(ch) {
                case KEY_RESIZE:
                    {
                        resizeUI = true;
                        break;
                    }

                case KEY_DOWN:
                    {
                        /* Make sure that the menu is not empty */
                        if (ui->controller->isActive()) {
                            ui->downloads_menu->menuDriver(REQ_DOWN_ITEM);
                            ui->statusDriver(KEY_DOWN);
                        }
                        break;
                    }

                case KEY_UP:
                    {
                        /* Make sure that the menu is not empty */
                        if (ui->controller->isActive()) {
                            ui->downloads_menu->menuDriver(REQ_UP_ITEM);
                            ui->statusDriver(KEY_UP); 
                        }
                        break;
                    }

                case 10:
                    /* Enter - user has selected an item */
                    {

                        /* Make sure that the menu is not empty */
                        if (!ui->controller->isActive()) {
                            break;
                        }
                        ui->stopStatusUpdate();

                        /* showDetails returns true if the download was killed and that we should update the
                         * menu */
                        if (ui->showDownloadDetails(ui->downloads_menu->getItemName())) {
                            ui->updateDownloadsMenu();
                            ui->resetStatusDriver();
                            ui->main_windows.at(ui->status_window_index)->resetWin();
                        }

                        ui->startStatusUpdate(); 
                        break;
                    }

                case 'a':
                    /* Opens a subwindow in which we'll enter the new download item info */
                    {
                        ui->stopStatusUpdate();
                        if (ui->addNewDownload()) {
                            ui->updateDownloadsMenu();
                            ui->resetStatusDriver();
                        }
                        ui->startStatusUpdate();
                        break;
                    }

                case 'h':
                    {
                        ui->stopStatusUpdate();
                        if (ui->showHelp()) {
                            ui->updateDownloadsMenu();
                            ui->resetStatusDriver();
                        }
                        ui->startStatusUpdate();
                        break;
                    }

                case 's':
                    {
                        ui->stopStatusUpdate();
                        if (ui->showSettings()) {
                            ui->updateDownloadsMenu();
                            ui->resetStatusDriver();
                        }
                        ui->startStatusUpdate();
                        break;
                    }
                case 'p':
                    /* Pause highlighted download */
                    {
                        if (!ui->controller->isActive()) {
                            break;
                        }
                        ui->controller->pause(ui->downloads_menu->getItemName());
                        break;
                    }

                case 'P':
                    /* Pause all active downloads */
                    {
                        if (!ui->controller->isActive()) {
                            break;
                        }
                        ui->stopStatusUpdate();
                        ui->controller->pauseAll();
                        ui->startStatusUpdate();
                        break;
                    }

                case 'r':
                    {
                        if (!ui->controller->isActive()) {
                            break;
                        }
                        ui->stopStatusUpdate();
                        ui->controller->resume(ui->downloads_menu->getItemName());
                        ui->startStatusUpdate();
                        break;
                    }
                case 'R':
                    {
                        if (!ui->controller->isActive()) {
                            break;
                        }
                        ui->stopStatusUpdate();
                        ui->controller->resumeAll();
                        ui->startStatusUpdate();
                        break;
                    }

                case 'c':
                    {
                        if (!ui->controller->isActive()) {
                            break;
                        }
                        ui->stopStatusUpdate();
                        ui->controller->clearInactives();
                        ui->main_windows.at(ui->status_window_index)->resetWin();
                        ui->updateDownloadsMenu();
                        ui->resetStatusDriver();
                        ui->startStatusUpdate();
                        break;
                    }

                case 'k':
                    /* Kill selected download */
                    {
                        if (!ui->controller->isActive()) {
                            break;
                        }
                        ui->stopStatusUpdate();
                        ui->controller->stop(ui->downloads_menu->getItemName()); 
                        ui->updateDownloadsMenu();
                        ui->resetStatusDriver();
                        ui->main_windows.at(ui->status_window_index)->resetWin();
                        ui->startStatusUpdate();
                        break;
                    }

                case 'K':
                    /* Kill all downloads */
                    {
                        if (!ui->controller->isActive()) {
                            break;
                        }
                        ui->stopStatusUpdate();
                        ui->controller->killAll();
                        ui->updateDownloadsMenu();
                        ui->main_windows.at(ui->status_window_index)->resetWin();
                        ui->startStatusUpdate();
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
                ui->stopStatusUpdate();
                ui->resizeUI();
                ui->updateDownloadsMenu();
                ui->resetStatusDriver();
                ui->startStatusUpdate();
                resizeUI = false;
            }

            /* Consistantly update main windows */
            {
                std::lock_guard<std::mutex> guard(ui->downloads_infos_window_mutex);
                ui->refreshMainWindows();        
            }
        }
    }
    return 0;
}

