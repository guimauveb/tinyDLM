# tinyDLM - A C++ download manager

![tinyDLM](/imgs/screenshot.jpg)

## Small multi-threaded download manager built around cURLpp / ncurses 

I made this project to learn more about desktop applications, C++, threads, cURL / cURLpp and ncurses.
C++ download manager built around cURLpp and ncurses

I've never encountered a simple yet efficient download manager that would be free AND still maintained.

So I decided to build my own and figured out it would be a great way to learn more about C++, multithreading, sockets and desktop applications and I did learn a lot along the way !

I could have used multiple non blocking sockets instead of threads and therefore only use one thread for the download manager part, but I really wanted to learn more about multithreading and its pitfalls.

The main window is actually made of 2 subwindows: the downloads list displayed as a curses menu and a 'downloads status' window sitting next to it and updated in its own thread.
Using different threads for windows was also a challenge since curses (actually ncurses) is not multi-thread safe, but using C++ mutexes I managed to make it work smoothly.

I plan on adding some other useful features like limiting the number of simultaneous transfers or adding more than one link at the same time.

### Features:
    - Works on macOS and Linux Debian based distributions
    - Supports Direct Download Links
    - Supports simultaneous transfers (tested up to 32)
    - Transfers can be paused / resumed / killed
    - Inactive transfers can be cleared from the list
    - Displays current download speed and progress for each transfer
    - Displays current transfer status
    - Signals user input and URL / HTTP errors
    - Very low CPU / memory consumption
    - (DOING) Partially supports adding multiple links at the same time

### Installation 

Works on **Linux Debian based distributions** and **macOS (>= 10.14)**
    
1. Make sure you have the following dependencies installed:
```
    cmake
```

2. Clone this repo
```
    git clone https://github.com/guimauveb/tinyDLM.git
```

3. Run **install.sh**


### Tested on: 
- **macOS Mojave (10.14.6)** 
- **Linux Ubuntu (20.04 LTS)**
- **Kali Linux (20.03)**

### Missing features coming soon 
    - [X] Duplicate filename checker 
    - [IN PROGRESS] Add multiple links at the same time 
    - [ ] Set the default number of simultaneous transfers to the number of CPU cores / threads
    - [ ] Set the number of max simultaneous transfers  
    - [ ] Schedule download start  
    - [ ] Compute / Display ETA  
    - [ ] Limit download speed  
    - [ ] Select download directory (transfers are stored in 'downloads') by creating a file explorer 
    - [ ] Save downloads on exit and restore them on the next launch  



