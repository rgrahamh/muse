#ifndef MUSE_SERVER_HPP
#define MUSE_SERVER_HPP
#include <dirent.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <taglib/tag_c.h>

#include <sqlite3.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netdb.h>

#include "../shared.h"

#define DEFAULT_PORT "2442"
#define BUFF_SIZE PATH_MAX + 1

struct dbsonginfo {
	sqlite* db;
	char* title;
	char* artist;
	char* album;
	char* comment;
	int year;
	int track_num;
	char genre;
};

int serve(char* port);
int handleRequest(int new_sockfd);
int sendSongCallback(void* new_sockfd, int colNum, char** column, char** result);
int scan(char** lib_paths, int num_paths);
void stop(int sig);

#endif
