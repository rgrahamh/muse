# SETUP
Before trying to build this application, you have to download the `ncurses` library. In order to do this, run this command if you have apt installed on your computer:
```
sudo apt-get install libncurses5-dev libncursesw5-dev
```
Or this if you have yum:
```
sudo yum install ncurses-devel
```
After this, you have to install the sqlite development libraries. To do this, run:
```
sudo apt-get install sqlite3 libsqlite3-dev
```
Then, you have to install TagLib. Navigate to the libraries directory (or to https://taglib.org/) and follow the installation instructions. The flow of instructions should look something like:
```
cmake .
make
make install
```
