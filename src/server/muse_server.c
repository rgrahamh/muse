#include "muse_server.h"

//This is here because we were having linker issues when it was in the hpp
//Socket file descriptor
int sockfd;

//Integer to keep track of the max file size and a buffer to keep the max file.
int max_file_size;
char* file_buff;

int calledback = 0;

#ifdef TEST
 int main(int argc, char** argv){
 /*
 	if(argc == 2){
 		serve(argv[1]);
 	}
 	else{
 		serve(DEFAULT_PORT);
 	}
 */
	char* test[] = {"/home/rhouck/Music"};
 	scan(test, 1);
 	return 0;
 }
#endif

/**Spins up the MUSE server
 * @param port The port you wish to open the server on
 * @return If the server exited normally
 */
int serve(char* port){
	//Set up signals so that it cleans up properly
	signal(SIGTERM, stop);
	signal(SIGHUP, stop);

	//Initialize the file buffer
	max_file_size = 8388608;
	file_buff = (char*)calloc(max_file_size, 1);

	struct addrinfo seed;
	struct addrinfo* host;
	struct sockaddr_storage* client = (struct sockaddr_storage*)calloc(1, sizeof(struct sockaddr_storage*));

	//Seed the hints
	memset(&seed, 0, sizeof(struct addrinfo));
	seed.ai_flags = AI_PASSIVE;
	seed.ai_family = AF_UNSPEC;
	seed.ai_socktype = SOCK_STREAM;

	//Get our info
	getaddrinfo(NULL, port, &seed, &host);

	//Open the socket
	if((sockfd = socket(host->ai_family, host->ai_socktype, host->ai_protocol)) == -1){
		fprintf(stderr, "Error opening a socket!\n");
	}

	//Bind the socket to a port
	if(bind(sockfd, host->ai_addr, host->ai_addrlen)){
		fprintf(stderr, "Error binding a socket!\n");
	}

	//Set up the socket to listen
	if(listen(sockfd, 20)){
		fprintf(stderr, "Couldn't set up to listen!\n");
	}

	freeaddrinfo(host);

	socklen_t addr_len = sizeof(struct sockaddr_storage);
	int new_sockfd;
	while(1){
		//Accept a new connection
		new_sockfd = accept(sockfd, (struct sockaddr*)client, &addr_len);
		if(new_sockfd != -1){
			//Spawn a new child process
			if(!fork()){
				handleRequest(new_sockfd);
				close(new_sockfd);
				exit(0);
			}
			else{
				printf("Connection detected!\n");
			}
		}
		else{
			fprintf(stderr, "Error accepting the new connection!\n");
		}
	}
	
	return 0;
}

/**Handles the request 
 * @param new_sockfd The new socket file descriptor gotten through accept()
 * @return If the connection ended normally
 */
int handleRequest(int new_sockfd){
	//The incoming character buffer; this will be recieved from the client
	char* incoming = (char*)calloc(BUFF_SIZE, 1);
	//Holds the buffer for SQL queries
	char* query = (char*)calloc(500, 1);

	//Holds the incoming flags
	char incoming_flags = 0;
	//Holds the incoming message
	char* incoming_msg;
	//Holds the amount of bytes recieved
	int amnt_recv = 0;
	do{
		//Recieve the request, set the amount to the transferred data minus the flags.
		amnt_recv = recv(new_sockfd, incoming, BUFF_SIZE, 0) - 1;
		incoming_flags = *incoming;
		incoming_msg = incoming + 1;

		if((incoming_flags & REQ_TYPE_MASK) != TERMCON){
			sqlite3* db;
			//Open the database connection
			if(sqlite3_open("./muse.db", &db) != SQLITE_OK){
				printf("Could not open the sqlite database!\n");
			}
			switch(incoming_flags & REQ_TYPE_MASK){
				case REQSNG:
					sprintf(query, "SELECT file_path FROM song\nWHERE song_id='%lu';", *((unsigned long*)incoming_msg));
					sqlite3_exec(db, query, sendSongCallback, &new_sockfd, NULL);
					break;

				case QWRYSNG:
					break;

				case QWRYALBM:
					break;

				case QWRYART:
					break;

				case QWRYARTSNG:
					break;

				case QWRYGNR:
					break;

				case QWRYGNRSNG:
					break;
			}
			sqlite3_close(db);


			//Clear the buffers
			memset(incoming, 0, BUFF_SIZE);
			memset(query, 0, BUFF_SIZE);
		}
	}while((incoming_flags & REQ_TYPE_MASK) != TERMCON);

	free(query);
	free(incoming);

	return 0;
}

/**Sends a song over TCP
 * @param new_sockfd The socket fd to send the song over
 * @param song_id The number of results returned
 */
int sendSongCallback(void* new_sockfd, int colNum, char** result, char** column){
	//Open the file for reading
	FILE* file = fopen(result[0], "r");

	fseek(file, 0UL, SEEK_END);
	unsigned long file_size = ftell(file);
	if(file_size > max_file_size){
		max_file_size = file_size;
		file_buff = (char*)realloc(file_buff, max_file_size);
	}
	rewind(file);

	char* tmp_buff = file_buff;
	//I'm pulling a block at a time on my system (block size is 4096)
	for(unsigned long i = 0; i < file_size - 4096; i++){
		fread(tmp_buff, 1, 4096, file);
		tmp_buff += 4096;
	}
	if(file_size % 4096){
		fread(tmp_buff, 1, file_size % 4096, file);
	}
	fclose(file);
	return !send(*((int*)new_sockfd), file_buff, file_size, 0);
}

void printSongInfo(struct dbsonginfo* song_info){
	printf("Title: %s\nArtist: %s\nAlbum: %s\nYear: %d\nTrack: %d\nFilepath: %s\nGenre: %s\n",song_info->title, song_info->artist, song_info->album, song_info->year, song_info->track_num, song_info->filepath, song_info->genre);
}

int addSongCallback(void* sinfo, int colNum, char** result, char** column){
	calledback = 1;
	struct dbsonginfo* song_info = (struct dbsonginfo*)sinfo;
	char* query = (char*)calloc(1, BUFF_SIZE);

	unsigned long album_id = 0;
	unsigned long artist_id = 0;
	//Parsing the artist/album.
	printf("\n");
	printSongInfo(song_info);
	printf("ColNum: %i\n", colNum);
	for(int i = 0; i < colNum; i++){
		//ADD STRING CHECKING BECAUSE THE OR STATEMENT JUST MEANS WE ACCEPT THE FIRST SONG@
		printf("Column:%s\nResult:%s\n", column[i], result[i]);
		if((strcmp(column[i], "ALBUM_ID") == 0) && (strcmp(result[i+1], song_info->album) == 0)){
			printf("Found album_id: %lu\n", strtoul(result[i], NULL, 10));
			album_id = strtoul(result[i], NULL, 10);
		}
		else if(strcmp(column[i], "ARTIST_ID") == 0 && (strcmp(result[i+1], song_info->album) == 0)){
			printf("Found artist_id: %lu\n", strtoul(result[i], NULL, 10));
			artist_id = strtoul(result[i], NULL, 10);
		}
	}

	//printf("%lu\n", song_info->album_id);

	//Create new entry for the song and album, link the artist.
	if(album_id == 0){
		printf("Creating new album!\n");
		sprintf(query, "INSERT INTO album(name, year)\nVALUES(%s, %i);", song_info->album, song_info->year);
		sqlite3_exec(song_info->db, query, NULL, NULL, NULL);
		album_id = ++song_info->next_album;
	}
	//Create new entry for the song and artist, link the album.
	else if(artist_id == 0){
		printf("Creating new artist!\n");
		sprintf(query, "INSERT INTO artist(name)\nVALUES(%s);", song_info->artist);
		sqlite3_exec(song_info->db, query, NULL, NULL, NULL);
		artist_id = ++song_info->next_artist;
	}
	//Create new entry for the song, link the artist and album FK's
	printf("Creating new song!\n");
	sprintf(query, "INSERT INTO song(name, album_id, artist_id, track_num, filepath, genre)\nVALUES('%s', %lu, %lu, %i, '%s', '%s');", song_info->title, album_id, artist_id, song_info->track_num, song_info->filepath, song_info->genre);
	printf("%s\n", query);
	sqlite3_exec(song_info->db, query, NULL, NULL, NULL);
	printf("Got past the exec!\n");
	
	free(query);
	return 0;
}

int initAlbumID(void* sinfo, int colNum, char** result, char** column){
	((struct dbsonginfo*)sinfo)->next_album = colNum + 1;
	return 0;
}

int initArtistID(void* sinfo, int colNum, char** result, char** column){
	((struct dbsonginfo*)sinfo)->next_artist = colNum + 1;
	return 0;
}

/**Scans the file system for mp3s and updates the database based upon that
 * @param lib_paths The library paths to search
 * @param num_paths The number of library paths passed in
 * @return The number of files successfully scanned in
 */
int scan(char** lib_paths, int num_paths){
	DIR* dir;
	struct dirent* file_info;
	struct stat stat_info;
	char* curr_path = (char*)calloc(1, PATH_MAX);
	int subdir_num = 0;
	int subdir_max = 32;
	//Subdirectories that should be scanned
	char** subdirs = (char**)malloc(32 * sizeof(char*));

	//Variables that are re-used and are declared in this scope for the purpose of efficiency:
	char* start_path = (char*)calloc(1, PATH_MAX);
	char* query = (char*)calloc(1, 4096);
	struct dbsonginfo* song_info = (struct dbsonginfo*)malloc(sizeof(dbsonginfo));

	//SQLite database work
	sqlite3* db;
	song_info->db = db;
	//Open the database connection
	if(sqlite3_open("./muse.db", &db) != SQLITE_OK){
		printf("Could not open the sqlite database!\n");
	}

	sqlite3_exec(db, "SELECT album.id FROM album;", initAlbumID, song_info, NULL);
	sqlite3_exec(db, "SELECT artist.id FROM artist;", initArtistID, song_info, NULL);

	getcwd(curr_path, PATH_MAX);
	//For every path passed in,
	for(int i = 0; i < num_paths; i++){
		chdir(lib_paths[i]);
		//Open the directory stream
		if((dir = opendir(lib_paths[i])) != NULL){
			//Read in the file information
			while((file_info = readdir(dir)) != NULL){
				lstat(file_info->d_name, &stat_info);
				if(S_ISDIR(stat_info.st_mode)){
				//If not the previous or current directory
					if(strcmp(file_info->d_name, ".") != 0 && strcmp(file_info->d_name, "..") != 0){
						while(subdir_num > subdir_max){
							subdir_max *= 4;
							subdirs = (char**)realloc(subdirs, subdir_max * sizeof(char*));
						}
						getcwd(start_path, PATH_MAX);
						char* subdir = (char*)malloc(strlen(start_path) + strlen(curr_path)+2);
						strcpy(subdir, start_path);
						strcat(subdir, "/");
						strcat(subdir, file_info->d_name);
						subdirs[subdir_num] = subdir;
						subdir_num++;
					}
				}
				else{
					calledback = 0;
					int file_name_len = strlen(file_info->d_name);
					song_info->filepath = file_info->d_name;

					//Checking to see if the file is an mp3
					if(strcmp((file_info->d_name + (file_name_len - 4)), ".mp3") == 0){
						//printf("Found an mp3 file: %s\n", file_info->d_name);
						TagLib_File* tag_file = taglib_file_new(file_info->d_name);
						TagLib_Tag* tag = taglib_file_tag(tag_file);

						//Populating the songinfo struct
						song_info->title = taglib_tag_title(tag);
						song_info->artist = taglib_tag_artist(tag);
						song_info->album= taglib_tag_album(tag);
						song_info->comment = taglib_tag_comment(tag);
						song_info->year = taglib_tag_year(tag);
						song_info->track_num = taglib_tag_track(tag);
						song_info->genre = taglib_tag_genre(tag);

						//Make sure that we escape apostrophe character so it doesn't mess up the queries
						char* safe_title = escapeApostrophe(song_info->title);
						char* safe_artist = escapeApostrophe(song_info->artist);
						char* safe_album = escapeApostrophe(song_info->album);
						char* safe_genre = escapeApostrophe(song_info->genre);

						free(song_info->title);
						free(song_info->artist);
						free(song_info->album);
						free(song_info->genre);

						song_info->title = safe_title;
						song_info->artist = safe_artist;
						song_info->album = safe_album;
						song_info->genre = safe_genre;

						//Query for the artist and album
						sprintf(query, "SELECT album.id AS ALBUM_ID, album.name AS ALBUM_NAME, artist.id AS ARTIST_ID\nFROM album INNER JOIN song ON album.id = song.album_id INNER JOIN artist ON song.artist_id = artist.id\nWHERE album.name = '%s' OR artist.name = '%s';", song_info->album, song_info->artist);
						char** err = (char**)calloc(1, sizeof(char**));
						int error_code = 5;
						sqlite3_exec(db, query, addSongCallback, song_info, err);

						if(*err != NULL){
							fprintf(stdout, "%s\n%s\n%s\n", song_info->album, song_info->artist, *err);
						}

						//If there were no columns returned
						if(calledback == 0){
							//Create new entries for the artist, song, and album
							sprintf(query, "INSERT INTO artist(name)\nVALUES('%s');\n\nINSERT INTO album(name, year)\nVALUES('%s', %i);\n\nINSERT INTO song(name, album_id, artist_id, track_num, genre)\nVALUES('%s', %lu, %lu, %i, '%s');", song_info->artist, song_info->album, song_info->year, song_info->title, ++song_info->next_album, ++song_info->next_artist, song_info->track_num, song_info->genre);
							sqlite3_exec(db, query, NULL, NULL, err);

							if(*err != NULL){
								fprintf(stdout, "%s\n%s\n%s\n", song_info->album, song_info->artist, *err);
							}
						}

						//Free the taglib_file
						taglib_file_free(tag_file);
					}
				}
			}
		}
		//Go back to original directory in case there's relative filepaths
		chdir(curr_path);
	}
	//Close the db
	sqlite3_close(db);

	//Call scan recursively on subdirectories
	if(subdir_num >= 1){
		scan(subdirs, subdir_num);
		for(int i = 0; i < subdir_num; i++){
			free(subdirs[i]);
		}
	}

	//Free allocated space
	free(dir);
	free(file_info);
	free(song_info);
	free(query);
	free(curr_path);
	free(start_path);
	free(subdirs);
	return 0;
}

/**Stops the server
 * @param sig The signal that was sent to the server
 */
void stop(int sig){
	close(sockfd);
	exit(0);
}