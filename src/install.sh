WORK_PATH=~/Documents/MUSE
CURR_PATH=$(pwd)

set -e

if [ ! -d $WORK_PATH ]; then
	mkdir $WORK_PATH
fi

make muse_server
sudo mv server/muse_server /usr/bin
cp server/db_setup.sql $WORK_PATH
cd $WORK_PATH
$CURR_PATH/server/init_db.sh
cd $CURR_PATH
