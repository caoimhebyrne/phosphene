#!/bin/sh
set -e

# You probably shouldn't change this
BUILD_DIRECTORY="Build"

# Error if the build directory doesn't exist
if [ ! -d "${BUILD_DIRECTORY}" ]
then
    echo "! Please run Scripts/setup.sh" first!
    exit -1
fi

pushd "${BUILD_DIRECTORY}" > /dev/null

# Build the project 
cmake --build .

popd > /dev/null
