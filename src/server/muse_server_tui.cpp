#include "muse_server_tui.hpp"

int main(int argc, char** argv) {
	int last_char, ascii_height, ascii_length;
	MENU* menu;
	WINDOW *win, *menu_win;

	/* Initialize the port */
	strncpy(port, "2442", sizeof(port) - 1);
	strncpy(ip, "192.168.122.231", sizeof(ip) - 1);

	/* Initialize curses */
	initscr();
	cbreak();
	noecho();
	curs_set(0);
	keypad(stdscr, TRUE);

	FILE* ascii = fopen("../assets/banner.ascii", "r");
	ascii_height = getASCIIHeight(ascii);
	ascii_length = getASCIILength(ascii);

	/* Create menu */
	ITEM** empty_item_list = (ITEM**) calloc(1, sizeof(ITEM*));
	empty_item_list[0] = NULL;
	menu = new_menu((ITEM**) empty_item_list);

	/* Create menu window */
	win = newwin(LINES, COLS, 0, 0);
	menu_win = derwin(win, 6, (COLS / 2), ascii_height+2, 1);
	keypad(win, TRUE);

	/* Set menu window and sub window */
	set_menu_win(menu, win);
	set_menu_sub(menu, menu_win);
	set_menu_mark(menu, " > ");

	/* Decorate menu */
	wprintASCII(win, ascii, 1, (COLS / 2) - (ascii_length / 2));
	mvwhline(win, ascii_height+1, 1, ACS_HLINE, COLS);
	mvwvline(win, ascii_height+2, (COLS / 2) + 1, ACS_VLINE, LINES);
	box(win, 0, 0);
	refresh();

	changePage(menu, MAIN_PAGE);

	post_menu(menu);
	writeInfoWindow(win, ascii_height+2, (COLS / 2)+2);
	wrefresh(win);

	fclose(ascii);

	while((last_char = wgetch(win)) != KEY_F(1))
	{
		switch(last_char) {
			case KEY_DOWN:
				menu_driver(menu, REQ_DOWN_ITEM);
				break;
			case KEY_UP:
				menu_driver(menu, REQ_UP_ITEM);
				break;
			case 10: /* ENTER */
				ITEM* selected = current_item(menu);
				void* callback = item_userptr(selected);

				int idx = item_index(selected);
				handleMenuCallback(win, menu, callback, idx);
				break;
		}
		writeInfoWindow(win, ascii_height+2, (COLS / 2)+2);
		wrefresh(win);
	}

	cleanup(menu);
}

/**Changes the currently selected page
 * @param menu The menu which needs to be updated
 * @param page_num The enumerated value that corresponds to a menu page
 */
void changePage(MENU* &menu, int page_num) {
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
		set_item_userptr(new_items[i], page_select[page_num][i].funct);
	}
	new_items[n_choices] = NULL;

	curr_page = page_num;

	/* Update menu */
	set_menu_items(menu, new_items);
	post_menu(menu);
}

/**Handles and processes callback functions when selecting a menu item
 * @param win The window in which the menu lives
 * @param menu The menu in which the item was selected
 * @param callback A pointer to the callback function that is associated with the selected menu item
 * @param index The index of the selected menu item
 */
void handleMenuCallback(WINDOW* &win, MENU* &menu, void* callback, int index) {
	if( callback == &changePage ) {
		int page_dest = page_select[curr_page][index].chng_pg_to;
		changePage(menu, page_dest);
	} else if( callback == &updateIP ) {
		updateIP(win);
	} else if( callback == &updatePort ) {
		updatePort(win);
	} else if( callback == &exitMuse ) {
		exitMuse(win, menu);
	} else {
		return;
	}
}

/**Writes various informations to the screen
 * @param win The window in which the info lives
 * @param y The y coordinate to act as an origin for information
 * @param x The x coordinate to act as an origin for information
 */
void writeInfoWindow(WINDOW* &win, int y, int x) {
	/* Write the network information */
	char* ip_label = "IP: ";
	char* port_label = "Port: ";
	char* exit_label = "Press F1 to exit.";

	mvwaddstr(win, y, x, ip_label);
	mvwaddstr(win, y, x+strlen(ip_label), ip);

	mvwaddstr(win, y, x+strlen(ip_label)+strlen(ip)+1, port_label);
	mvwaddstr(win, y, x+strlen(ip_label)+strlen(ip)+strlen(port_label)+1, port);

	mvwaddstr(win, LINES - 2, COLS - strlen(exit_label)-1, exit_label);

	/* Draw the vertical seperator line */
	mvwvline(win, y, x - 1, ACS_VLINE, LINES - y - 1);
	wrefresh(win);
}

/**Prompts the user to confirm (ENTER) or cancel (ESC) from a selection
 * @param win The window in which the menu lives
 */
int confirmSelection(WINDOW* &win) {
		int last_char = 0;
		mvwaddstr(win, LINES - 2, 1, "Press <ENTER> to confirm or <ESC> to cancel.");

		int response = -1;

		/* Wait for confirm / cancel */
		while( last_char != 27 /* ESC */ && last_char != 10 /* ENTER */ ) {
			last_char = wgetch(win);
			switch(last_char) {
				case 10:
					response = 1;
					break;
				case 27:
					response = 0;
					break;
			}
		}

		for( int x = 1; x < COLS - 1; x++ ) {
			mvwaddch(win, LINES - 2, x, ' ');
		}

		return response;
}

/**Exits Muse, user has to confirm
 * @param win The window in which the menu lives
 * @param menu The menu to be cleaned up should Muse be exited
 */
void exitMuse(WINDOW* &win, MENU* &menu) {
	int confirmed = confirmSelection(win);
	if( confirmed == 1 ) {
		cleanup(menu);
	}
}


/**Frees used memory and otherwise cleans up the project space before closing
 * @param menu The menu which needs to be cleaned
 */
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
	curs_set(1);

	endwin();
	exit(0);
}

void cleanupServ() {
	return;
	// kill(muse_pid, SIGTERM);
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

/**Updates the IP address with user prompt
 * @param win The window which prompts the user for input
 */
void updateIP(WINDOW* &win) {
	/* Show the cursor */
	curs_set(1);
	int last_char;
	char new_ip[16] = "";
	int index = 0;

	/* Move the cursor to the proper screen position */
	int y = LINES - 3;
	int x = 1;
	wmove(win, y, x);

	/* Write a prompt to the user */
	mvwaddstr(win, y, x, "New IP (<ESC> to cancel): ");

	/* User types in 3 digits at a time, auto-filling the period every 3 */
	while( index < 15 && (last_char = wgetch(win)) != 27 /* ESC */ ) {
		switch(last_char) {
			case 48:
			case 49:
			case 50:
			case 51:
			case 52:
			case 53:
			case 54:
			case 55:
			case 56:
			case 57: /* digits 0-9 */
				waddch(win, last_char);
				new_ip[index++] = last_char;
				break;
		}

		if( (index % 4) - 3 == 0 && index != 0 && index != 15 ) { // autofill periods for user
			waddch(win, '.');
			new_ip[index++] = (int)'.';
		} 

		wrefresh(win);
	}

	/* If proper digits entered, ask for confirmation */
	if( last_char != 27 /* ESC */ ) {
		int confirmed = confirmSelection(win);
		if( confirmed == 1 ) {
			strncpy(ip, new_ip, sizeof(ip) - 1);
		}
	}

	/* Clear the bottom two rows */
	for( x = COLS - 2; x > 0; x-- ) {
		mvwaddch(win, LINES - 3, x, ' ');
	}

	/* Reset cursor state and refresh window */
	curs_set(0);
	wrefresh(win);
}

/**Updates the port information with user prompt
 * @param win The window which prompts the user for input
 */
void updatePort(WINDOW* &win) {
	/* Show the cursor */
	curs_set(1);
	int last_char;
	char new_port[5] = "";
	int index = 0;

	/* Move the cursor to the proper screen position */
	int y = LINES - 3;
	int x = 1;
	wmove(win, y, x);

	/* Write a prompt to the user */
	mvwaddstr(win, y, x, "New Port (<ESC> to cancel): ");

	/* User types in 4 digits */
	while( index < 4 && (last_char = wgetch(win)) != 27 /* ESC */ ) {
		switch(last_char) {
			case 48:
			case 49:
			case 50:
			case 51:
			case 52:
			case 53:
			case 54:
			case 55:
			case 56:
			case 57: /* digits 0-9 */
				waddch(win, last_char);
				new_port[index++] = last_char;
				break;
		}
		wrefresh(win);
	}

	/* If proper digits entered, ask for confirmation */
	if( last_char != 27 /* ESC */ ) {
		int confirmed = confirmSelection(win);
		if( confirmed == 1 ) {
			strncpy(port, new_port, sizeof(port) - 1);
		}
	}

	/* Clear the bottom two rows */
	for( x = COLS - 2; x > 0; x-- ) {
		mvwaddch(win, LINES - 3, x, ' ');
	}

	/* Reset cursor state and refresh window */
	curs_set(0);
	wrefresh(win);
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
