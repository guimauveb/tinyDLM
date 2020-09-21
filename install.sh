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

# TODO - create functions instead of using the same blocks of code over and over

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

# check if gcc is installed
gccout=$( { gcc  > outfile; } 2>&1 )
if [[ $gccout == *"no input files"* ]]; then
    echo "gcc is installed" 
    gcc=1
    comp=1
else
    echo "gcc is not installed"
    echo "checking if clang is installed..."
fi

# check if clang is installed
if [[ "${comp}" -ne 1 ]]
then
    clout=$( { clang -v  > outfile; } 2>&1 )
    if [[ $clout == *"no input files"* ]]; then
        echo "clang is installed"
        clang=1
        comp=1
    else 
        echo "clang is not installed"
    fi
fi

curl=0
curlpp=0
ncurses=0

if [ "${gcc}" -eq 1 ]; then
    # TODO - function for each dependency
    curlout=$( { gcc -lcurl > outfile; } 2>&1 ) 
    if [[ $curlout != *"main"* ]]; then
        echo "curl is not installed"
        if [ "$brew" -eq 1 ]; then
            read -p "Do you want to install curl (using homebrew)? Y/n " answer
            if [ "$answer" == "Y" ] || [ "$answer" == "y" ]; then
                brew install curl
                curl=1
                echo curl is installed
            else
                echo Please install curl.
            fi
        elif [ "$apt" -eq 1 ]; then 
            read -p "Do you want to install curl (using apt)? Y/n " answer
            if [ "$answer" == "Y" ] || [ "$answer" == "y" ]; then
                sudo apt-get install libcurl4-openssl-dev
                curl=1
                echo curl is installed
            fi
        else 
            echo Please install curl development libraries.
        fi

    else 
        curl=1
        echo "curl is installed."
    fi

    # Build curlpp from source 
    curlppout=$( { gcc -lcurlpp > outfile; } 2>&1 ) 
    if [[ $curlppout != *"main"* ]]; then
        echo "curlpp is not installed"
        read -p "Do you want to install curlpp ? Y/n " answer
        if [ "$answer" == "Y" ] || [ "$answer" == "y" ]; then
            cd ../dependencies/curlpp-0.8.1
            mkdir build
            cd build
            sudo cmake ../
            sudo make install
            cd ../../build
            curlpp=1
            echo curlpp is installed
        fi
    else 
        curlpp=1
        echo "curlpp is installed."
    fi

    ncursesout=$( { gcc -lncurses > outfile; } 2>&1 ) 
    if [[ $curlppout != *"main"* ]]; then
        echo "ncurses is not installed"
        if [ "$brew" -eq 1 ]; then
            read -p "Do you want to install ncurses (using homebrew)? Y/n " answer
            if [ "$answer" == "Y" ] || [ "$answer" == "y" ]; then
                brew install ncurses
                ncurses=1
            else
                echo Please install ncurses.
            fi
        elif [ "$apt" -eq 1 ]; then 
            read -p "Do you want to install ncurses (using apt)? Y/n " answer
            if [ "$answer" == "Y" ] || [ "$answer" == "y" ]; then
                sudo apt-get install libncurses-dev
                ncurses=1
                echo ncurses is installed
            fi
        else
            echo Please install ncurses development libraries.
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
    if [[ $curlout != *"main"* ]]; then
        echo "curl is not installed"
        if [ "$brew" -eq 1 ]; then
            read -p "Do you want to install curl (using homebrew)? Y/n " answer
            if [ "$answer" == "Y" ] || [ "$answer" == "y" ]; then
                brew install curl
                curl=1
                echo curl is installed
            else
                echo Please install curl.
            fi
        elif [ "$apt" -eq 1 ]; then 
            read -p "Do you want to install curl (using apt)? Y/n " answer
            if [ "$answer" == "Y" ] || [ "$answer" == "y" ]; then
                sudo apt-get install libcurl4-openssl-dev
                curl=1
                echo curl is installed
            fi
        else
            echo Please install curl development libraries.
        fi
    else 
        curl=1
        echo "curl is installed."
    fi

    curlppout=$( { clang -lcurlpp > outfile; } 2>&1 ) 
    if [[ $curlppout != *"main"* ]]; then
        echo "curlpp is not installed"
        if [ "$brew" -eq 1 ]; then
            read -p "Do you want to install curlpp (using homebrew)? Y/n " answer
            if [ "$answer" == "Y" ] || [ "$answer" == "y" ]; then
                brew install curlpp
                curlpp=1
                echo curlpp is installed.
            else
                echo Please install curlpp.
            fi
        else 
            read -p "Do you want to install curlpp ? Y/n " answer
            if [ "$answer" == "Y" ] || [ "$answer" == "y" ]; then
                cd ../dependencies/curlpp-0.8.1
                mkdir build
                cd build
                sudo cmake ../
                sudo make install
                cd ../
                sudo rm -r build
                cd ../../build
                curlpp=1
                echo curlpp is installed
            else
                echo Please install curlpp.
            fi
        fi
    else 
        curlpp=1
        echo "curlpp is installed."
    fi

    ncursesout=$( { clang -lncurses > outfile; } 2>&1 ) 
    if [[ $curlppout != *"main"* ]]; then
        echo "ncurses is not installed"
        if [ "$brew" -eq 1 ]; then
            read -p "Do you want to install ncurses (using homebrew)? Y/n " answer
            if [ "$answer" == "Y" ] || [ "$answer" == "y" ]; then
                brew install ncurses
                ncurses=1
            else
                echo Please install ncurses.
            fi
        elif [ "$apt" -eq 1 ]; then 
            read -p "Do you want to install ncurses (using apt)? Y/n " answer
            if [ "$answer" == "Y" ] || [ "$answer" == "y" ]; then
                sudo apt-get install libncurses-dev
                ncurses=1
                echo ncurses is installed
            fi
        else
            echo Please install ncurses development libraries.
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

