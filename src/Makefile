CC=g++
CFLAGS=-Wall -lmenu -lncurses -g

SERVER=shared.cpp \
	   server/muse_server.cpp \
	   server/muse_server_tui.cpp

muse_server:
	$(CC) $(SERVER) -o server/muse_server $(CFLAGS)

muse_client:
	$(CC) shared.cpp -o client/muse_client $(CFLAGS)
