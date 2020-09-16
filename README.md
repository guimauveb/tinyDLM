# tinyDLM - A tiny download manager

![tinyDLM](/imgs/screenshot.jpg)

## Small multi-threaded download manager built around cURLpp / ncurses 

I made this project to learn more about desktop applications, C++, threads, and great librairies that are cURL /
cURLpp and ncurses.

### Features:
    - Supports Direct Download Link only 
    - Supports simultaneous transfers  
    - Transfers can be paused / resumed / killed.
    - Displays current download speed and progress for each transfer  

### Installation (a makefile is coming) 
    - clang++ main/main.cpp UI/source/dlManagerUI.cpp helper/source/userInputCheck.cpp dialogs/source/dialogs.cpp curses/source/cursesMenu.cpp curses/source/cursesForm.cpp curses/source/cursesWindow.cpp dlManager/source/dlManager.cpp dlManagerController/source/dlManagerController.cpp helper/source/stringify.cpp -o tinyDLM -std=c++17 -lncurses -lpanel -lmenu -lform -lcurl -lcurlpp -Wall -Wextra -Wmissing-field-initializers -Wuninitialized -Wshadow -g -Os

Tested on: 
- **macOS Mojave (10.14.6)** 
- **Linux Ubuntu (20.04 LTS)**

### Missing functionalities that I wish to implement
    
    - [ ] Limit download speed  
    - [ ] Chose number of max simultaneous downloads  
    - [ ] Add multiple links at the same time  
    - [ ] Schedule download start  
    - [ ] Select download directory (transfers are stored in 'downloads')  
    - [ ] Compute the ETA  
    - [ ] Save downloads on exit and restore them on the next launch  



