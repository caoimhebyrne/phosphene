#!/bin/sh
set -e

# You probably shouldn't change this
BUILD_DIRECTORY="Build"
BOOT_DIRECTORY="/Volumes/boot"

# Error if the build directory doesn't exist
if [ ! -d "${BUILD_DIRECTORY}" ]
then
    echo "! Please run Scripts/setup.sh" first!
    exit -1
fi

# Error if kernel8.img doesn't exist
if [ ! -f "${BUILD_DIRECTORY}/kernel8.img" ]
then
    echo "! Please run Scripts/build.sh" first!
    exit -1
fi

pushd "${BUILD_DIRECTORY}" > /dev/null

# Notify that the script will halt until the folder exists
if [ ! -d "${BOOT_DIRECTORY}" ] || [ ! -w "${BOOT_DIRECTORY}" ]
then
    echo "- Waiting until ${BOOT_DIRECTORY} exists and is writable..."
fi

until [ -d "${BOOT_DIRECTORY}" ] && [ -w "${BOOT_DIRECTORY}" ]
do
    sleep 0
done

echo "* Copying image..."

cp kernel8.img "${BOOT_DIRECTORY}/phosphene.img"

# TODO: Other platforms
if [ "$(uname)" == "Darwin" ]
then
    echo "* Unmounting SD card..."
    diskutil eject "${BOOT_DIRECTORY}" > /dev/null
fi

echo "+ Don't forget to add \`kernel=phosphene.img\` to ${BOOT_DIRECTORY}/config.txt!"

popd > /dev/null
