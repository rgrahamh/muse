#include "muse_client.h"

int sockfd;

#ifdef TEST
int main(int argc, char** argv){
	connectToServ("2442", "127.0.0.1");
	/*struct songinfolst* song_info;
	struct albuminfolst* album_info;
	struct artistinfolst* artist_info;
	struct genreinfolst* genre_info;

    getSong(14, "/tmp/muse_download_14.mp3");
    getSong(13, "/tmp/muse_download_13.mp3");
	queryAlbumSongs(25, &song_info);
	printSongInfo(song_info);
	free_songinfolst(song_info);

	queryArtistAlbums(25, &album_info);
	printAlbumInfo(album_info);
	free_albuminfolst(album_info);

	queryGenreSongs("Rock", &song_info);
	printSongInfo(song_info);
	free_songinfolst(song_info);

	queryGenres(&genre_info);
	printGenreInfo(genre_info);
	free_genreinfolst(genre_info);

	queryGenresBurst(&genre_info, 0, 3);
	printGenreInfo(genre_info);
	free_genreinfolst(genre_info);

	querySongs(&song_info);
	printSongInfo(song_info);
	free_songinfolst(song_info);

	querySongsBurst(&song_info, 0, 25);
	printSongInfo(song_info);
	free_songinfolst(song_info);

	queryAlbums(&album_info);
	printAlbumInfo(album_info);
	free_albuminfolst(album_info);

	queryAlbumsBurst(&album_info, 0, 25);
	printAlbumInfo(album_info);
	free_albuminfolst(album_info);

	queryArtists(&artist_info);
	printArtistInfo(artist_info);
	free_artistinfolst(artist_info);

	queryArtistsBurst(&artist_info, 0, 25);
	printArtistInfo(artist_info);
	free_artistinfolst(artist_info);

	getSong(1, "./Green_Grass_And_High_Tides.mp3");

	//Playlist testing
	struct playlist* playlists = NULL;
	char* buff = (char*)calloc(1, 42);
	strcpy(buff, "My Playlist");
	addPlaylist(buff, &playlists);
	addSongToPlaylist(50, playlists);
	addSongToPlaylist(1877, playlists);
	addSongToPlaylist(850, playlists);
	addSongToPlaylist(956, playlists);
	addSongToPlaylist(1158, playlists);
	savePlaylist(playlists, "./my_playlist.pl");

	buff = (char*)calloc(1, 42);
	strcpy(buff, "Playlist Two: Electric Boogaloo");
	addPlaylist(buff, &playlists);
	addSongToPlaylist(2222, playlists);
	addSongToPlaylist(1, playlists);
	addSongToPlaylist(165, playlists);
	addSongToPlaylist(3885, playlists);
	addSongToPlaylist(987, playlists);
	savePlaylist(playlists, "./my_playlist2.pl");
	
	//loadPlaylist(&playlists, "./my_playlist.pl");
	scanPlaylists(&playlists);
	printf("Song 1: %llu\n", playlists->first_song->id);
	printf("Song 2: %llu\n", playlists->first_song->next->id);
	printf("Song 3: %llu\n", playlists->first_song->next->next->id);
	printf("Song 4: %llu\n", playlists->first_song->next->next->next->id);
	printf("Song 5: %llu\n", playlists->first_song->next->next->next->next->id);

	printf("Song 1: %llu\n", playlists->prev->first_song->id);
	printf("Song 2: %llu\n", playlists->prev->first_song->next->id);
	printf("Song 3: %llu\n", playlists->prev->first_song->next->next->id);
	printf("Song 4: %llu\n", playlists->prev->first_song->next->next->next->id);
	printf("Song 5: %llu\n", playlists->prev->first_song->next->next->next->next->id);

	free_playlist(playlists);
	*/

	return 0;
}
#endif

/** Attempts to connect the application to an instance of the muse server on the given port and ip
 * @param server_ip The ip that you wish to connect to
 * @param port The port that you wish to connect to
 * @return 0 if successful, 1 otherwise
 */
int connectToServ(const  char* port, const char* server_ip){
	signal(SIGTERM, stop);
	signal(SIGHUP, stop);

	struct addrinfo seed,* server;

	memset(&seed, 0, sizeof(struct addrinfo));
	seed.ai_family = AF_UNSPEC;
	seed.ai_socktype = SOCK_STREAM;
	
	getaddrinfo(server_ip, port, &seed, &server);

	//Open the socket
	if((sockfd = socket(server->ai_family, server->ai_socktype, server->ai_protocol)) == -1){
		fprintf(stderr, "Error opening a socket!\n");
		return 1;
	}

	if(connect(sockfd, server->ai_addr, server->ai_addrlen) == -1){
		printf("Could not connect to the server!\n");
		return 1;
	}

	freeaddrinfo(server);

	return 0;
}

/** Gets the song with the id of song_id and then prints it to filepath
 * @param song_id The id of the requested song
 * @param filepath The filepath that the song will be printed to
 * @return 0 if successful, 1 otherwise.
 */
int getSong(unsigned long long song_id, char* filepath){
	FILE* file = fopen(filepath, "w");

    if( file == NULL ) {
        printf("File could not be opened!\n");
        return 1;
    }

	int request_size = sizeof(char) + sizeof(unsigned long long);
	char* request = (char*)malloc(request_size);
	request[0] = REQSNG;
	*((unsigned long long *)(request+1)) = song_id;
	
	if(send(sockfd, request, request_size, 0) == -1){
		printf("Could not send request!\n");
		return 1;
	}

	char* resp_size_str = (char*)malloc(sizeof(unsigned long long));
	if(recv(sockfd, resp_size_str, sizeof(unsigned long long), 0) == -1){
		return 1;
	}
	unsigned long long resp_size = (*((unsigned long long*)resp_size_str)) - sizeof(unsigned long long);
	char* resp = (char*)malloc(resp_size+1);
	char* resp_cursor = resp;

	unsigned long long amnt_recv = 0;
	while((amnt_recv += recv(sockfd, resp_cursor, resp_size - amnt_recv, 0)) < resp_size){
		resp_cursor = amnt_recv + resp;
	}
	fwrite(resp, sizeof(char), resp_size, file);

	//Flush the socket of all of the messiness that was sent after the actual song length
	recv(sockfd, resp, resp_size, MSG_DONTWAIT);

	free(resp_size_str);
	free(request);
	fclose(file);
	free(resp);
	return 0;
}

/** Queries all songs and returns them through song_info
 * @param song_info The songinfolst struct that is populated with song information
 * @return 0 if successful, 1 otherwise.
 */
int querySongs(struct songinfolst** song_info){
	char request = QWRYSNG | ASC | ORDSNG;

	if(send(sockfd, &request, 1, 0) == 0){
		printf("Could not send request!\n");
		return 1;
	}

	char* resp = NULL;
	if(receiveResponse(&resp)){
		initSong(song_info);
		return 1;
	}

	parseSongs(resp, song_info);

	free(resp);
	return 0;
}

/** Queries a burst of songs and returns them through song_info
 * @param song_info The songinfolst struct that is populated with song information
 * @param start The requested starting index of the burst
 * @param end The requested ending index of the burst
 * @return 0 if successful, 1 otherwise.
 */
int querySongsBurst(struct songinfolst** song_info, unsigned long long start, unsigned long long end){
	char* request = (char*)malloc(1 + (sizeof(unsigned long long) * 2));
	request[0] = QWRYSNGBRST | ASC | ORDSNG;
	*((unsigned long long*)(request+1)) = start;
	*((unsigned long long*)(request+1+sizeof(unsigned long long))) = end;

	if(send(sockfd, request, 1 + (sizeof(unsigned long long) * 2), 0) == 0){
		printf("Could not send request!\n");
		return 1;
	}

	char* resp = NULL;
	if(receiveResponse(&resp)){
		initSong(song_info);
		return 1;
	}

	parseSongs(resp, song_info);

	free(resp);
	return 0;
}

/** Queries all albums and returns them through album_info
 * @param album_info The albuminfolst struct that is populated with album information
 * @return 0 if successful, 1 otherwise.
 */
int queryAlbums(struct albuminfolst** album_info){
	char request = QWRYALBM | ASC | ORDSNG;

	if(send(sockfd, &request, 1, 0) == 0){
		printf("Could not send request!\n");
		return 1;
	}

	char* resp = NULL;
	if(receiveResponse(&resp)){
		initAlbum(album_info);
		return 1;
	}

	parseAlbums(resp, album_info);

	free(resp);
	return 0;
}

/** Queries a burst of albums and returns them through album_info
 * @param album_info The albuminfolst struct that is populated with album information
 * @param start The requested starting index of the burst
 * @param end The requested ending index of the burst
 * @return 0 if successful, 1 otherwise.
 */
int queryAlbumsBurst(struct albuminfolst** album_info, unsigned long long start, unsigned long long end){
	char* request = (char*)malloc(1 + (sizeof(unsigned long long) * 2));
	request[0] = QWRYALBMBRST | ASC | ORDSNG;
	*((unsigned long long*)(request+1)) = start;
	*((unsigned long long*)(request+1+sizeof(unsigned long long))) = end;

	if(send(sockfd, request, 1 + (sizeof(unsigned long long) * 2), 0) == 0){
		printf("Could not send request!\n");
		return 1;
	}

	char* resp = NULL;
	if(receiveResponse(&resp)){
		initAlbum(album_info);
		return 1;
	}

	parseAlbums(resp, album_info);

	free(resp);
	return 0;
}

/** Queries all songs from an album
 * @param album_id The id of the album that is being queried
 * @param song_info The songinfolst struct that is populated with song information
 * @return 0 if successful, 1 otherwise.
 */
int queryAlbumSongs(unsigned long long album_id, struct songinfolst** song_info){
	if(queryEntity(album_id, QWRYALBMSNG | ASC)){
		printf("Error querying entity!\n");
		return 1;
	}

	char* resp = NULL;
	if(receiveResponse(&resp)){
		initSong(song_info);
		return 1;
	}

	parseSongs(resp, song_info);

	free(resp);
	return 0;
}

/** Queries a burst of artists
 * @param artist_info The artistinfolst struct that is populated with artist information
 * @param start The requested starting index of the burst
 * @param end The requested ending index of the burst
 * @return 0 if successful, 1 otherwise.
 */
int queryArtists(struct artistinfolst** artist_info){
	char request = QWRYART | ASC | ORDSNG;

	if(send(sockfd, &request, 1, 0) == 0){
		printf("Could not send request!\n");
		return 1;
	}

	char* resp = NULL;
	if(receiveResponse(&resp)){
		initArtist(artist_info);
		return 1;
	}

	parseArtists(resp, artist_info);

	free(resp);
	return 0;
}

/** Queries all artists
 * @param artist_info The artistinfolst struct that is populated with artist information
 * @return 0 if successful, 1 otherwise.
 */
int queryArtistsBurst(struct artistinfolst** artist_info, unsigned long long start, unsigned long long end){
	char* request = (char*)malloc(1 + (sizeof(unsigned long long) * 2));
	request[0] = QWRYARTBRST | ASC | ORDSNG;
	*((unsigned long long*)(request+1)) = start;
	*((unsigned long long*)(request+1+sizeof(unsigned long long))) = end;


	if(send(sockfd, request, 1 + (sizeof(unsigned long long) * 2), 0) == 0){
		printf("Could not send request!\n");
		return 1;
	}

	char* resp = NULL;
	if(receiveResponse(&resp)){
		initArtist(artist_info);
		return 1;
	}

	parseArtists(resp, artist_info);

	free(resp);
	return 0;
}

/** Queries all artists from an album
 * @param album_id The id of the album that is being queried
 * @param artist_info The artistinfolst struct that is populated with artist information
 * @return 0 if successful, 1 otherwise.
 */
int queryArtistAlbums(unsigned long long artist_id, struct albuminfolst** album_info){
	if(queryEntity(artist_id, QWRYARTALBM | ORDYR | ASC)){
		printf("Error querying entity!\n");
		return 1;
	}

	char* resp = NULL;
	if(receiveResponse(&resp)){
		initAlbum(album_info);
		return 1;
	}

	parseAlbums(resp, album_info);

	free(resp);
	return 0;
}

/** Queries all genres
 * @param genre_info The genreinfolst struct that is populated with genre information
 * @return 0 if successful, 1 otherwise.
 */
int queryGenres(struct genreinfolst** genre_info){
	char request = QWRYGNR | ASC | ORDSNG;

	if(send(sockfd, &request, 1, 0) == 0){
		printf("Could not send request!\n");
		return 1;
	}

	char* resp = NULL;
	if(receiveResponse(&resp)){
		initGenre(genre_info);
		return 1;
	}

	parseGenre(resp, genre_info);

	free(resp);
	return 0;
}

/** Queries a burst of genres
 * @param genre_info The genreinfolst struct that is populated with genre information
 * @param start The requested starting index of the burst
 * @param end The requested ending index of the burst
 * @return 0 if successful, 1 otherwise.
 */
int queryGenresBurst(struct genreinfolst** genre_info, unsigned long long start, unsigned long long end){
	char* request = (char*)malloc(1 + (sizeof(unsigned long long) * 2));
	request[0] = QWRYGNRBRST | ASC | ORDSNG;
	*((unsigned long long*)(request+1)) = start;
	*((unsigned long long*)(request+1+sizeof(unsigned long long))) = end;

	if(send(sockfd, request, 1 + (sizeof(unsigned long long) * 2), 0) == 0){
		printf("Could not send request!\n");
		return 1;
	}

	char* resp = NULL;
	if(receiveResponse(&resp)){
		initGenre(genre_info);
		return 1;
	}

	parseGenre(resp, genre_info);

	free(resp);
	return 0;
}

/** Queries all songs from an genre
 * @param genre_id The id of the genre that is being queried
 * @param song_info The songinfolst struct that is populated with song information
 * @return 0 if successful, 1 otherwise.
 */
int queryGenreSongs(const char* genre, struct songinfolst** song_info){
	if(genre == NULL){
		return 1;
	}
	int genre_size = strlen(genre);
	int request_size = genre_size + sizeof(char);
	char* request = (char*)calloc(request_size, 1);
	request[0] = QWRYGNRSNG | ASC | ORDSNG;
	strncpy(request+1, genre, genre_size);

	if(send(sockfd, request, request_size, 0) == -1){
		printf("Could not send request!\n");
		return 1;
	}

	free(request);

	char* resp = NULL;
	if(receiveResponse(&resp)){
		initSong(song_info);
		free(request);
		return 1;
	}

	parseSongs(resp, song_info);

	free(resp);
	return 0;
}

/** A function that queries for an entity id with given flags (a generalized function to minimize repeated code)
 * @param entity_id The id of the entity being queried
 * @param flags The request flags that are being sent
 * @return 1 if unsuccessful, 0 otherwise
 */
int queryEntity(unsigned long long entity_id, char flags){
	if(entity_id == 0){
		return 1;
	}
	int request_size = sizeof(unsigned long long) + sizeof(char);
	char* request = (char*)calloc(request_size, 1);
	request[0] = flags;
	*((unsigned long long*)(request+1)) = entity_id;

	if(send(sockfd, request, request_size, 0) == -1){
		printf("Could not send request!\n");
		free(request);
		return 1;
	}

	free(request);
	return 0;
}

/** Parses songs into the given songinfolst from the server's response
 * @param resp The response received from the server
 * @param song_info The songinfolst struct that is populated with song information
 */
void parseSongs(char* resp, struct songinfolst** song_info){
	*song_info = (struct songinfolst*)calloc(1, sizeof(struct songinfolst));

	struct songinfolst* song_cursor = *song_info;
	char* resp_cursor = resp;

	int first_iter = 1;
	while(*(resp_cursor) != '\0'){
		struct songinfolst* new_song = (first_iter == 1)? song_cursor : (struct songinfolst*)calloc(1, sizeof(struct songinfolst));
		resp_cursor = parseFieldLong(&(new_song->id), resp_cursor, '\t');
		resp_cursor = parseFieldStr(&(new_song->title), resp_cursor, '\t');
		resp_cursor = parseFieldStr(&(new_song->artist), resp_cursor, '\t');
		resp_cursor = parseFieldStr(&(new_song->album), resp_cursor, '\t');
		resp_cursor = parseFieldLong(&(new_song->year), resp_cursor, '\t');
		resp_cursor = parseFieldLong(&(new_song->track_num), resp_cursor, '\t');
		resp_cursor = parseFieldStr(&(new_song->genre), resp_cursor, '\n');
		if(!first_iter){
			song_cursor->next = new_song;
			song_cursor = song_cursor->next;
		}
		first_iter = 0;
	}
}

/** Parses albums into the given albuminfolst from the server's response
 * @param resp The response received from the server
 * @param album_info The albuminfolst struct that is populated with album information
 */
void parseAlbums(char* resp, struct albuminfolst** album_info){
	*album_info = (struct albuminfolst*)calloc(1, sizeof(struct albuminfolst));

	struct albuminfolst* album_cursor = *album_info;
	char* resp_cursor = resp;

	int first_iter = 1;
	while(*(resp_cursor) != '\0'){
		struct albuminfolst* new_album = (first_iter == 1)? album_cursor : (struct albuminfolst*)calloc(1, sizeof(struct albuminfolst));
		resp_cursor = parseFieldLong(&(new_album->id), resp_cursor, '\t');
		resp_cursor = parseFieldStr(&(new_album->title), resp_cursor, '\t');
		resp_cursor = parseFieldLong(&(new_album->year), resp_cursor, '\n');
		if(!first_iter){
			album_cursor->next = new_album;
			album_cursor = album_cursor->next;
		}
		first_iter = 0;
	}
}

/** Parses artists into the given artistinfolst from the server's response
 * @param resp The response received from the server
 * @param artist_info The artistinfolst struct that is populated with artist information
 */
void parseArtists(char* resp, struct artistinfolst** artist_info){
	*artist_info = (struct artistinfolst*)calloc(1, sizeof(struct artistinfolst));

	struct artistinfolst* artist_cursor = *artist_info;
	char* resp_cursor = resp;

	int first_iter = 1;
	while(*(resp_cursor) != '\0'){
		struct artistinfolst* new_artist = (first_iter == 1)? artist_cursor : (struct artistinfolst*)calloc(1, sizeof(struct artistinfolst));
		resp_cursor = parseFieldLong(&(new_artist->id), resp_cursor, '\t');
		resp_cursor = parseFieldStr(&(new_artist->name), resp_cursor, '\n');
		if(!first_iter){
			artist_cursor->next = new_artist;
			artist_cursor = artist_cursor->next;
		}
		first_iter = 0;
	}
}

/** Parses genres into the given genreinfolst from the server's response
 * @param resp The response received from the server
 * @param genre_info The genreinfolst struct that is populated with genre information
 */
void parseGenre(char* resp, struct genreinfolst** genre_info){
	*genre_info = (struct genreinfolst*)calloc(1, sizeof(struct genreinfolst));

	struct genreinfolst* genre_cursor = *genre_info;
	char* resp_cursor = resp;

	int first_iter = 1;
	while(*(resp_cursor) != '\0'){
		struct genreinfolst* new_genre = (first_iter == 1)? genre_cursor : (struct genreinfolst*)calloc(1, sizeof(struct genreinfolst));
		resp_cursor = parseFieldStr(&(new_genre->genre), resp_cursor, '\n');
		if(!first_iter){
			genre_cursor->next = new_genre;
			genre_cursor = genre_cursor->next;
		}
		first_iter = 0;
	}
}

/** Parses a field into an address of a char* from base to whenever it runs into the endchar (same functionality as parseFieldLong, but uses a different name to avoid improper C++ name mangling)
 * @param dest The final destination of the field
 * @param base The place the field is being read from
 * @param endchar The character to read until
 * @return The address of the character after the endchar
 */
char* parseFieldStr(char** dest, char* base, char endchar){
	int str_size = substrsize(base, endchar) + 1;
	char* new_str = (char*)malloc(str_size + 1);
	*dest = new_str;
	snprintf(new_str, str_size, "%s", base);
	new_str[str_size] = '\0';
	return base + str_size;
}

/** Parses a field into an address of an unsigned long long from base to whenever it runs into the endchar (same functionality as parseFieldStr, but uses a different name to avoid improper C++ name mangling)
 * @param dest The final destination of the field
 * @param base The place the field is being read from
 * @param endchar The character to read until
 * @return The address of the character after the endchar
 */
char* parseFieldLong(unsigned long long* dest, char* base, char endchar){
	int str_size = substrsize(base, endchar);
	char* num_str = (char*)malloc(str_size + 1);
	substr(base, '\t', num_str, str_size);
	num_str[str_size] = '\0';
	*dest = strtoul(num_str, NULL, 10);
	free(num_str);
	return base + str_size + 1;
}

/** Initializes an empty songinfolst
 * @param song_info The address of the songinfolst* you wish to initialize
 */
void initSong(struct songinfolst** song_info){
	*song_info = (struct songinfolst*)calloc(1, sizeof(struct songinfolst));
	struct songinfolst* song = *song_info;
	song->title = (char*)malloc(1);
	strcpy(song->title, "");
	song->id = 0;
	song->artist = (char*)malloc(1);
	strcpy(song->artist, "");
	song->album = (char*)malloc(1);
	strcpy(song->album, "");
	song->year = 0;
	song->track_num = 0;
	song->genre = (char*)malloc(1);
	strcpy(song->genre, "");
	song->next = NULL;
}

/** Initializes an empty albuminfolst
 * @param album_info The address of the albuminfolst* you wish to initialize
 */
void initAlbum(struct albuminfolst** album_info){
	*album_info = (struct albuminfolst*)calloc(1, sizeof(struct albuminfolst));
	struct albuminfolst* album = *album_info;
	album->title = (char*)malloc(1);
	strcpy(album->title, "");
	album->id = 0;
	album->year = 0;
	album->next = NULL;
}

/** Initializes an empty artistinfolst
 * @param artist_info The address of the artistinfolst* you wish to initialize
 */
void initArtist(struct artistinfolst** artist_info){
	*artist_info = (struct artistinfolst*)calloc(1, sizeof(struct artistinfolst));
	struct artistinfolst* artist = *artist_info;
	artist->name = (char*)malloc(1);
	strcpy(artist->name, "");
	artist->id = 0;
	artist->next = NULL;
}

/** Initializes an empty genreinfolst
 * @param genre_info The address of the genreinfolst* you wish to initialize
 */
void initGenre(struct genreinfolst** genre_info){
	*genre_info = (struct genreinfolst*)calloc(1, sizeof(struct genreinfolst));
	struct genreinfolst* genre = *genre_info;
	genre->genre = (char*)malloc(1);
	strcpy(genre->genre, "");
	genre->next = NULL;
}

/** Receives the response of the server and loads it into a char*
 * @param resp The address of the char* you wish to store the response in
 * @return 0 if successful, 1 otherwise
 */
int receiveResponse(char** resp){
	char* resp_size_str = (char*)malloc(sizeof(unsigned long long));
    if(recv(sockfd, resp_size_str, sizeof(unsigned long long), 0) == -1){
		printf("Error receiving data!\n");
		return 1;
	}
	unsigned long long resp_size = (*((unsigned long long*)(resp_size_str))) - sizeof(unsigned long long);
	if(resp_size == 0){
		printf("Response has no size!\n");
		return 1;
	}
	*resp = (char*)malloc(resp_size+1);
	char* resp_cursor = *resp;
	unsigned long long amnt_recv = 0;
	while((amnt_recv += recv(sockfd, resp_cursor, resp_size - amnt_recv, 0)) < resp_size){
		resp_cursor = amnt_recv + *resp;
	}
	(*resp)[resp_size] = '\0';
	free(resp_size_str);
	return 0;
}

/** Adds a new playlist
 * @param name The name of the new playlist
 * @param list The address of the start of the list of playlists
 */
void addPlaylist(char* name, struct playlist** list){
	struct playlist* new_playlist = (struct playlist*)calloc(1, sizeof(struct playlist));
	new_playlist->name = name;
	new_playlist->first_song = NULL;
	new_playlist->last_song = NULL;
	new_playlist->prev = *list;
	*list = new_playlist;
}

/** Adds a song to the playlist
 * @param song_id The id of the song that you wish to add to the playlist
 * @param list The list that you wish to add a song to
 */
void addSongToPlaylist(unsigned long long song_id, struct playlist* list){
	struct songlst* new_song = (struct songlst*)malloc(sizeof(struct songlst));
	new_song->id = song_id;
	new_song->next = NULL;
	if(list->first_song == NULL){
		list->first_song = new_song;
		list->last_song = new_song;
	}
	else{
	list->last_song->next = new_song;
	list->last_song = new_song;
	}
}

/** Adds a song to the playlist
 * @param song_id The id of the song that you wish to add to the playlist
 * @param list The list that you wish to add a song to
 * @return 0 if successful, 1 if the file could not be written
 */
int savePlaylist(struct playlist* list, char* filepath){
	unsigned int song_count = 0;
	struct songlst* cursor = list->first_song;
	while(cursor != NULL){
		song_count++;
		cursor = cursor->next;
	}

	FILE* file = fopen(filepath, "w");
	if(file == NULL){
		printf("Can't save playlist at %s!\n", filepath);
		return 1;
	}
	
	//Print the playlist name to the file
	fwrite(list->name, 1, strlen(list->name) + 1, file);

	//Print the song IDs to the file
	char* str = (char*)calloc(song_count, sizeof(unsigned long long));
	char* str_cursor = str;
	cursor = list->first_song;
	while(cursor != NULL){
		*((unsigned long long*)str_cursor) = cursor->id;
		str_cursor += sizeof(unsigned long long);
		cursor = cursor->next;
	}
	fwrite(str, sizeof(unsigned long long), song_count, file);
	fclose(file);

	return 0;
}

/**
 * @param list The address of the list that you want to load the playlist info into
 * @param filepath The filepath to the playlist
 * @return 0 if successful, 1 if the file can't be opened
 */
int loadPlaylist(struct playlist** list, char* filepath){
	struct playlist* play_list = (struct playlist*)calloc(1, sizeof(struct playlist));
	
	FILE* file = fopen(filepath, "r");
	if(file == NULL){
		printf("Can't open playplay_list at %s!\n", filepath);
		return 1;
	}

	unsigned int name_len = 0;
	char c;
	while((c = getc(file)) != '\0'){
		name_len++;
	}
	rewind(file);
	play_list->name = (char*)calloc(1, name_len+1);
	fread(play_list->name, name_len + 1, 1, file);

	//Parse the song IDs
	char* id_str = (char*)calloc(1, sizeof(unsigned long long) + 1);
	struct songlst* prev_song = (struct songlst*)calloc(1, sizeof(struct songlst));
	fread(id_str, 1, sizeof(unsigned long long), file);
	prev_song->id = *((unsigned long long*)id_str);
	prev_song->next = NULL;
	play_list->first_song = prev_song;
	fread(id_str, 1, sizeof(unsigned long long), file);
	while(!feof(file)){
		struct songlst* song = (struct songlst*)calloc(1, sizeof(struct songlst));
		song->id = *((unsigned long long*)id_str);
		song->next = NULL;
		prev_song->next = song;
		play_list->last_song = song;
		prev_song = song;
		fread(id_str, 1, sizeof(unsigned long long), file);
	}
	play_list->prev = *list;
	*list = play_list;

	free(id_str);

	return 0;
}

/** Scans the current directory for playlists and attempts to load them as they're found
 * @param list The address of the playlist that will be populated
 * @return The current number of playlists
 */
int scanPlaylists(struct playlist** list){
	DIR* dir;
	struct dirent* file_info;
	struct stat stat_info;
	if((dir = opendir(".")) != NULL){
		while((file_info = readdir(dir)) != NULL){
			lstat(file_info->d_name, &stat_info);
			if(strcmp((file_info->d_name + (strlen(file_info->d_name) - 3)), ".pl") == 0){
				loadPlaylist(list, file_info->d_name);
			}
		}
	}
	closedir(dir);
	return 0;
}

/** Frees the playlist
 * @param list The playlist that you want to be freed
 */
void free_playlist(struct playlist* plist){
	struct playlist* playlist_cursor = plist;
	while(playlist_cursor != NULL){
		//Free the songs in the playlist
		struct songlst* songlst_cursor = playlist_cursor->first_song;
		while(songlst_cursor != NULL){
			struct songlst* death_row = songlst_cursor;
			songlst_cursor = songlst_cursor->next;
			free(death_row);
		}
		//Free the playlist
		free(playlist_cursor->name);
		struct playlist* death_row = playlist_cursor;
		playlist_cursor = playlist_cursor->prev;
		free(death_row);
	}
}

/** Frees a given songinfolst
 * @param song_info The songinfolst you wish to free
 */
void free_songinfolst(struct songinfolst* song_info){
	struct songinfolst* cursor = song_info;
	struct songinfolst* tmp;
	while(cursor != NULL){
        free(cursor->title);
        free(cursor->artist);
        free(cursor->album);
        free(cursor->genre);

        tmp = cursor;
        cursor = cursor->next;
        free(tmp);
	}
}

/** Frees a given albuminfolst
 * @param album_info The albuminfolst you wish to free
 */
void free_albuminfolst(struct albuminfolst* album_info){
	struct albuminfolst* cursor = album_info;
	struct albuminfolst* tmp;
	while(cursor != NULL){
        free(cursor->title);

		tmp = cursor;
		cursor = cursor->next;
		free(tmp);
	}
}

/** Frees a given artistinfolst
 * @param artist_info The artistinfolst you wish to free
 */
void free_artistinfolst(struct artistinfolst* artist_info){
	struct artistinfolst* cursor = artist_info;
	struct artistinfolst* tmp;
	while(cursor != NULL){
        free(cursor->name);

		tmp = cursor;
		cursor = cursor->next;
		free(tmp);
	}
}

/** Frees a given genreinfolst
 * @param genre_info The genreinfolst you wish to free
 */
void free_genreinfolst(struct genreinfolst* genre_info){
    struct genreinfolst* cursor = genre_info;
    struct genreinfolst* tmp;
    while(cursor != NULL){
        free(cursor->genre);

        tmp = cursor;
        cursor = cursor->next;
        free(tmp);
    }
}

/** Iterates through a string and does a deep copy until a null byte or specified stopping point is hit
 * @param base The base string
 * @param until The ending character
 * @param cpy The char* to copy file contents into
 * @param cpySize The size of cpy
 * @return 1 on success, 0 otherwise
 */
int substr(char* base, char until, char* cpy, int cpySize){
    //Iterate through until we hit a null byte or the delim
    int i;
    for(i = 0; *base != until && i < cpySize; i++){
        *(cpy++) = *(base++);
    }
    if(i == cpySize){
        return 0;
    }
    *cpy = '\0';
    return 1;
}

/** Finds the number of characters between the start of str and either a null terminator or the until character
 * @param str The string you want to count through
 * @param until The end character being searched for
 * @return The number of characters between the start of str and either a null terminator or the until character
 */
int substrsize(char* str, char until){
	int i;
	for(i = 0; str[i] != until && str[i] != '\0'; i++){
		continue;
	}
	return i;
}

void printSongInfo(struct songinfolst* song_info){
	printf("Songs:\n");
	while(song_info != NULL){
		printf("%llu\n", song_info->id);
		printf("%s\n", song_info->title);
		printf("%s\n", song_info->artist);
		printf("%s\n", song_info->album);
		printf("%llu\n", song_info->year);
		printf("%llu\n", song_info->track_num);
		printf("%s\n\n", song_info->genre);
		song_info = song_info->next;
	}
}

void printAlbumInfo(struct albuminfolst* album_info){
	printf("Albums:\n");
	while(album_info != NULL){
		printf("%llu\n", album_info->id);
		printf("%s\n", album_info->title);
		printf("%llu\n\n", album_info->year);
		album_info = album_info->next;
	}
}

void printArtistInfo(struct artistinfolst* artist_info){
	printf("Artists:\n");
	while(artist_info != NULL){
		printf("%s\n", artist_info->name);
		printf("%llu\n", artist_info->id);
		artist_info = artist_info->next;
	}
}

void printGenreInfo(struct genreinfolst* genre_info){
	while(genre_info != NULL){
		printf("Genre: %s\n", genre_info->genre);
		genre_info = genre_info->next;
	}
}

/** Disconnects from the server */
void disconnect(){
	send(sockfd, "\0", 1, 0);
	close(sockfd);
}

/** The function that safely closes the socket upon exiting the application
 * @param sig The signal received
 */
void stop(int sig){
	//Terminate the connection
	send(sockfd, "\0", 1, 0);
	close(sockfd);
	exit(0);
}
