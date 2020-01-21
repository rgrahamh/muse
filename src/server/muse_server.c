#include "muse_server.h"

//This is here because we were having linker issues when it was in the hpp
//Socket file descriptor
int sockfd;

int calledback = 0;

#ifdef TEST
int main(int argc, char** argv){

	/*
	//Test code for the linkedStr struct
	linkedstr* last = (linkedStr*)calloc(1,sizeof(struct linkedstr));

	char* str1 = (char*)malloc(256);
	char* str2 = (char*)malloc(256);
	char* str3 = (char*)malloc(256);
	char* str4 = (char*)malloc(256);
	char* str5 = (char*)malloc(256);

	strcpy(str1, "First element!");
	strcpy(str2, "Second element!");
	strcpy(str3, "Third element!");
	strcpy(str4, "Fourth element!");
	strcpy(str5, "Fifth element!");

	insertLinkedStr(last, str1);
	insertLinkedStr(last, str2);
	insertLinkedStr(last, str3);
	insertLinkedStr(last, str4);
	insertLinkedStr(last, str5);
	struct linkedStr* cursor = last->prev;

	while(cursor != last){
		printf("%s\n", cursor->str);
		cursor = cursor->prev;
	}
	
	freeLinkedStr(last);
	//End test code
	*/

	//char* test[] = {"/home/rhouck/Documents/music_iso"};

	FILE* log_file = fopen("/tmp/muse_server.log", "w");

 	if(argc == 2){
 		serve(argv[1], log_file);
 	}
 	else{
 		serve(DEFAULT_PORT, log_file);
 	}

	 fclose(log_file);

 	return 0;
 }
#endif

/**Spins up the MUSE server
 * @param port The port you wish to open the server on
 * @param log_file The log file that the server program should use to print useful output
 * @return If the server exited normally
 */
int serve(char* port, FILE* log_file){
	//Set up signals so that it cleans up properly
	signal(SIGTERM, stop);
	signal(SIGHUP, stop);
	signal(SIGUSR1, stop);

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

	//Log information
	fprintf(log_file, "Port number is: %s.\nAttempting server startup...\n", port);

	//Open the socket
	if((sockfd = socket(host->ai_family, host->ai_socktype, host->ai_protocol)) == -1){
		fprintf(log_file, "Error opening a socket!\n");
		return 1;
	}

	//Bind the socket to a port
	if(bind(sockfd, host->ai_addr, host->ai_addrlen)){
		fprintf(log_file, "Error binding a socket!\n");
		return 1;
	}

	//Set up the socket to listen
	if(listen(sockfd, 20)){
		fprintf(log_file, "Couldn't set up to listen!\n");
		return 1;
	}

	freeaddrinfo(host);

	socklen_t addr_len = sizeof(struct sockaddr_storage);
	while(1){
		//Accept a new connection
		int new_sockfd = accept(sockfd, (struct sockaddr*)client, &addr_len);
		if(new_sockfd != -1){
			//Spawn a new child process
			if(!fork()){
				handleRequest(new_sockfd, log_file);
				close(new_sockfd);
				exit(0);
			}
			else{
				fprintf(log_file, "Connection detected!\n");
			}
		}
		else{
			fprintf(log_file, "Error accepting the new connection!\n");
		}
	}

	return 0;
}

/**Handles the request 
 * @param new_sockfd The new socket file descriptor gotten through accept()
 * @return If the connection ended normally
 */
int handleRequest(int new_sockfd, FILE* log_file){
	//The incoming character buffer; this will be recieved from the client
	char* incoming = (char*)malloc(BUFF_SIZE);
	//Holds the buffer for SQL queries
	char* query = (char*)malloc(BUFF_SIZE);
	char* order_dir = (char*)calloc(5, 1);

	printf("handle request...\n");

	//Holds the incoming flags
	unsigned char incoming_flags = 0;
	//Holds the incoming message
	char* incoming_msg;
	//Holds the amount of bytes recieved
	int amnt_recv = 0;
	do{
		//Re-clear the memory
		memset(incoming, 0, BUFF_SIZE);
		memset(query, 0, BUFF_SIZE);

		//Recieve the request, set the amount to the transferred data minus the flags.
		amnt_recv = recv(new_sockfd, incoming, BUFF_SIZE, 0) - 1;
		incoming_flags = *incoming;
		incoming_msg = incoming + 1;

		if((incoming_flags & REQ_TYPE_MASK) != TERMCON){
			sqlite3* db;
			//Open the database connection
			if(sqlite3_open("./server/muse.db", &db) != SQLITE_OK){
				fprintf(log_file, "Could not open the sqlite database!\n");
				return 1;
			}

			//Set the order (opposite as recieved, since we're returning a linked list)
			if((incoming_flags & ORD_DIR_MASK) == ASC){
				strcpy(order_dir, "DESC");
			}
			else{
				strcpy(order_dir, "ASC");
			}
			struct linkedstr* results = NULL;
			//(struct linkedstr*)calloc(1, sizeof(struct linkedstr));
			//results->prev = NULL;

			//Parse the packet type
			switch(incoming_flags & REQ_TYPE_MASK){
				case REQSNG:
					//Read everything past the flags as a 
					sprintf(query, "SELECT song.filepath\nFROM song\nWHERE song.id = %lu;", *((unsigned long*)incoming_msg));
					sqlite3_exec(db, query, sendSongCallback, &new_sockfd, NULL);
					sqlite3_close(db);
					freeLinkedStr(results);
					continue;

				case QWRYSNG:
					sprintf(query, "SELECT song.id, song.name, artist.name, album.name, album.year, song.track_num, song.genre\nFROM album INNER JOIN song ON album.id = song.album_id INNER JOIN artist ON artist.id = song.artist_id\nORDER BY song.name %s;", order_dir);
					break;

				case QWRYALBM:
					sprintf(query, "SELECT DISTINCT album.id, album.name, album.year\nFROM album INNER JOIN song ON album.id = song.album_id INNER JOIN artist ON artist.id = song.artist_id\nORDER BY album.name %s;", order_dir);
					break;

				case QWRYALBMSNG:
					sprintf(query, "SELECT song.id, song.name, artist.name, album.name, album.year, song.track_num, song.genre\nFROM album INNER JOIN song on album.id = song.album_id INNER JOIN artist ON artist.id = song.artist_id\nWHERE album.id = %lu ORDER BY song.track_num DESC;", *((unsigned long*)incoming_msg));
					break;

				case QWRYART:
					sprintf(query, "SELECT DISTINCT artist.id, artist.name\nFROM artist\nORDER BY artist.name %s;", order_dir);
					break;

				case QWRYARTALBM:
					sprintf(query, "SELECT DISTINCT album.id, album.name, album.year\nFROM album INNER JOIN song ON album.id = song.album_id INNER JOIN artist ON artist.id = song.artist_id\nWHERE artist.id = %lu ORDER BY album.year %s;", *((unsigned long*)incoming_msg), order_dir);
					break;

				case QWRYGNR:
					sprintf(query, "SELECT DISTINCT song.genre\nFROM song\nORDER BY song.genre %s;", order_dir);
					break;

				case QWRYGNRSNG:
					char* safe_genre = escapeApostrophe(incoming_msg);
					sprintf(query, "SELECT song.id, song.name, artist.name, album.name, album.year, song.track_num, song.genre\nFROM artist INNER JOIN song ON artist.id = song.artist_id INNER JOIN album ON album.id = song.album_id\nWHERE song.genre LIKE '%s'\nORDER BY song.name %s;", safe_genre, order_dir);
					free(safe_genre);
					break;
			}
			sqlite3_exec(db, query, sendInfo, &results, NULL);

			printf("Received results\n");
			struct linkedstr* cursor = results;
			unsigned result_str_len = 0;

			printf("entering while\n");
			while(cursor != NULL){
				result_str_len += strlen(cursor->str);
				printf("cursor at:  %s\n", cursor->str);
				cursor = cursor->prev;
			}
			printf("exiting while\n");
			char* result_str = (char*)calloc(result_str_len + sizeof(unsigned long), 1);
			//Send the returned size as the first byte grouping
			*((unsigned long*)result_str) = result_str_len + sizeof(unsigned long);
			char* str_cursor = result_str+sizeof(unsigned long);

			printf("entering while2\n");
			cursor = results;
			while(cursor != NULL){
				strcat(str_cursor, cursor->str);
				cursor = cursor->prev;
				printf("%s\n", str_cursor);
			}
			printf("exiting while2\n");
			cursor = results;
			while(cursor != NULL){
				printf("%s", cursor->str);
				cursor = cursor->prev;
			}

			send(new_sockfd, result_str, result_str_len+sizeof(unsigned long), 0);
			
			freeLinkedStr(results);
			free(result_str);

			printf("closing database\n");

			int status = sqlite3_close(db);

			printf("Status is: %d\n", status);
		}
	}while((incoming_flags & REQ_TYPE_MASK) != TERMCON);

	free(query);
	free(incoming);

	return 0;
}

/** An sqlite callback function that sends a song over TCP
 * @param new_sockfd The socket fd to send the song over
 * @param col_num The number of columns returned
 * @param result The results given for each column
 * @param column The column that each index represents
 */
int sendSongCallback(void* new_sockfd, int col_num, char** result, char** column){
	//Open the file for reading
	FILE* file = fopen(result[0], "r");

	fseek(file, 0UL, SEEK_END);
	unsigned long file_size = ftell(file);
	rewind(file);

	char* file_buff = (char*)malloc(file_size + sizeof(unsigned long));

	*((unsigned long*)file_buff) = file_size;

	//I'm pulling a block at a time on my system (block size is 4096)
	char* tmp_buff = file_buff + sizeof(unsigned long);
	for(unsigned long i = 0; i < file_size; i += BLK_SIZE){
		fread(tmp_buff, 1, BLK_SIZE, file);
		tmp_buff += BLK_SIZE;
	}
	if(file_size % BLK_SIZE){
		fread(tmp_buff, 1, file_size % BLK_SIZE, file);
	}
	fclose(file);

	send(*((int*)new_sockfd), file_buff, file_size + sizeof(unsigned long), 0);
	free(file_buff);
	return 0;
}

/** Sends a response to the client's query
 * @param result_list The list of resultant strings
 * @param col_num The number of columns returned
 * @param result The results given for each column
 * @param column The column that each index represents
 */
int sendInfo(void* result_list, int col_num, char** result, char** column){
	unsigned int str_size;
	for(int i = 0; i < col_num; i++){
		str_size += strlen(result[i]) + 1;
	}
	//String size plus one for each tab plus one for the pointer plus one for the newline
	char* new_result = (char*)calloc(str_size + col_num + sizeof(unsigned long) + 1, 1);
	for(int i = 0; i < col_num - 1; i++){
		strcat(new_result, result[i]);
		strcat(new_result, "\t");
	}
	strcat(new_result, result[col_num-1]);
	strcat(new_result, "\n");
	insertLinkedStr((struct linkedstr**)result_list, new_result);

	return 0;
}

/** Gets the albums/artists matching what is scanned from the song
 * @param sinfo A struct dbsonginfo* that stuff is passed back in through
 * @param col_num The number of columns returned
 * @param result The results given for each column
 * @param column The column that each index represents
 */
int getAlbumArtist(void* sinfo, int col_num, char** result, char** column){
	calledback = 1;
	struct dbsonginfo* song_info = (struct dbsonginfo*)sinfo;

	//Parsing the artist/album.
	int album_col = 0, album_valid = 0, artist_col = 0, artist_valid = 0;
	for(int i = 0; i < col_num; i++){
		//ADD STRING CHECKING BECAUSE THE OR STATEMENT JUST MEANS WE ACCEPT THE FIRST SONG@
		if((strcmp(column[i], "ALBUM_ID") == 0)){
			album_col = i;
		}
		else if(strcmp(column[i], "ALBUM_NAME") == 0 && strcmp(result[i], song_info->album) == 0){
			album_valid = 1;
		}
		else if(strcmp(column[i], "ARTIST_ID") == 0){
			artist_col = i;
		}
		else if(strcmp(column[i], "ARTIST_NAME") == 0 && (strcmp(result[i], song_info->artist) == 0)){
			artist_valid = 1;
		}
	}
	if(artist_valid){
		song_info->artist_id = strtoul(result[artist_col], NULL, 10);
	}
	if(album_valid){
		song_info->album_id = strtoul(result[album_col], NULL, 10);
	}

	return 0;
}

/** Gets the album id from the database
 * @param A struct dbsonginfo* that the album id is passed back through to the calling function
 * @param col_num The number of columns returned
 * @param result The results given for each column
 * @param column The column that each index represents
 */
int initAlbumID(void* sinfo, int col_num, char** result, char** column){
	((struct dbsonginfo*)sinfo)->next_album = strtoul(result[0], NULL, 10);
	return 0;
}

/** Gets the artist id from the database
 * @param A struct dbsonginfo* that the artist id is passed back through to the calling function
 * @param col_num The number of columns returned
 * @param result The results given for each column
 * @param column The column that each index represents
 */
int initArtistID(void* sinfo, int col_num, char** result, char** column){
	((struct dbsonginfo*)sinfo)->next_artist = strtoul(result[0], NULL, 10);
	return 0;
}

/**Scans the file system for mp3s and updates the database based upon that
 * @param lib_paths The library paths to search
 * @param num_paths The number of library paths passed in
 * @return 0 if successful, 1 otherwise.
 */
int scan(char** lib_paths, int num_paths, FILE* log_file){
	DIR* dir;
	struct dirent* file_info;
	struct stat stat_info;
	char* curr_path = (char*)calloc(1, PATH_MAX);
	int subdir_num = 0;
	//Subdirectories that should be scanned
	char** subdirs = (char**)malloc(65535 * sizeof(char*));

	//Variables that are re-used and are declared in this scope for the purpose of efficiency:
	char* start_path = (char*)calloc(1, PATH_MAX);
	char* query = (char*)calloc(1, 4096);
	struct dbsonginfo* song_info = (struct dbsonginfo*)malloc(sizeof(struct dbsonginfo));

	//SQLite database work
	sqlite3* db;
	//Open the database connection
	if(sqlite3_open("./server/muse.db", &db) != SQLITE_OK){
		fprintf(log_file, "Could not open the sqlite database!\n");
		return 1;
	}
	song_info->db = db;

	sqlite3_exec(db, "SELECT song.filepath AS FILEPATH, song.id AS SONG_ID, artist.id AS ARTIST_ID, album.id AS ALBUM_ID\nFROM song INNER JOIN artist ON artist.id = song.artist_id INNER JOIN album ON song.album_id = album.id;", cullSongCallback, db, NULL);

	song_info->next_album = 0;
	song_info->next_artist = 0;
	sqlite3_exec(db, "SELECT COUNT(album.id) FROM album;", initAlbumID, song_info, NULL);
	sqlite3_exec(db, "SELECT COUNT(artist.id) FROM artist;", initArtistID, song_info, NULL);

	getcwd(curr_path, PATH_MAX);
	//For every path passed in,
	for(int i = 0; i < num_paths; i++){
		chdir(lib_paths[i]);
		//Open the directory stream
		if((dir = opendir(lib_paths[i])) != NULL){
			//Read in the file information
			while((file_info = readdir(dir)) != NULL){
				lstat(file_info->d_name, &stat_info);
				getcwd(start_path, PATH_MAX);
				char* full_path = (char*)malloc(strlen(start_path) + strlen(file_info->d_name)+2);
				strcpy(full_path, start_path);
				strcat(full_path, "/");
				strcat(full_path, file_info->d_name);
				if(S_ISDIR(stat_info.st_mode)){
				//If not the previous or current directory
					if(strcmp(file_info->d_name, ".") != 0 && strcmp(file_info->d_name, "..") != 0){
						subdirs[subdir_num++] = full_path;
					}
				}
				else{
					calledback = 0;
					int file_name_len = strlen(file_info->d_name);
					song_info->filepath = full_path;

					//Checking to see if the file is an mp3
					if((strcmp((file_info->d_name + (file_name_len - 4)), ".mp3") == 0)){
						TagLib_File* tag_file = taglib_file_new(file_info->d_name);
						//TODO: Investigate memory leak?
						TagLib_Tag* tag = taglib_file_tag(tag_file);

						//Populating the songinfo struct
						song_info->title = taglib_tag_title(tag);
						song_info->artist = taglib_tag_artist(tag);
						song_info->album = taglib_tag_album(tag);
						song_info->comment = taglib_tag_comment(tag);
						song_info->year = taglib_tag_year(tag);
						song_info->track_num = taglib_tag_track(tag);
						song_info->genre = taglib_tag_genre(tag);
						song_info->album_id = 0;
						song_info->artist_id = 0;

						//Free the taglib_file
						taglib_file_free(tag_file);

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

						sprintf(query, "SELECT song.id\nFROM song\nWHERE song.filepath = '%s';", song_info->filepath);
						if(!(sqlite3_exec(song_info->db, query, returnOne, song_info, NULL))){
							//Query for the artist and album
							//printSongInfo(song_info);
							sprintf(query, "SELECT album.id AS ALBUM_ID, album.name AS ALBUM_NAME, artist.id AS ARTIST_ID, artist.name AS ARTIST_NAME\nFROM album INNER JOIN song ON album.id = song.album_id INNER JOIN artist ON song.artist_id = artist.id\nWHERE album.name = '%s' OR artist.name = '%s'\nORDER BY album.id DESC;", song_info->album, song_info->artist);
							sqlite3_exec(db, query, getAlbumArtist, song_info, NULL);
							//If there were no columns returned
							if(calledback == 0){
								//Create new entries for the artist, song, and album
								sprintf(query, "INSERT INTO artist(name)\nVALUES('%s');\n\nINSERT INTO album(name, year)\nVALUES('%s', %i);\n\nINSERT INTO song(name, album_id, artist_id, track_num, filepath, genre)\nVALUES('%s', %lu, %lu, %i, '%s', '%s');", song_info->artist, song_info->album, song_info->year, song_info->title, ++(song_info->next_album), ++(song_info->next_artist), song_info->track_num, song_info->filepath, song_info->genre);

								sqlite3_exec(db, query, NULL, NULL, NULL);
							}
							else{
								//Create new entry for the song and album, link the artist.
								if(song_info->album_id == 0){
									sprintf(query, "INSERT INTO album(name, year)\nVALUES('%s', %i);\n\n", song_info->album, song_info->year);
									sqlite3_exec(db, query, NULL, NULL, NULL);
									song_info->album_id = ++(song_info->next_album);
								}
								//Create new entry for the song and artist, link the album.
								else if(song_info->artist_id == 0){
									sprintf(query, "INSERT INTO artist(name)\nVALUES('%s');\n\n", song_info->artist);
									sqlite3_exec(db, query, NULL, NULL, NULL);
									song_info->artist_id = ++(song_info->next_artist);
								}
								//Create new entry for the song, link the artist and album FK's
								sprintf(query, "INSERT INTO song(name, album_id, artist_id, track_num, filepath, genre)\nVALUES('%s', %lu, %lu, %i, '%s', '%s');", song_info->title, song_info->album_id, song_info->artist_id, song_info->track_num, song_info->filepath, song_info->genre);
								sqlite3_exec(db, query, NULL, NULL, NULL);
							}
							free(song_info->title);
							free(song_info->artist);
							free(song_info->album);
							free(song_info->comment);
							free(song_info->genre);
							free(full_path);
						}
					}
				}
			}
			free(file_info);
		}
		//Go back to original directory in case there's relative filepaths
		chdir(curr_path);
	}
	//Close the db
	while(sqlite3_close(db) == SQLITE_BUSY){
		continue;
	}

	//Free somme memory
	free(dir);
	free(song_info);
	free(query);
	free(curr_path);
	free(start_path);

	//Call scan recursively on subdirectories
	if(subdir_num >= 1){
		scan(subdirs, subdir_num, log_file);
		for(int i = 0; i < subdir_num; i++){
			free(subdirs[i]);
		}
	}

	free(subdirs);
	return 0;
}

/** Culls songs based on if their path can be found or not
 * @param datab The database being queried
 * @param col_num The number of columns returned
 * @param result The results given for each column
 * @param column The column that each index represents
 */
int cullSongCallback(void* datab, int col_num, char** result, char** column){
	sqlite3* db = (sqlite3*)datab;
	char* query = (char*)calloc(1, 4096);
	char album_still_exists = 0;
	char artist_still_exists = 0;
	if(access(result[0], F_OK | R_OK)){
		sprintf(query, "DELETE FROM song\nWHERE song.id = %lu;", strtoul(result[1], NULL, 10));
		sqlite3_exec(db, query, NULL, NULL, NULL);
		for(int i = 0; i < col_num; i++){
			if(strcmp("ALBUM_ID", column[i]) == 0){
				unsigned long album_id = strtoul(result[i], NULL, 10);
				sprintf(query, "SELECT album.id\nFROM album INNER JOIN song ON album.id = song.album_id\nWHERE song.album_id = %lu;", album_id);
				sqlite3_exec(db, query, deleteAlbum, &album_still_exists, NULL);
				if(!album_still_exists){
					sprintf(query, "DELETE FROM album\nWHERE album.id = %lu;", album_id);
					sqlite3_exec(db, query, NULL, NULL, NULL);
				}
			}
			else if(strcmp("ARTIST_ID", column[i]) == 0){
				unsigned long artist_id = strtoul(result[i], NULL, 10);
				sprintf(query, "SELECT artist.id\nFROM artist INNER JOIN song ON artist.id = song.artist_id\nWHERE song.artist_id = %lu;", artist_id);
				sqlite3_exec(db, query, deleteArtist, &artist_still_exists, NULL);
				if(!artist_still_exists){
					sprintf(query, "DELETE FROM artist\nWHERE artist.id = %lu;", artist_id);
					sqlite3_exec(db, query, NULL, NULL, NULL);
				}
			}
		}
	}
	free(query);

	return 0;
}

/** Returns one (used to see if something exists in an sqlite callback)
 * @return 1
 */
int returnOne(void* sinfo, int col_num, char** result, char** column){
	return 1;
}

/** Deletes artist based on if their path can be found or not
 * @param artist_still_exists Returns 1 (used to query if the artist still exists)
 * @param col_num The number of columns returned
 * @param result The results given for each column
 * @param column The column that each index represents
 */
int deleteArtist(void* artist_still_exists, int col_num, char** result, char** column){
	*((char*)artist_still_exists) = 1;
	return 0;
}

/** Deletes album based on if their path can be found or not
 * @param album_still_exists Returns 1 (used to query if the album still exists)
 * @param col_num The number of columns returned
 * @param result The results given for each column
 * @param column The column that each index represents
 */
int deleteAlbum(void* album_still_exists, int col_num, char** result, char** column){
	*((char*)album_still_exists) = 1;
	return 0;
}

/** Prints song information (used for debugging)
 * @param song_info The struct contatining the song information
 */
void printSongInfo(struct dbsonginfo* song_info){
	printf("Title: %s\nArtist: %s\nAlbum: %s\nYear: %d\nTrack: %d\nFilepath: %s\nGenre: %s\n",song_info->title, song_info->artist, song_info->album, song_info->year, song_info->track_num, song_info->filepath, song_info->genre);
}

/** Inserts a string into the linked list
 * @param prev The last node of the linked list you wish to insert into
 * @param element The char* that you wish to insert
 */
void insertLinkedStr(struct linkedstr** prev, char* element){
	struct  linkedstr* new_ls = (struct linkedstr*)malloc(sizeof(struct linkedstr));
	new_ls->str = element;
	new_ls->prev = *prev;
	*prev = new_ls;
}

/** Frees the linked list (and the strings contained therein)
 * @param last The last node of the linked list you wish to free
 */
void freeLinkedStr(struct linkedstr* last){
	while(last != NULL){
		struct linkedstr* death_row = last;
		last = last->prev;
		free(death_row->str);
		free(death_row);
	}
}

/**Stops the server
 * @param sig The signal that was sent to the server
 */
void stop(int sig){
	close(sockfd);
	exit(0);
}
