#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include "../shared.h"

struct songinfo{
	char* title;
	unsigned long id;
	char* artist;
	char* album;
	int year;
	int track_num;
	char* genre;
};

struct albuminfo{
	char* title;
	unsigned long id;
	int year;
};

int sockfd;

int connectToServ(char* port, char* ip);
int queryAlbumSongs(unsigned long album_id, struct albuminfo* album_info);
void stop(int sig);
