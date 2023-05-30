#!/bin/sh
set -e

# You probably shouldn't change this
BUILD_DIRECTORY="Build"

# NOTE: The `|| true` basically ignores all errors
rm -rf Build || true

# Create the `Build` directory, if it doesn't exist already
if [ ! -d "${BUILD_DIRECTORY}" ]
then
    mkdir "${BUILD_DIRECTORY}"
fi

pushd "${BUILD_DIRECTORY}" > /dev/null

# Setup CMake with Ninja
cmake -GNinja ..

popd > /dev/null
