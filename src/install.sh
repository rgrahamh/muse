#!/bin/bash

WORK_PATH=~/Documents/MUSE
CURR_PATH=$(pwd)

if [ ! -d $WORK_PATH ]; then
	mkdir $WORK_PATH
	mkdir $WORK_PATH/assets
	mkdir $WORK_PATH/playlists
fi

#Installing ncurses and sqlite (best-try for Ubuntu and Fedora, look up your own package manager if you don't have 
which apt >/dev/null 2>&1
if [ $? = 0 ]; then
    sudo apt-get install -y libncurses5-dev libncurses5-dev sqlite3 libsqlite3-dev libgl1-mesa-dev
fi

which dnf >/dev/null 2>&1
if [ $? = 0 ]; then
    sudo dnf install ncurses-devel sqlite sqlite-devel
fi

set -e

#Installing TagLib
cd ../external-libraries/taglib-1.11.1
cmake .
make
sudo make install
cd $CURR_PATH

#Making the server
make muse_server

#Moving the server to /usr/bin
sudo mv server/muse_server /usr/bin

#Copy the db_setup SQL commands to the work path
cp server/db_setup.sql $WORK_PATH

#Copy assets and libraries to proper places
cp ../assets/banner.ascii $WORK_PATH/assets/
sudo cp ../external-libraries/fmod-core-2.00.07/lib/* /lib

#Build the database
cd $WORK_PATH
$CURR_PATH/server/init_db.sh
cd $CURR_PATH
