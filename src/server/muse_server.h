#ifndef MUSE_SERVER_HPP
#define MUSE_SERVER_HPP
#include <dirent.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <id3v2lib.h>

#include <sqlite3.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "../shared.h"

#define DEFAULT_PORT "2442"
#define BUFF_SIZE 256

int serve(char* port);
int handleRequest(int new_sockfd);
int sendSongCallback(void* new_sockfd, int colNum, char** column, char** result);
int scan(char** lib_paths, int num_paths);
void stop(int sig);

#endif
