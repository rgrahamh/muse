#include "client.h"

int main(int argc, char** argv){
	connectToServ("2442", "127.0.0.1");
	return 0;
}

void connectToServ(char* port, char* server_ip){
	struct addrinfo seed;
	struct addrinfo* client;

	memset(&seed, 0, sizeof(struct addrinfo));
	seed.ai_flags = AI_PASSIVE;
	seed.ai_family = AF_UNSPEC;
	seed.ai_socktype = SOCK_STREAM;

	//Get our info
	getaddrinfo(NULL, NULL, &seed, &client);

	//Open the socket
	if((sockfd = socket(client->ai_family, client->ai_socktype, client->ai_protocol)) == -1){
		fprintf(stderr, "Error opening a socket!\n");
	}

	//Bind the socket to a port
	if(bind(sockfd, client->ai_addr, client->ai_addrlen)){
		fprintf(stderr, "Error binding a socket!\n");
	}

	struct sockaddr_in server;
	server.sin_family = AF_UNSPEC;
	server.sin_addr.s_addr = inet_addr(server_ip);
	server.sin_port = htons(strtoul(port, 0, 10));

	connect(sockfd, (sockaddr*)&server, sizeof(struct sockaddr_in));
	printf("Got here!\n");
}
