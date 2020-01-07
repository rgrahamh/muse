#include "muse_server.hpp"

int serve(int port){
	signal(SIGTERM, stop);
	signal(SIGHUP, stop);
}

void stop(){
	close(sockfd);
	exit(0);
}
