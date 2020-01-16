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
	sqlite3* db;
	char* title;
	char* artist;
	unsigned long artist_id;
	unsigned long next_artist;
	char* album;
	unsigned long album_id;
	unsigned long next_album;
	char* comment;
	int year;
	int track_num;
	char* filepath;
	char* genre;
};

struct linkedStr{
	char* str;
	struct linkedStr* next;
};

void insertLinkedStr(struct linkedStr* last, char* element);
void freeLinkedStr(struct linkedStr* last);

int serve(char* port, FILE* log_file);
int handleRequest(int new_sockfd);
int sendSongCallback(void* new_sockfd, int colNum, char** column, char** result);
int sendInfo(void* result_list, int colNum, char** column, char** result);
int getAlbumArtist(void* sinfo, int colNum, char** result, char** column);
int addAllCallback(void* sinfo, int colNum, char** result, char** column);
int cullSongCallback(void* datab, int colNum, char** result, char** column);
int deleteArtist(void* artist_still_exists, int colNum, char** result, char** column);
int deleteAlbum(void* album_still_exists, int colNum, char** result, char** column);
int initAlbumID(void* sinfo, int colNum, char** result, char** column);
int initArtistID(void* sinfo, int colNum, char** result, char** column);
int returnOne(void* sinfo, int colNum, char** result, char** column);
int scan(char** lib_paths, int num_paths);
void stop(int sig);

#endif
