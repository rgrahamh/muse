#include "muse_client.h"

int main(int argc, char** argv){
	connectToServ("2442", "127.0.0.1");
	struct songinfolst* song_info = (struct songinfolst*)calloc(1, sizeof(struct songinfolst));
	struct albuminfolst* album_info = (struct albuminfolst*)calloc(1, sizeof(struct albuminfolst));
	struct artistinfolst* artist_info = (struct artistinfolst*)calloc(1, sizeof(struct artistinfolst));
	//queryAlbumSongs(25, &song_info);
	//queryArtistAlbums(25, &album_info);
	//queryGenreSongs("Rock", &song_info);
	//queryGenre(argv);
	//querySongs(&song_info);
	//queryAlbums(&album_info);
	//queryArtists(&artist_info);
	getSong(4062);
	free_songinfolst(song_info);
	free_albuminfolst(album_info);
	free_artistinfolst(artist_info);
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

	freeaddrinfo(server);

	/*char* ch = "A message has been sent! Praise the sun!";
	if(send(sockfd, ch, 25, 0) == -1){
		printf("Could not send!\n");
	}*/

	return 0;
}

int getSong(unsigned long song_id){
	FILE* file = fopen("./song.mp3", "w");

	int request_size = sizeof(char) + sizeof(unsigned long);
	char* request = (char*)malloc(request_size);
	request[0] = REQSNG;
	*((unsigned long *)(request+1)) = song_id;
	
	if(send(sockfd, request, request_size, 0) == -1){
		printf("Could not send request!\n");
		return 1;
	}

	char* resp_size_str = (char*)malloc(sizeof(unsigned long));
	if(recv(sockfd, resp_size_str, sizeof(unsigned long), 0) == -1){
		return 1;
	}
	unsigned long resp_size = (*((unsigned long*)(resp_size_str))) - sizeof(unsigned long);
	printf("Resp size: %lu\n", resp_size);
	char* resp = (char*)malloc(resp_size);

	if(recv(sockfd, resp, resp_size, 0) == -1){
		return 1;
	}
	free(resp_size_str);

	fwrite(resp, sizeof(char), resp_size, file);

	fclose(file);
	free(resp);

	return 0;
}

int querySongs(struct songinfolst** song_info){
	char request = QWRYSNG | ASC | ORDSNG;

	if(send(sockfd, &request, 1, 0) == 0){
		printf("Could not send request!\n");
		return 1;
	}

	char* resp = NULL;
	if(receiveResponse(&resp)){
		printf("Error recieving response!\n");
		return 1;
	}

	printf("Response:\n%s\n", resp);

	free(resp);

	return 0;
}

int queryAlbums(struct albuminfolst** album_info){
	char request = QWRYALBM | ASC | ORDSNG;

	if(send(sockfd, &request, 1, 0) == 0){
		printf("Could not send request!\n");
		return 1;
	}

	char* resp = NULL;
	if(receiveResponse(&resp)){
		printf("Error recieving response!\n");
		return 1;
	}

	printf("Response:\n%s\n", resp);

	free(resp);

	return 0;
}

int queryAlbumSongs(unsigned long album_id, struct songinfolst** song_info){
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

	free(resp);

	return 0;
}

int queryArtists(struct artistinfolst** artist_info){
	char request = QWRYART | ASC | ORDSNG;

	if(send(sockfd, &request, 1, 0) == 0){
		printf("Could not send request!\n");
		return 1;
	}

	char* resp = NULL;
	if(receiveResponse(&resp)){
		printf("Error recieving response!\n");
		return 1;
	}

	printf("Response:\n%s\n", resp);

	free(resp);

	return 0;
}

int queryArtistAlbums(unsigned long artist_id, struct albuminfolst** album_info){
	if(queryEntity(artist_id, QWRYARTALBM | ORDYR | ASC)){
		printf("Error querying entity!\n");
		return 1;
	}

	//*album_info = (struct albuminfolst*)calloc(1, sizeof(struct albuminfolst));

	char* resp = NULL;
	if(receiveResponse(&resp)){
		printf("Error recieving response!\n");
		free(album_info);
		return 1;
	}

	printf("Response:\n%s\n", resp);

	/*struct albuminfolst* cursor = *album_info;
	while(
		cursor->title = 
	(*albuminfolst)->next = new_album;*/
	return 0;
}


int queryGenre(char** genres){
	char request = QWRYGNR | ASC | ORDSNG;

	if(send(sockfd, &request, 1, 0) == 0){
		printf("Could not send request!\n");
		return 1;
	}

	char* resp = NULL;
	if(receiveResponse(&resp)){
		printf("Error recieving response!\n");
		return 1;
	}

	printf("Response:\n%s\n", resp);

	free(resp);

	return 0;
}

int queryGenreSongs(char* genre, struct songinfolst** song_info){
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
		printf("Error recieving response!\n");
		return 1;
	}

	printf("Response:\n%s\n", resp);

	free(resp);

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

	free(request);

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

void free_songinfolst(struct songinfolst* song_info){
	struct songinfolst* cursor = song_info;
	struct songinfolst* tmp;
	while(cursor != NULL){
		free(song_info->title);
		free(song_info->artist);
		free(song_info->album);
		free(song_info->genre);
		tmp = cursor;
		cursor = cursor->next;
		free(tmp);
	}
}

void free_albuminfolst(struct albuminfolst* album_info){
	struct albuminfolst* cursor = album_info;
	struct albuminfolst* tmp;
	while(cursor != NULL){
		free(album_info->title);
		tmp = cursor;
		cursor = cursor->next;
		free(tmp);
	}
}

void free_artistinfolst(struct artistinfolst* artist_info){
	struct artistinfolst* cursor = artist_info;
	struct artistinfolst* tmp;
	while(cursor != NULL){
		free(artist_info->name);
		tmp = cursor;
		cursor = cursor->next;
		free(tmp);
	}
}

//TODO: Remove circularly linked list in server, change out w/ normal list. I dunno why I thought circularly linked was the right implementation for that :P

void stop(int sig){
	//Terminate the connection
	send(sockfd, "\0", 1, 0);
	close(sockfd);
	exit(0);
}
