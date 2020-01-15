#include "client.h"

int main(int argc, char** argv){
	connectToServ("2442", "127.0.0.1");
	struct albuminfo* album_info = (struct albuminfo*)calloc(sizeof(struct albuminfo), 1);
	queryAlbumSongs(25, album_info);
	free(album_info);
	return 0;
}

int connectToServ(char* port, char* server_ip){
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

	/*char* ch = "A message has been sent! Praise the sun!";
	if(send(sockfd, ch, 25, 0) == -1){
		printf("Could not send!\n");
	}*/

	return 0;
}

int queryAlbumSongs(unsigned long album_id, struct albuminfo* album_info){
	if(album_id == 0){
		return 1;
	}
	int register request_size = sizeof(unsigned long) + sizeof(char);
	char* request = (char*)calloc(request_size, 1);
	char flags = QWRYALBMSNG | ASC;
	request[0] = flags;
	*((unsigned long*)(request+1)) = album_id;
	if(send(sockfd, request, request_size, 0) == -1){
		printf("Could not send request!\n");
		return 1;
	}

	char* new_buff = (char*)calloc(4096, 1);
	if(recv(sockfd, new_buff, 4096, 0) == -1){
		printf("Error recieving response!\n");
	}

	printf("Response:\n%s", new_buff);

	free(new_buff);

	return 0;
}

void stop(int sig){
	//Terminate the connection
	send(sockfd, "\0", 1, 0);
	close(sockfd);
	exit(0);
}
