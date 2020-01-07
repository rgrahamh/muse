#ifndef MUSE_SERVER_TUI_HPP
#define MUSE_SERVER_TUI_HPP

#include <curses.h>
#include <menu.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <iostream>

#include "../shared.hpp"
#include "muse_server.hpp"

#define ARR_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

enum pages {
	MAIN_PAGE,
	PORT_PAGE,
	LIBRARY_PAGE,
	SERVER_PAGE
};

struct MenuItem {
	MenuItem(char* l, char* o, void* f) : label(l), option(o), funct(f) { }
	char* label;
	char* option;
	void* funct;
};

char* port = "2442";
char* lib_paths[64];
int lib_path_num;
int curr_page = LIBRARY_PAGE;
int muse_pid;

void cleanup();
void cleanupServ();
void backgroundProc();
void updatePort(char* new_port);
int addLibPath(char* lib_path);
int removeLibPath(int idx);

struct MenuItem main_page[] = {
	MenuItem("1.", "Start In Background", (void*)backgroundProc),
	MenuItem("2.", "Change Port", (void*)refresh),
	MenuItem("3.", "Library Location", (void*)refresh),
	MenuItem("4.", "Refresh Database", (void*)refresh),
	MenuItem("5.", "Start Server", (void*)refresh),
	MenuItem("6.", "Exit", (void*)cleanup)
};

struct MenuItem port_page[] = {
	MenuItem("1.", "Specify Port", (void*)updatePort),
	MenuItem("2.", "Back", (void*)refresh)
};

struct MenuItem library_page[] = {
	MenuItem("1.", "Add Path", (void*)addLibPath),
	MenuItem("2.", "Remove Path", (void*)removeLibPath),
	MenuItem("3.", "Back", (void*)refresh)
};

struct MenuItem server_page[] = {
	MenuItem("1.", "Kill Server", (void*)cleanupServ),
	MenuItem("2.", "Run In Background", (void*)backgroundProc)
};

struct MenuItem* page_select[] = { main_page, port_page, library_page, server_page };
int page_length[] = { ARR_SIZE(main_page), ARR_SIZE(port_page), ARR_SIZE(library_page), ARR_SIZE(server_page) };

#endif
