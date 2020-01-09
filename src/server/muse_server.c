#include "muse_server.h"

//This is here because we were having linker issues when it was in the hpp
//Socket file descriptor
int sockfd;

//Integer to keep track of the max file size and a buffer to keep the max file.
int max_file_size;
char* file_buff;

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
	char* test[] = {"."};
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
					sprintf(query, "SELECT file_path FROM song\nWHERE song_id=%lu;", *((unsigned long*)incoming_msg));
					sqlite3_exec(db, query, sendSongCallback, (void*)new_sockfd, NULL);
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
 * @param song_id The number of columns returned
 */
int sendSongCallback(void* new_sockfd, int colNum, char** column, char** result){
	if(colNum == 0){
		printf("Could not find the key!\n");
		return 0;
	}
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
	return !send((int)new_sockfd, file_buff, file_size, 0);
}

/**Scans the file system for mp3s and updates the database based upon that
 * @param lib_paths The library paths to search
 * @param num_paths The number of library paths passed in
 * @return The number of files successfully scanned in
 */
int scan(char** lib_paths, int num_paths){
	DIR* dir;
	struct dirent* file_info;
	//For every path passed in,
	for(int i = 0; i < num_paths; i++){
		//Open the directory stream
		if((dir = opendir(lib_paths[i])) != NULL){
			//Read in the file information
			while((file_info = readdir(dir)) != NULL){
				int file_name_len = strlen(file_info->d_name);

				//Checking to see if the file is an mp3
				if(strcmp((file_info->d_name + (file_name_len - 4)), ".mp3") == 0){
					printf("Found an mp3 file: %s\n", file_info->d_name);
					//TODO: Parse info
				}
			}
		}
	}
	return 0;
}

/**Stops the server
 * @param sig The signal that was sent to the server
 */
void stop(int sig){
	close(sockfd);
	exit(0);
}
