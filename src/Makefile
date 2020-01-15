CC=g++
CFLAGS=-Wall -Wno-write-strings -lmenu -lncurses -lsqlite3 -g -ltag_c -ltag

SERVER=shared.c \
	   server/muse_server.c \
	   server/muse_server_tui.cpp

CLIENT=client/client.c

muse_server:
	$(CC) $(SERVER) -o server/muse_server $(CFLAGS)

muse_client:
	$(CC) $(CLIENT) -o client/muse_client $(CFLAGS)

muse_server_test:
	$(CC) shared.c server/muse_server.c -o server/server_test $(CFLAGS) -DTEST
