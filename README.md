# tinyDLM - A tiny download manager

![tinyDLM](/imgs/screenshot.jpg)

## Small multi-threaded download manager built around cURLpp / ncurses 
**I made this program to learn more about programming and more specifically about C++, threads, curses, curl etc.**

### Features:
    - Supports Direct Download Link only 
    - Supports simultaneous transfers  
    - Transfers can be paused / resumed / killed.
    - Displays current download speed and progress for each transfer  

### Installation:
    
    - [ ] makefile

Works pretty well on macOS Mojave (10.14.6), not so well on Linux (tested on Ubuntu and Kali) because of 
a problem I have with threads and curses window sizes representation which seem to differ from macOS. 


**Missing functionalities that I wish to implement (but certainly won't since I'm moving to new projects):**

    - [ ] Keep the window from resizing under a certain size (probably easy)
    - [ ] Add multiple links at the same time (easy)
    - [ ] Schedule download start (probably easy)
    - [ ] Select download directory (transfers are stored in 'downloads') (trivial)
    - [ ] Compute the ETA (probably easy)
    - [ ] Save downloads on exit and restore them on the next launch (not so trivial)



