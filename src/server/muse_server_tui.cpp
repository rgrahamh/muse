#include "muse_server_tui.hpp"

int main(int argc, char** argv) {
	int last_char;
	MENU* menu;
	WINDOW* win, *subwin;

	/* Initialize curses */
	initscr();
	cbreak();
	noecho();
	curs_set(0);
	keypad(stdscr, TRUE);

	FILE* ascii = fopen("../assets/banner.ascii", "r");

	/* Create menu */
	ITEM** empty_item_list = (ITEM**) calloc(1, sizeof(ITEM*));
	empty_item_list[0] = NULL;
	menu = new_menu((ITEM**) empty_item_list);

	/* Create menu window */
	win = newwin(LINES, COLS, 0, 0);
	subwin = derwin(win, 6, 78, getASCIIHeight(ascii)+2, 1);
	keypad(win, TRUE);

	/* Set menu window and sub window */
	set_menu_win(menu, win);
	set_menu_sub(menu, subwin);
	set_menu_mark(menu, " > ");

	/* Decorate menu */
	wprintASCII(win, ascii, 1, (COLS / 2) - (getASCIILength(ascii) / 2));
	mvwhline(win, getASCIIHeight(ascii)+1, 1, ACS_HLINE, COLS);
	box(win, 0, 0);
	fclose(ascii);
	refresh();

	changePage(win, menu, MAIN_PAGE);

	post_menu(menu);
	wrefresh(win);

	while((last_char = wgetch(win)) != KEY_F(1))
	{
		switch(last_char) {
			case KEY_DOWN:
				menu_driver(menu, REQ_DOWN_ITEM);
				break;
			case KEY_UP:
				menu_driver(menu, REQ_UP_ITEM);
				break;
			case KEY_F(2):
				changePage(win, menu, MAIN_PAGE);
				break;
			case KEY_F(3):
				changePage(win, menu, PORT_PAGE);
				break;
			case KEY_F(4):
				changePage(win, menu, LIBRARY_PAGE);
				break;
			case KEY_F(5):
				changePage(win, menu, SERVER_PAGE);
				break;
		}
		wrefresh(win);
	}

	cleanup(menu);
}

void changePage(WINDOW* &win, MENU* &menu, int page_num) {
	/* Cleanup old list of items */
	ITEM** old_items = menu_items(menu);
	int n_old_items = item_count(menu);

	for( int i = 0; i < n_old_items; ++i ) {
		free_item(old_items[i]);
	}

	/* Unpost and free all the memory taken up */
	unpost_menu(menu);

	/* Create a new list of items */
	int n_choices = page_length[page_num];
	ITEM** new_items = (ITEM**) calloc(n_choices+1, sizeof(ITEM*));
	for( int i = 0; i < n_choices; ++i ) {
		new_items[i] = new_item(page_select[page_num][i].label, page_select[page_num][i].option);
	}
	new_items[n_choices] = NULL;

	/* Update menu */
	set_menu_items(menu, new_items);
	post_menu(menu);
}


void cleanup(MENU* menu) {
	/* Cleanup old list of items */
	ITEM** old_items = menu_items(menu);
	int n_old_items = item_count(menu);

	for( int i = 0; i < n_old_items; ++i ) {
		free_item(old_items[i]);
	}

	/* Unpost and free all the memory taken up */
	unpost_menu(menu);
	free_menu(menu);

	/* show the cursor again before exiting */
	curs_set(2);

	endwin();
	exit(0);
}

void cleanupServ() {
	kill(muse_pid, SIGTERM);
	// cleanup();
}

void backgroundProc() {
	return;
}

void start(bool background) {
	//If the child process
	if(!(muse_pid = fork())) {
		//If serve returns anything but zero
		if(/* serve(port) */ TRUE) {
			//Spit them out to the main page if something goes wrong in MUSE itself
			// curr_page = MAIN_PAGE;
		}
	}
	if(background) {
		//backgroundProc
		backgroundProc();
	}
	else{
		// curr_page = SERVER_PAGE;
	}
	refresh();
}

void updatePort(char* new_port) {
	port = new_port;
}

int addLibPath(char* lib_path) {
	if(lib_path_num < 64) {
		lib_paths[lib_path_num++] = lib_path;
		return 0;
	}
	return 1;
}

int removeLibPath(int idx) {
	int i = 0;
	if( lib_paths[i][0] != '\0' ) {
		for( i = idx; lib_paths[i][0] != '\0' && i < lib_path_num; i++ ) {
			lib_paths[i] = lib_paths[i+1];
		}
		if( i == lib_path_num ) {
			memset(lib_paths[i], 0, 255);
		}
		lib_path_num--;
		return 0;
	}
	return 1;
}

void refreshDatabase() {
	return;
}
