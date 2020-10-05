#!/bin/bash

# Install script that will look for the required dependencies in the envionrment and will try to install them 
# if not found. Once all the required dependencies are installed the script will build tinyDLM from source and
# launch the program.

# I only provide cURLpp source since it does not seem to be found on some Linux distributions using apt. 
# macOS
# libcurl and libncurses will be installed via homebrew if homebrew is installed. Otherwise the installation
# will stop. 
# Linux
# lbcurl and libncurses will be installed via apt on Linux Debian distributions. Otherwise the installation
# will stop.

# set some bools
linux=0
macos=0

# set some bools
apt=0
brew=0

if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    linux=1
    echo Using Linux...
    # temporary
    apt=1
    # check apt presence
elif [[ "$OSTYPE" == "darwin"* ]]; then
    macos=1
    echo Using macOS...
    echo checking if brew is installed... 
    brewout=$( { brew > tmp; } 2>&1 )
    if [[ $brewout == *"brew install FORMULA"* ]]; then
        brew=1
        echo brew is installed.
    else
        echo brew is not installed.
    fi 
else
    echo tinyDLM can only be installed on Linux and macOS for now.
fi

    mkdir build
    mkdir build/downloads

    cd build

# comp will either be "gcc" or "clang" 
comp="0"

# set some bools
gcc=0
clang=0

# check for gcc or clang presence
gccout=$( { gcc  > tmp; } 2>&1 )
if [[ $gccout == *"no input files"* ]]; then
    echo "gcc is installed" 
    comp="gcc"
else
    echo "gcc is not installed"
    echo "checking if clang is installed..."
fi

if [[ "${comp}" == "0" ]]
then
    clout=$( { clang -v  > tmp; } 2>&1 )
    if [[ $clout == *"Apple clang"* ]]; then
        echo "clang is installed"
        comp="clang"
    else 
        echo "clang is not installed"
    fi
fi

# set some bools
curl=0
curlpp=0
ncurses=0

if [ "${comp}" != "0" ]; then
    curlout=$( { ${comp} -lcurl > tmp; } 2>&1 ) 
    if [[ $curlout != *"main"* ]]; then
        echo "curl dev libraries are not installed"
        if [ "$brew" -eq 1 ]; then
            read -p "Do you want to install libcurl-dev (using homebrew)? [Y/n] " answer
            if [ "$answer" == "Y" ] || [ "$answer" == "y" ]; then
                brew install curl --with-openssl	
                curl=1
                echo curl dev libraries are installed
            else
                echo Please install curl dev libraries
            fi
        elif [ "$apt" -eq 1 ]; then 
            read -p "Do you want to install curl dev libraries (using apt)? [Y/n] " answer
            if [ "$answer" == "Y" ] || [ "$answer" == "y" ]; then
                sudo apt-get install libcurl4-openssl-dev
                curl=1
                echo curl dev libraries are installed.
            fi
        else 
            echo Please install curl dev libraries.
        fi

    else 
        curl=1
        echo "curl dev libraries are installed."
    fi

    # Build curlpp from source 
    curlppout=$( { ${comp} -lcurlpp > tmp; } 2>&1 ) 
    if [[ $curlppout != *"main"* ]]; then
        echo "curlpp is not installed"
        read -p "Do you want to install curlpp ? Y/n " answer
        if [ "$answer" == "Y" ] || [ "$answer" == "y" ]; then
            cd ../dependencies/curlpp-0.8.1
            mkdir build
            cd build
            sudo cmake ../
            sudo make install
            cd ../../../build
            curlpp=1
            echo curlpp is installed
        fi
    else 
        curlpp=1
        echo "curlpp is installed."
    fi

    ncursesout=$( { ${comp} -lncurses > tmp; } 2>&1 ) 
    if [[ $curlppout != *"main"* ]]; then
        echo "ncurses dev libraries are not installed"
        if [ "$brew" -eq 1 ]; then
            read -p "Do you want to install ncurses dev libraries (using homebrew)? [Y/n] " answer
            if [ "$answer" == "Y" ] || [ "$answer" == "y" ]; then
                brew install ncurses
                ncurses=1
            else
                echo Please install ncurses.
            fi
        elif [ "$apt" -eq 1 ]; then 
            read -p "Do you want to install ncurses dev libraries (using apt)? [Y/n] " answer
            if [ "$answer" == "Y" ] || [ "$answer" == "y" ]; then
                sudo apt-get install libncurses-dev
                ncurses=1
                echo ncurses dev libraries are installed
            fi
        else
            echo Please install ncurses dev libraries.
        fi
    else 
        ncurses=1
        echo "ncurses dev libraries are installed."
    fi   
else
    echo Please installed all the required dependencies.
fi


# rm tmp files
rm tmp
rm ../tmp

# if everything required is installed
if [ "${comp}" != "0" ] && [ "${curl}" -eq 1 ] && [ "${curlpp}" -eq 1 ] && [ "${ncurses}" -eq 1 ]; then
    # run cmake from build
    cmake ../
    # run make
    make

    if [ ! -f tinyDLM ]; then
        echo "Couldn't locate tinyDLM. Build certainly failed. Check cmake error logs."
    else 
        # ask user if he wants to run the program 
        read -p "Do you want to launch tinyDLM ? [Y/n] " answer
        if [ "$answer" == "Y" ] || [ "$answer" == "y" ]
        then
            # run tinyDLM
            ./tinyDLM
        else 
            echo tinyDLM was built successfully.
        fi
    fi
else
    echo Installation failed. Please install all required dependencies.
fi

