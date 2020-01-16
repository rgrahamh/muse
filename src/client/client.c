#include "client.h"

int main(int argc, char** argv){
	connectToServ("2442", "127.0.0.1");
	struct songinfo** song_info = NULL;
	struct albuminfo** album_info = NULL;
	queryAlbumSongs(25, song_info);
	queryArtistAlbums(25, album_info);
	free(song_info);
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

int queryArtistAlbums(unsigned long artist_id, struct albuminfo** album_info){
	if(queryEntity(artist_id, QWRYARTALBM | ORDYR | ASC)){
		printf("Error querying entity!\n");
		return 1;
	}

	char* resp = NULL;
	if(receiveResponse(&resp)){
		printf("Error recieving response!\n");
		return 1;
	}

	printf("Response:\n%s\n", resp);

	return 0;
}

int queryAlbumSongs(unsigned long album_id, struct songinfo** song_info){
	if(queryEntity(album_id, QWRYALBMSNG | ASC)){
		printf("Error querying entity!\n");
		return 1;
	}

	char* resp = NULL;
	if(receiveResponse(&resp)){
		printf("Error recieving response!\n");
		return 1;
	}

	printf("Response:\n%s\n", resp);

	return 0;
}

int queryEntity(unsigned long entity_id, char flags){
	if(entity_id == 0){
		return 1;
	}
	int request_size = sizeof(unsigned long) + sizeof(char);
	char* request = (char*)calloc(request_size, 1);
	request[0] = flags;
	*((unsigned long*)(request+1)) = entity_id;

	if(send(sockfd, request, request_size, 0) == -1){
		printf("Could not send request!\n");
		return 1;
	}

	return 0;
}

int receiveResponse(char** resp){
	char* resp_size_str = (char*)malloc(sizeof(unsigned long));
	if(recv(sockfd, resp_size_str, sizeof(unsigned long), 0) == -1){
		return 1;
	}
	unsigned long resp_size = (*((unsigned long*)(resp_size_str))) - sizeof(unsigned long);
	*resp = (char*)malloc(resp_size+1);
	//TODO: Make the server send the response size
	if(recv(sockfd, *resp, resp_size, 0) == -1){
		return 1;
	}
	(*resp)[resp_size] = '\0';
	free(resp_size_str);

	return 0;
}

void stop(int sig){
	//Terminate the connection
	send(sockfd, "\0", 1, 0);
	close(sockfd);
	exit(0);
}
