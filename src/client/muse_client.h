#ifndef MUSE_CLIENT_H
#define MUSE_CLIENT_H

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

struct songinfolst{
	char* title;
	unsigned long id;
	char* artist;
	char* album;
	int year;
	int track_num;
	char* genre;
    struct songinfolst* next;
};

struct albuminfolst{
	char* title;
	unsigned long id;
	int year;
    struct albuminfolst* next;
};

struct artistinfolst{
	char* name;
	unsigned long id;
    struct artistinfolst* next;
};

struct genreinfolst{
    char* genre;
    struct genreinfolst* next;
};

#ifdef __cplusplus
extern "C" {
#endif
    int connectToServ(char* port, char* ip);
    int getSong(unsigned long song_id);
    int querySongs(struct songinfolst** song_info);
    int queryAlbums(struct albuminfolst** album_info);
    int queryAlbumSongs(unsigned long album_id, struct songinfolst** song_info);
    int queryArtists(struct artistinfolst** artist_info);
    int queryArtistAlbums(unsigned long artist_id, struct albuminfolst** album_info);
    int queryGenre(char** genres);
    int queryGenreSongs(char* genre, struct songinfolst** song_info);

    void free_songinfolst(struct songinfolst* song_info);
    void free_albuminfolst(struct albuminfolst* album_info);
    void free_artistinfolst(struct artistinfolst* artist_info);
    void free_genreinfolst(struct genreinfolst* genre_info);

    int queryEntity(unsigned long entity_id, char flags);
    int receiveResponse(char** resp);
    void stop(int sig);
#ifdef __cplusplus
}
#endif

#endif // MUSE_CLIENT_H
