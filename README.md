# tinyDLM - A tiny download manager

![tinyDLM](/imgs/screenshot.jpg)

## Small multi-threaded download manager built around cURLpp / ncurses 

I made this project to learn more about desktop applications, C++, threads, cURL / cURLpp and ncurses.

### Features:
    - Supports Direct Download Link only 
    - Supports simultaneous transfers  
    - Transfers can be paused / resumed / killed.
    - Displays current download speed and progress for each transfer  

### Installation 
    
1. Dependencies needed:
```
   - libcurl
   - libcurlpp
   - ncurses
```

2. Create a **build** directory at the top of the project directory and cd into it
```
   $ mkdir build
   $ cd build
``` 

3. From **buid/** run cmake
```
    $ cmake ../
    $ make
```

4. Run **tinyDLM** newly created into **build/**
```
    $ ./tinyDLM
```

Tested on: 
- **macOS Mojave (10.14.6)** 
- **Linux Ubuntu (20.04 LTS)**

### Missing features I want to implement
    
    - [ ] Limit download speed  
    - [ ] Limit the number of max simultaneous downloads  
    - [ ] Add multiple links at the same time  
    - [ ] Schedule download start  
    - [ ] Select download directory (transfers are stored in 'downloads')  
    - [ ] Compute the ETA  
    - [ ] Save downloads on exit and restore them on the next launch  



