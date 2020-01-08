#ifndef MUSE_SERVER_HPP
#define MUSE_SERVER_HPP
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "../shared.hpp"

#define DEFAULT_PORT "2442"
#define BUFF_SIZE 256

int serve(char* port);
int handleRequest(int new_sockfd);
void stop(int sig);

#endif
