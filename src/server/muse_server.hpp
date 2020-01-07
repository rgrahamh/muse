#ifndef MUSE_SERVER_HPP
#define MUSE_SERVER_HPP
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "../shared.hpp"

#define DEFAULT_PORT "2442"

int sock_fd;

int serve(char* port);
void stop(int sig);

#endif
