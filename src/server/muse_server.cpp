#include "muse_server.hpp"

//This is here because we were having linker issues when it was in the hpp
int sockfd;

#ifdef TEST
 int main(int argc, char** argv){
 	if(argc == 2){
 		serve(argv[1]);
 	}
 	else{
 		serve(DEFAULT_PORT);
 	}
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

	struct addrinfo seed;
	struct addrinfo* host;
	struct sockaddr_storage* client;

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
	//The outgoing character buffer; this will be sent out to the client
	char* outgoing = (char*)calloc(BUFF_SIZE, 1);
	//The incoming character buffer; this will be recieved from the client
	char* incoming = (char*)calloc(BUFF_SIZE, 1);

	//Holds the ougoing flags
	char outgoing_flags = 0;
	//Holds the incoming flags
	char incoming_flags = 0;
	//Holds the incoming message
	char* incoming_msg;

	int amnt_sent, amnt_recv;
	do{
		//Recieve the request, set the amount to the transferred data minus the flags.
		amnt_recv = recv(new_sockfd, incoming, BUFF_SIZE, 0) - 1;
		incoming_flags = *incoming;
		incoming_msg = incoming + 1;

		if(incoming_flags & REQ_TYPE_MASK != TERMCON){
			switch(incoming_flags & REQ_TYPE_MASK){
				case REQSNG:
					sqlite3 db;
					//Open the database connection
					if(sqlite3_open("./muse.db", &db) != SQLITE_OK){
						printf("Could not open the sqlite database!\n");
					}
					sprintf()
					amnt_sent = sendSong(new_sockfd, *((unsigned long*)incoming_msg));
					sqlite3_close(&db);
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

			//Clear the buffers
			memset(outgoing, 0, BUFF_SIZE);
			memset(incoming, 0, BUFF_SIZE);
		}
	}while((incoming & REQ_TYPE) != TERMCON);

	free(incoming);
	free(outgoing);

	return 0;
}

/**Sends a song over TCP
 * @param new_sockfd The socket fd to send the song over
 * @param song_id The song ID
 */
int sendSong(int new_sockfd, unsigned long song_id, sqlite* db){
	

	//Close the database connection
	FILE* file = fopen(path, "r");

	return send(new_sockfd, outgoing, strlen(outgoing), 0);
}

/**Stops the server
 * @param sig The signal that was sent to the server
 */
void stop(int sig){
	close(sockfd);
	exit(0);
}
