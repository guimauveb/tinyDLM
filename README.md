# tinyDLM - A tiny download manager

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

### Missing features coming in version 1.1
    
    - [X] Duplicate filename checker 
    - [ ] Add multiple links at the same time **DOING** 
    - [ ] Limit the number of max simultaneous downloads  
    - [ ] Schedule download start  
    - [ ] Compute / Display ETA  
    - [ ] Limit download speed  
    - [ ] Select download directory (transfers are stored in 'downloads') by creating a file explorer 
    - [ ] Save downloads on exit and restore them on the next launch  



