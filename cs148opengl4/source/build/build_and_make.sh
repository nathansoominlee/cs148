#!/bin/bash
# build_and_make.sh
# Builds the Makefiles then runs make

# Change this for different assignments
ASSIGNMENT=3

# The build location
MIKE_DIR=unix/Debug
NATHAN_DIR=xcode

# Turn on command echoing
set -x

if [[ `hostname` == "coche" ]]; then

    echo "---> Building for Mike"

    # Make the directory if it doesn't exist
    mkdir -p $MIKE_DIR
    
    cd $MIKE_DIR

    # Build and make
    cmake ../../../ -G "Unix Makefiles" -DASSIGNMENT=$ASSIGNMENT -DCMAKE_BUILD_TYPE=CONFIG && \
        make

else

    echo "---> Building for Nathan"
    mkdir -p $NATHAN_DIR
    cd $NATHAN_DIR
    cmake ../../ -G "Xcode" -DASSIGNMENT=$ASSIGNMENT

fi
