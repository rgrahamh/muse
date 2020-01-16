#ifndef CLIENT_H
#define CLIENT_H

#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

int sockfd;

void connectToServ(char* port, char* ip);
void stop();

#endif // CLIENT_H
