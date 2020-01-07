#include "muse_server_TUI.hpp"

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

void cleanupServ(int sockfd){
	kill(pid, SIGTERM);
	cleanup();
}

void refresh(){
	return;
}

void backgroundProc(){
	return;
}

void start(bool background){
	//If the child process
	if(!(muse_pid = fork())){
		//If serve returns anything but zero
		if(serve(port)){
			//Spit them out to the main page if something goes wrong in MUSE itself
			curr_page = MAIN_PAGE;
		}
	}
	if(background){
		//backgroundProc
		backgroundProc();
	}
	else{
		curr_page = SERVER_PAGE;
	}
	refresh();
}

void updatePort(int new_port){
	port = new_port;
}

int addLibPath(char* lib_path){
	if(lib_path_num < 64){
		lib_paths[lib_path_num++] = lib_path;
		return 0;
	}
	return 1;
}

int removeLibPath(int idx){
	int i;
	if(lib_paths[i][0] != '\0'){
		for(i = idx; lib_paths[i][0] != '\0' && i < lib_path_num; i++){
			lib_paths[i] = lib_paths[i+1];
		}
		if(i == lib_path_num){
			memset(lib_paths[i], 0, 255);
		}
		lib_path_num--;
		return 0;
	}
	return 1;
}
