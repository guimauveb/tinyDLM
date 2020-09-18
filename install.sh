#!/bin/bash

# set some bools
linux=0
macos=0

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
    brewout=$( { brew > outfile; } 2>&1 )
    if [[ $brewout == *"brew install FORMULA"* ]]; then
        brew=1
        echo brew is installed
    else
        echo brew is not installed
    fi 
fi

# Create a build directory
mkdir build

# cd into build
cd build

# set some bools
gcc=0
clang=0

# check for gcc or clang presence

# check for gcc
gccout=$( { gcc  > outfile; } 2>&1 )
if [[ $gccout == *"error"* ]]; then
    echo "gcc is not installed"
    echo "checking if clang is installed..."
else
    gcc=1
    comp=1
    echo "gcc is installed" 
fi

if [[ "${comp}" -ne 1 ]]
then
    clout=$( { clang -v  > outfile; } 2>&1 )
    # check for clang
    # check if clang is installed
    if [[ $clout == *"error"* ]]; then
        echo "clang is not installed"
    else 
        echo "clang is installed"
        clang=1
        comp=1
    fi
fi

curl=0
curlpp=0
ncurses=0

# gcc TODO 
if [ "${gcc}" -eq 1 ]; then
    # TODO - function for each dependency
    curlout=$( { gcc -lcurl > outfile; } 2>&1 ) 
    if [[ $curlout != *"Undefined symbols"* ]]; then
        echo "curl is not installed"
        if [ "$brew" -eq 1 ]; then
            read -p "Do you want to install curl ? Y/n " answer
            if [ "$answer" == "Y" ] || [ "$answer" == "y" ]; then
                brew install curl
                curl=1
                #TODO check installation 
                echo curl is installed
            else
                echo Please install curl.
            fi
        fi
    else 
        curl=1
        echo "curl is installed."
    fi
    curlppout=$( { gcc -lcurlpp > outfile; } 2>&1 ) 
    if [[ $curlppout != *"Undefined symbols"* ]]; then
        echo "curlpp is not installed"
        if [ "$brew" -eq 1 ]; then
            read -p "Do you want to install curlpp ? Y/n " answer
            if [ "$answer" == "Y" ] || [ "$answer" == "y" ]; then
                brew install curlpp
                curlpp=1
                #TODO check installation 
                echo curlpp is installed.
            else
                echo Please install curlpp.
            fi
        fi
    else 
        curlpp=1
        echo "curlpp is installed."
    fi
    ncursesout=$( { gcc -lncurses > outfile; } 2>&1 ) 
    if [[ $curlppout != *"Undefined symbols"* ]]; then
        echo "ncurses is not installed"
        if [ "$brew" -eq 1 ]; then
            read -p "Do you want to install ncurses ? Y/n " answer
            if [ "$answer" == "Y" ] || [ "$answer" == "y" ]; then
                brew install ncurses
                ncurses=1
                #TODO check installation 
            else
                echo Please install ncurses.
            fi
        fi
    else 
        ncurses=1
        echo "ncurses is installed."
    fi   
fi


# clang
if [ "${clang}" -eq 1 ]; then
    # TODO - function for each dependency
    curlout=$( { clang -lcurl > outfile; } 2>&1 ) 
    if [[ $curlout != *"Undefined symbols"* ]]; then
        echo "curl is not installed"
        if [ "$brew" -eq 1 ]; then
            read -p "Do you want to install curl ? Y/n " answer
            if [ "$answer" == "Y" ] || [ "$answer" == "y" ]; then
                brew install curl
                curl=1
                #TODO check installation 
                echo curl is installed
            else
                echo Please install curl.
            fi
        fi
    else 
        curl=1
        echo "curl is installed."
    fi
    curlppout=$( { clang -lcurlpp > outfile; } 2>&1 ) 
    if [[ $curlppout != *"Undefined symbols"* ]]; then
        echo "curlpp is not installed"
        if [ "$brew" -eq 1 ]; then
            read -p "Do you want to install curlpp ? Y/n " answer
            if [ "$answer" == "Y" ] || [ "$answer" == "y" ]; then
                brew install curlpp
                curlpp=1
                #TODO check installation 
                echo curlpp is installed.
            else
                echo Please install curlpp.
            fi
        fi
    else 
        curlpp=1
        echo "curlpp is installed."
    fi
    ncursesout=$( { clang -lncurses > outfile; } 2>&1 ) 
    if [[ $curlppout != *"Undefined symbols"* ]]; then
        echo "ncurses is not installed"
        if [ "$brew" -eq 1 ]; then
            read -p "Do you want to install ncurses ? Y/n " answer
            if [ "$answer" == "Y" ] || [ "$answer" == "y" ]; then
                brew install ncurses
                ncurses=1
                #TODO check installation 
            else
                echo Please install ncurses.
            fi
        fi
    else 
        ncurses=1
        echo "ncurses is installed."
    fi
fi

# rm tmp file
rm outfile

# if everything required is installed
if [ "${comp}" -eq 1 ] && [ "${curl}" -eq 1 ] && [ "${curlpp}" -eq 1 ] && [ "${ncurses}" -eq 1 ]; then
    # run cmake from build
    cmake ../
    # run make
    make

    if [ ! -f tinyDLM ]; then
        echo "Couldn't locate tinyDLM. Build certainly failed. Check cmake error logs"
    else 
        # ask user if he wants to run the program 
        read -p "Do you want to launch tinyDLM ? Y/n " answer
        if [ "$answer" == "Y" ] || [ "$answer" == "y" ]
        then
            # run tinyDLM
            ./tinyDLM
        else 
            echo tinyDLM was built successfully.
        fi
    fi
fi

