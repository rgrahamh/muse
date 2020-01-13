#include "muse_server_tui.hpp"

int main(int argc, char** argv) {
	int last_char;
	MENU* menu;
	WINDOW *win, *menu_win;

	/* Initialize the state */
	readStateFromFile();

	/* Scan new mp3s into database */
	scan(&lib_paths.at(0), lib_paths.size());

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
	} else if( callback == &updatePort ) {
		updatePort(win);
	} else if( callback == &addLibPath ) {
		addLibPath(win);
	} else if( callback == &removeLibPath ) {
		removeLibPath(win);
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
	unsigned int length_avail = COLS - (COLS / 2) - 5;
	unsigned int height_avail = LINES - ascii_height - 5;

	/* Static labels */
	char* port_label = "Port: ";
	char* library_label = "Libraries: ";
	char* exit_label = "Press F1 to exit.";

	/* Network information */
	mvwaddstr(win, y, x, port_label);
	mvwaddstr(win, y, x+strlen(port_label), port);

	/* Library information */
	mvwaddstr(win, y+1, x, library_label);
	if( height_avail - 1 >= lib_paths.size() ) {
		for( unsigned int i = 0; i < (  lib_paths.size()); i++ ) {
			if( strlen(lib_paths.at(i)) < length_avail ) {
				mvwaddstr(win, y+2+i, x+2, lib_paths.at(i));
			} else {
				for( unsigned int idx = 0; idx < length_avail; idx++ ) {
					mvwaddch(win, y+2+i, x+2+idx, lib_paths.at(i)[(strlen(lib_paths.at(i)) - length_avail) + idx]);
				}
				mvwaddstr(win, y+2+i, x+2, "...");
			}
		}
	} else {
		for( unsigned int i = 0; i < height_avail - 2; i++ ) {
			if( strlen(lib_paths.at(i)) < length_avail ) {
				mvwaddstr(win, y+2+i, x+2, lib_paths.at(i));
			} else {
				for( unsigned int idx = 0; idx < length_avail; idx++ ) {
					mvwaddch(win, y+2+i, x+2+idx, lib_paths.at(i)[(strlen(lib_paths.at(i)) - length_avail) + idx]);
				}
				mvwaddstr(win, y+2+i, x+2, "...");
			}
		}

		char more_msg[30];
		snprintf(more_msg, sizeof(more_msg), "... and %d more...", ((int)lib_paths.size() - height_avail + 2));
		mvwaddstr(win, y+height_avail, x+2, more_msg);
	}


	/* Exit label */
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
	/* Write program's state down */
	writeStateToFile();

	/* Cleanup old list of items */
	ITEM** old_items = menu_items(menu);
	int n_old_items = item_count(menu);

	for( int i = 0; i < n_old_items; ++i ) {
		free_item(old_items[i]);
	}

	/* Unpost and free all the memory taken up */
	unpost_menu(menu);
	free_menu(menu);



	/* Free all of the lib_paths memory */
	for( unsigned int i = 0; i < lib_paths.size(); i++ ) {
		free(lib_paths.at(i));
	}

	/* Show the cursor again before exiting */
	curs_set(1);

	endwin();
	exit(0);
}

void writeStateToFile() {
	std::ofstream state_file;
	state_file.open("muse.state");
	
	if( state_file.good() ) {
		/* Write the port information */
		state_file << "p:" << port << std::endl;


		/* Write library information */
		for( unsigned int i = 0; i < lib_paths.size(); i++ ) {
			state_file << "l:" << lib_paths.at(i) << std::endl;
		}
	}
	
	state_file.close();
}

void readStateFromFile() {
	std::ifstream  state_file;
	state_file.open("muse.state");

	if( state_file.good() ) {
		/* Parse based on simple language */
		std::string line;
		while( std::getline(state_file, line) ) {
			if( line[0] == 'p' ) {
				strncpy( port, &line[2], sizeof(port) - 1 );
			} else if( line[0] == 'l' ) {
				std::string lib = line.substr(2);
				char* new_lib_path = (char*) calloc(PATH_MAX, sizeof(char));

				strncpy(new_lib_path, lib.c_str(), lib.length());

				lib_paths.push_back(new_lib_path);
			}
		}
	}

	state_file.close();
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
	// //If the child process
	// if(!(muse_pid = fork())) {
	// 	//If serve returns anything but zero
	// 	if(/* serve(port) */ TRUE) {
	// 		//Spit them out to the main page if something goes wrong in MUSE itself
	// 		// curr_page = MAIN_PAGE;
	// 	}
	// }
	// if(background) {
	// 	//backgroundProc
	// 	backgroundProc();
	// }
	// else{
	// 	// curr_page = SERVER_PAGE;
	// }
	// refresh();
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
		if( last_char >= 48 && last_char <= 57 /* 0-9 */ ) {
			waddch(win, last_char);
			new_port[index++] = last_char;
		} else if( (last_char == KEY_BACKSPACE || last_char == KEY_DC || last_char == 127) && (index > 0) ) {
			int cur_y, cur_x;
			getyx(win, cur_y, cur_x);
			mvwaddch(win, cur_y, cur_x-1, ' ');
			wmove(win, cur_y, cur_x-1);
			new_port[--index] = '\0';
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

	/* Clear the entry row */
	for( x = COLS - 2; x > 0; x-- ) {
		mvwaddch(win, LINES - 3, x, ' ');
	}

	/* Reset cursor state and refresh window */
	curs_set(0);
	wrefresh(win);
}

void addLibPath(WINDOW* win) {
	/* Show the cursor */
	curs_set(1);
	int last_char = 0;
	char* new_lib_path = (char*) calloc(PATH_MAX, sizeof(char));
	int index = 0;

	/* Move the cursor to the proper screen position */
	int y = LINES - 3;
	int x = 1;
	wmove(win, y, x);

	/* Write a prompt to the user */
	mvwaddstr(win, y, x, "New Library Path (<ESC> to cancel): ");
	wmove(win, y+1, x);

	/* Receive input from user */
	while( index < PATH_MAX-1 ) {
		last_char = wgetch(win);

		if( last_char == 27 /* ESC */ || last_char == 10 /* ENTER */ ) {
			break; // exit the loop immediately
		} else if( last_char == KEY_BACKSPACE || last_char == KEY_DC || last_char == 127 ) {
			// delete a character from the screen
			if( x+index <= (COLS / 2) && index > 0 ) { // if we don't have any text off-screen
				int cur_y, cur_x;
				getyx(win, cur_y, cur_x);
				mvwaddch(win, cur_y, cur_x-1, ' ');
				wmove(win, cur_y, cur_x-1);
				new_lib_path[--index] = '\0';
			} else if( index > 0 ) { // text has wrapped off-screen, so I need to wrap it back
				int length_avail = (COLS / 2) - x;
				for( int i = 0; i < length_avail; i++ ) {
					mvwaddch(win, y+1, x+i, new_lib_path[(index - length_avail - 1) + i]);
				}
				new_lib_path[--index] = '\0';
			}
		} else if( last_char >= 32 && last_char <= 176 /* printable chars */ ) { // write a new character to the screen
			new_lib_path[index] = last_char;
			if( x+index < (COLS / 2) ) { // write information until it has filled it's space
				mvwaddch(win, y+1, x+index, last_char);
			} else { // "wrap" text off screen
				int length_avail = (COLS / 2) - x;
				for( int i = 0; i < length_avail; i++ ) {
					mvwaddch(win, y+1, x+i, new_lib_path[(index - length_avail + 1) + i]);
				}
			}
			index++;
		}
	}

	new_lib_path[index] = '\0';

	if( last_char == 27 ) { /* If the last character was the ESC key, just exit without committing anything */
	    free(new_lib_path);
    } else if( (last_char == 10 || index >= PATH_MAX-1) && strlen(new_lib_path) > 0 ) { /* If the last character was the ENTER key, add it to the list of library paths */
		lib_paths.push_back(new_lib_path);
	}

	/* Clear the bottom two rows */
	for( x = COLS - 2; x > 0; x-- ) {
		mvwaddch(win, LINES - 3, x, ' ');
		mvwaddch(win, LINES - 2, x, ' ');
	}

	/* Reset cursor state and refresh window */
	curs_set(0);
	wrefresh(win);
}

void removeLibPath(WINDOW* win) {
	int last_char;
	int origin_y = ascii_height+4;
	int origin_x = (COLS / 2)+4;

	unsigned int length_avail = COLS - (COLS / 2) - 5;
	unsigned int height_avail = LINES - ascii_height - 5;
	unsigned int scrollfield_top = 0;
	unsigned int scrollfield_bot = height_avail - 2;

	unsigned int highlighted = 0;
	char* elipsis = "...";

	/* Print some help information */
	if( lib_paths.size() > 0 ) {
		char* help = "Press <ENTER> to select, <ESC> to cancel.";
		mvwaddstr(win, LINES - 2, 1, help);
	} else {
		char* help = "There are no items to remove.";
		mvwaddstr(win, LINES - 2, 1, help);
		wrefresh(win);
		while( true ) {
			last_char = wgetch(win);
			if( last_char == 10 || last_char == 27 ) {
				break;
			}
		}
		mvwaddstr(win, LINES - 2, 1, "                             ");
		return;
	}

	/* Erase the subwindow so it can be repainted */
	for( int y = origin_y; y < LINES - 2; y++ ) {
		for( int x = origin_x - 2; x < COLS - 1; x++ ) {
			mvwaddch(win, y, x, ' ');
		}
	}

	/* Paint the selected option */
	if( height_avail - 1 >= lib_paths.size() ) {
		for( unsigned int i = 0; i < lib_paths.size(); i++ ) {
			if( strlen(lib_paths.at(i)) < length_avail ) {
				for( unsigned int idx = 0; idx < strlen(lib_paths.at(i)); idx++ ) {
					mvwaddch(win, origin_y+i, origin_x+idx, lib_paths.at(i)[idx] | (highlighted == i ? A_STANDOUT : 0));
				}
			} else {
				for( unsigned int idx = 0; idx < length_avail; idx++ ) {
					mvwaddch(win, origin_y+i, origin_x+idx, lib_paths.at(i)[(strlen(lib_paths.at(i)) - length_avail) + idx] | (highlighted == i ? A_STANDOUT : 0));
				}
				for( unsigned int idx = 0; idx < strlen(elipsis); idx++ ) {
					mvwaddch(win, origin_y+i, origin_x+idx, elipsis[idx] | (highlighted == i ? A_STANDOUT : 0));
				}
			}
		}
	} else {
		for( unsigned int i = scrollfield_top; i <= scrollfield_bot; i++ ) {
			if( strlen(lib_paths.at(i)) < length_avail ) {
				for( unsigned int idx = 0; idx < strlen(lib_paths.at(i)); idx++ ) {
					mvwaddch(win, origin_y+i-scrollfield_top, origin_x+idx, lib_paths.at(i)[idx] | (highlighted == i ? A_STANDOUT : 0));
				}
			} else {
				for( unsigned int idx = 0; idx < length_avail; idx++ ) {
					mvwaddch(win, origin_y+i-scrollfield_top, origin_x+idx, lib_paths.at(i)[(strlen(lib_paths.at(i)) - length_avail) + idx] | (highlighted == i ? A_STANDOUT : 0));
				}
				for( unsigned int idx = 0; idx < strlen(elipsis); idx++ ) {
					mvwaddch(win, origin_y+i-scrollfield_top, origin_x+idx, elipsis[idx] | (highlighted == i ? A_STANDOUT : 0));
				}
			}
		}
	}

	/* Print scroll indicators */
	if( scrollfield_top != 0 ) {
		mvwaddch(win, origin_y, origin_x-2, '^');
	}

	if( scrollfield_bot != lib_paths.size()-1) {
		mvwaddch(win, LINES - 3, origin_x-2, 'v');
	}

	wrefresh(win);

	while( true ) {
		last_char = wgetch(win);

		switch(last_char) {
			case KEY_DOWN:
				if( highlighted < lib_paths.size() - 1 ) {
					highlighted++;
				}
				if( highlighted > scrollfield_bot ) {
					scrollfield_top++;
					scrollfield_bot++;
				}
				break;
			case KEY_UP:
				if( highlighted > 0 ) {
					highlighted--;
				}
				if( highlighted < scrollfield_top ) {
					scrollfield_top--;
					scrollfield_bot--;
				}
				break;
			case 10: /* ENTER */
				if(lib_paths.size() > 0) {
					free(lib_paths.at(highlighted));
					lib_paths.erase(lib_paths.begin() + highlighted);
				}
				break;
			case 27: /* ESC */
				break;
		}

		/* Exit the loop */
		if( last_char == 27 || last_char == 10 ) {
			break;
		}

		/* Erase the subwindow so it can be repainted */
		for( int y = origin_y; y < LINES - 2; y++ ) {
			for( int x = origin_x - 2; x < COLS - 1; x++ ) {
				mvwaddch(win, y, x, ' ');
			}
		}

		/* Print scroll indicators */
		if( scrollfield_top != 0 ) {
			mvwaddch(win, origin_y, origin_x-2, '^');
		}

		if( scrollfield_bot != lib_paths.size()-1) {
			mvwaddch(win, LINES - 3, origin_x-2, 'v');
		}

		/* Paint the selected option */
		if( height_avail - 1 >= lib_paths.size() ) {
			for( unsigned int i = 0; i < lib_paths.size(); i++ ) {
				if( strlen(lib_paths.at(i)) < length_avail ) {
					for( unsigned int idx = 0; idx < strlen(lib_paths.at(i)); idx++ ) {
						mvwaddch(win, origin_y+i, origin_x+idx, lib_paths.at(i)[idx] | (highlighted == i ? A_STANDOUT : 0));
					}
				} else {
					for( unsigned int idx = 0; idx < length_avail; idx++ ) {
						mvwaddch(win, origin_y+i, origin_x+idx, lib_paths.at(i)[(strlen(lib_paths.at(i)) - length_avail) + idx] | (highlighted == i ? A_STANDOUT : 0));
					}
					for( unsigned int idx = 0; idx < strlen(elipsis); idx++ ) {
						mvwaddch(win, origin_y+i, origin_x+idx, elipsis[idx] | (highlighted == i ? A_STANDOUT : 0));
					}
				}
			}
		} else {
			for( unsigned int i = scrollfield_top; i <= scrollfield_bot; i++ ) {
				if( strlen(lib_paths.at(i)) < length_avail ) {
					for( unsigned int idx = 0; idx < strlen(lib_paths.at(i)); idx++ ) {
						mvwaddch(win, origin_y+i-scrollfield_top, origin_x+idx, lib_paths.at(i)[idx] | (highlighted == i ? A_STANDOUT : 0));
					}
				} else {
					for( unsigned int idx = 0; idx < length_avail; idx++ ) {
						mvwaddch(win, origin_y+i-scrollfield_top, origin_x+idx, lib_paths.at(i)[(strlen(lib_paths.at(i)) - length_avail) + idx] | (highlighted == i ? A_STANDOUT : 0));
					}
					for( unsigned int idx = 0; idx < strlen(elipsis); idx++ ) {
						mvwaddch(win, origin_y+i-scrollfield_top, origin_x+idx, elipsis[idx] | (highlighted == i ? A_STANDOUT : 0));
					}
				}
			}
		}
		wrefresh(win);
	}

	/* Erase the subwindow so it can be repainted */
	for( int y = origin_y; y < LINES - 2; y++ ) {
		for( int x = origin_x - 2; x < COLS - 1; x++ ) {
			mvwaddch(win, y, x, ' ');
		}
	}

	/* Clear the help row */
	for( int x = COLS - 2; x > 0; x-- ) {
		mvwaddch(win, LINES - 2, x, ' ');
	}

	wrefresh(win);
}

void refreshDatabase() {
	return;
}
