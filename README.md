# tinyDLM - A C++ download manager

![tinyDLM](/imgs/screenshot.jpg)

## Small multi-threaded download manager built around cURLpp / ncurses 

I made this project to learn more about desktop applications, C++, threads, cURL / cURLpp and ncurses.

### Features:
    - Supports Direct Download Link only 
    - Supports simultaneous transfers  
    - Transfers can be paused / resumed / killed.
    - Displays current download speed and progress for each transfer  
    - Very low CPU / memory consumption
    - **NEW** Partially supports adding multiple links at the same time


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



