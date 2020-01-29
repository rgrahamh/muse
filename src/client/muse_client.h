#ifndef MUSE_CLIENT_H
#define MUSE_CLIENT_H

#include <dirent.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "../shared.h"

struct songinfolst{
	char* title;
	unsigned long long id;
	char* artist;
	char* album;
	unsigned long long year;
	unsigned long long track_num;
	char* genre;
	struct songinfolst* next;
};

struct albuminfolst{
	char* title;
	unsigned long long id;
	unsigned long long year;
	struct albuminfolst* next;
};

struct artistinfolst{
	char* name;
	unsigned long long id;
	struct artistinfolst* next;
};

struct genreinfolst{
	char* genre;
	struct genreinfolst* next;
};

struct playlist{
	char* name;
	struct songlst* first_song;
	struct songlst* last_song;
	struct playlist* prev;
};

struct songlst{
	unsigned long long id;
	struct songlst* next;
};

#ifdef __cplusplus
extern "C" {
#endif
	int connectToServ(const char* port, const char* ip);
	int getSong(unsigned long long song_id, char* filepath);
	int querySongs(struct songinfolst** song_info);
	int querySongInfo(struct songinfolst** song_info, unsigned long long song_id);
	int querySongsBurst(struct songinfolst** song_info, unsigned long long start, unsigned long long end);
	int queryAlbums(struct albuminfolst** album_info);
	int queryAlbumsBurst(struct albuminfolst** album_info, unsigned long long start, unsigned long long end);
	int queryAlbumSongs(unsigned long long album_id, struct songinfolst** song_info);
	int queryArtists(struct artistinfolst** artist_info);
	int queryArtistsBurst(struct artistinfolst** artist_info, unsigned long long start, unsigned long long end);
	int queryArtistAlbums(unsigned long long artist_id, struct albuminfolst** album_info);
	int queryGenres(struct genreinfolst** genre_info);
	int queryGenresBurst(struct genreinfolst** genre_info, unsigned long long start, unsigned long long end);
	int queryGenreSongs(const char* genre, struct songinfolst** song_info);

	void parseSongs(char* resp, struct songinfolst** song_info);
	void parseAlbums(char* resp, struct albuminfolst** album_info);
	void parseArtists(char* resp, struct artistinfolst** artist_info);
	void parseGenre(char* resp, struct genreinfolst** genre_info);

	char* parseFieldStr(char** dest, char* base, char endchar);
	char* parseFieldLong(unsigned long long* dest, char* base, char endchar);

	void initSong(struct songinfolst** song_info);
	void initAlbum(struct albuminfolst** album_info);
	void initArtist(struct artistinfolst** artist_info);
	void initGenre(struct genreinfolst** genre_info);

	int substrsize(char* str, char until);
	int substr(char* base, char until, char* cpy, int cpySize);

	void addPlaylist(char* name, struct playlist** list);
	void addSongToPlaylist(unsigned long long song_id, struct playlist* list);
	int savePlaylist(struct playlist* list, char* filepath);
	int loadPlaylist(struct playlist** list, char* filepath);
	int scanPlaylists(struct playlist** list);
	int deletePlaylist(const char* name);

	int deleteSongFromPlaylist(struct playlist* list, unsigned long row_id);

	void free_playlist(struct playlist* list);
	void free_songlst(struct songlst* list);
	void free_songinfolst(struct songinfolst* song_info);
	void free_albuminfolst(struct albuminfolst* album_info);
	void free_artistinfolst(struct artistinfolst* artist_info);
	void free_genreinfolst(struct genreinfolst* genre_info);

	void printSongInfo(struct songinfolst* song_info);
	void printAlbumInfo(struct albuminfolst* album_info);
	void printArtistInfo(struct artistinfolst* artist_info);
	void printGenreInfo(struct genreinfolst* genre_info);

	int queryEntity(unsigned long long entity_id, char flags);
	int receiveResponse(char** resp);
	void disconnect();
	void stop(int sig);
	void clearSock(int blocking);
#ifdef __cplusplus
}
#endif

#endif // MUSE_CLIENT_H
