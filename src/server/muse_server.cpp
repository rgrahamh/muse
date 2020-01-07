#include "muse_server.hpp"

int main(int argc, char** argv){
	if(argc == 2){
		serve(argv[1]);
	}
	else{
		serve(DEFAULT_PORT);
	}
	return 0;
}

int serve(char* port){
	//Set up signals so that it cleans up properly
	signal(SIGTERM, stop);
	signal(SIGHUP, stop);

	/*int err_code = 0;
	struct addrinfo hints;
	struct addrinfo* server_info;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;*/
}

void stop(int sig){
	close(sockfd);
	exit(0);
}
