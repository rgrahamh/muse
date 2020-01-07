#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#include "../shared.hpp"

int sockfd;

int serve(int port);
void stop();
