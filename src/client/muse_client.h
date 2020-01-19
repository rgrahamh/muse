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
	unsigned long year;
	unsigned long track_num;
	char* genre;
	struct songinfolst* next;
};

struct albuminfolst{
	char* title;
	unsigned long id;
	unsigned long year;
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
    int getSong(unsigned long song_id, char* filepath);
    int querySongs(struct songinfolst** song_info);
    int queryAlbums(struct albuminfolst** album_info);
    int queryAlbumSongs(unsigned long album_id, struct songinfolst** song_info);
    int queryArtists(struct artistinfolst** artist_info);
    int queryArtistAlbums(unsigned long artist_id, struct albuminfolst** album_info);
    int queryGenre(struct genreinfolst** genre_info);
    int queryGenreSongs(char* genre, struct songinfolst** song_info);

	int parseSongs(char* resp, struct songinfolst** song_info);
	int parseAlbums(char* resp, struct albuminfolst** album_info);
	int parseArtists(char* resp, struct artistinfolst** artist_info);
	int parseGenre(char* resp, struct genreinfolst** genre_info);

	char* parseFieldStr(char** dest, char* base, char endchar);
	char* parseFieldLong(unsigned long* dest, char* base, char endchar);

	void initSong(struct songinfolst** song_info);
	void initAlbum(struct albuminfolst** album_info);
	void initArtist(struct artistinfolst** artist_info);
	void initGenre(struct genreinfolst** genre_info);

    void free_songinfolst(struct songinfolst* song_info);
    void free_albuminfolst(struct albuminfolst* album_info);
    void free_artistinfolst(struct artistinfolst* artist_info);
    void free_genreinfolst(struct genreinfolst* genre_info);
	int substrsize(char* str, char until);
	int substr(char* base, char until, char* cpy, int cpySize);

    int queryEntity(unsigned long entity_id, char flags);
    int receiveResponse(char** resp);
    void stop(int sig);
#ifdef __cplusplus
}
#endif

#endif // MUSE_CLIENT_H
