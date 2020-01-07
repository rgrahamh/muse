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
		new_sockfd = accept(sockfd, (struct sockaddr*)client, &addr_len);
		if(new_sockfd != -1){
			if(!fork()){
				handleRequest();
				exit(0);
			}
			else(printf("Connection detected!\n"));
		}
		else{
			fprintf(stderr, "Error accepting the new connection!\n");
		}
	}
	
	return 0;
}

int handleRequest(){
}

void stop(int sig){
	close(sockfd);
	exit(0);
}
