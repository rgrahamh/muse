#ifndef MUSE_SERVER_TUI_HPP
#define MUSE_SERVER_TUI_HPP
#include <curses.h>
#include <menu.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

#include "../shared.hpp"
#include "muse_server.hpp"

enum pages {
	MAIN_PAGE,
	PORT_PAGE,
	LIBRARY_PAGE,
	SERVER_PAGE
};

struct Menu {
	char** label_arr;
	char** option_arr;
	void** funct_arr;
};

// char** label_arr[4] = {{"1.", "2.", "3.", "4.", "5.", "6."},
//                        {"1.", "2."},
//                        {"1.", "2.", "3."},
//                        {"1.", "2."}};
//
// char** option_arr[4] = {{"Start In Background", "Change Port", "Library Location", "Refresh Database", "Start Server", "Exit"},
//                        {"Specify Port", "Back"},
//                        {"Add Path", "Remove Path", "Back"},
//                        {"Kill Server", "Run In Background"}};
//
// void** funct_arr[4] = {{(void*)backgroundProc, (void*)refresh, (void*)refresh, (void*)refresh, (void*)refresh, (void*)cleanup},
//                        {(void*)updatePort, (void*)refresh},
//                        {(void*)addLibPath, (void*)removeLibPath, (void*)refresh},
//                        {(void*)cleanupServ, (void*)backgroundProc}};

char* port = "2442";
char* lib_paths[64];
char lib_path_num;
int curr_page = MAIN_PAGE;
int muse_pid;

void cleanup();
void cleanupServ();
void backgroundProc();
void updatePort(char* new_port);
int addLibPath(char* lib_path);
int removeLibPath(int idx);

struct Menu main_page;

char* main_label_arr[6] = {"1.", "2.", "3.", "4.", "5.", "6."};
char* main_option_arr[6] = {"Start In Background", "Change Port", "Library Location", "Refresh Database", "Start Server", "Exit"};
void* main_funct_arr[6] = {(void*)backgroundProc, (void*)refresh, (void*)refresh, (void*)refresh, (void*)refresh, (void*)cleanup};

#endif
