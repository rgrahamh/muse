#include "muse_server.hpp"

int main(int argc, char** argv) {
	int last_char;
	ITEM** items;
	MENU* menus;
	WINDOW* win;
}

void cleanup(){
	endwin();
	exit(0);
}

void cleanupServ(){
	//Close socket
	cleanup();
}

void refresh(){
	return;
}

void backgroundProc(){
	return;
}

void updatePort(int new_port){
	port = new_port;
}

void addLibPath(char* lib_path){
	if(lib_path_num < 64){
		lib_paths[lib_path_num++] = lib_path;
	}
}

void removeLibPath(int idx){
	int i;
	if(lib_paths[i][0] != '\0'){
		for(i = idx; lib_paths[i][0] != '\0' && i < lib_path_num; i++){
			lib_paths[i] = lib_paths[i+1];
		}
		if(i == lib_path_num){
			memset(lib_paths[i], 0, 255);
		}
		lib_path_num--;
	}
}
