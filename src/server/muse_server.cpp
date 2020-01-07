#include "muse_server.hpp"

int serve(char* port){
	signal(SIGTERM, stop);
	signal(SIGHUP, stop);

	return 0;
}

void stop(int sig){
	close(sockfd);
	exit(0);
}
