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

struct artistinfo{
	char* title;
	unsigned long id;
};

int sockfd;

int connectToServ(char* port, char* ip);
int queryArtistAlbums(unsigned long artist_id, struct albuminfo** album_info);
int queryAlbumSongs(unsigned long album_id, struct songinfo** song_info);
int queryEntity(unsigned long entity_id, char flags);
int receiveResponse(char** resp);
void stop(int sig);
