#include <menu.h>
#include <string.h>
#include <stdlib.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define CTRLD 	4

char *choices[] = {
                        "Choice 1",
                        "Choice 2",
                        "Choice 3",
                        "Choice 4",
                        "Exit",
                        (char *)NULL,
                  };
void print_in_middle(WINDOW *win, int starty, int startx, int width, char *string, chtype color);
int printASCII(WINDOW* win, FILE* file, int y, int x);

int main()
{	ITEM **my_items;
	int c;
	MENU *my_menu;
        WINDOW *my_menu_win;
        int n_choices, i;

	/* Initialize curses */
	initscr();
	start_color();
        cbreak();
        noecho();
	keypad(stdscr, TRUE);
	init_pair(1, COLOR_RED, COLOR_BLACK);
	/* Create items */
        n_choices = ARRAY_SIZE(choices);
        my_items = (ITEM **)calloc(n_choices, sizeof(ITEM *));
        for(i = 0; i < n_choices; ++i)
                my_items[i] = new_item(choices[i], choices[i]);

	/* Crate menu */
	my_menu = new_menu((ITEM **)my_items);

	/* Create the window to be associated with the menu */
        my_menu_win = newwin(17, 80, 4, 4);
        keypad(my_menu_win, TRUE);

	/* Set main window and sub window */
        set_menu_win(my_menu, my_menu_win);
        set_menu_sub(my_menu, derwin(my_menu_win, 6, 38, 11, 1));

	/* Set menu mark to the string " * " */
        set_menu_mark(my_menu, " > ");

	/* Print a border around the main window and print a title */
  FILE* file = fopen("ASCII.txt", "r");
  printASCII(my_menu_win, file, 0, 0);
	//mvwaddch(my_menu_win, 2, 0, ACS_LTEE);
	//mvwhline(my_menu_win, 2, 1, ACS_HLINE, 38);
	//mvwaddch(my_menu_win, 2, 39, ACS_RTEE);
	mvprintw(LINES - 2, 0, "F1 to exit");
	refresh();

	/* Post the menu */
	post_menu(my_menu);
	wrefresh(my_menu_win);

	while((c = wgetch(my_menu_win)) != KEY_F(1))
	{       switch(c)
	        {	case KEY_DOWN:
				menu_driver(my_menu, REQ_DOWN_ITEM);
				break;
			case KEY_UP:
				menu_driver(my_menu, REQ_UP_ITEM);
				break;
		}
                wrefresh(my_menu_win);
	}

	/* Unpost and free all the memory taken up */
        unpost_menu(my_menu);
        free_menu(my_menu);
        for(i = 0; i < n_choices; ++i)
                free_item(my_items[i]);
	endwin();
}

void print_in_middle(WINDOW *win, int starty, int startx, int width, char *string, chtype color)
{	int length, x, y;
	float temp;

	if(win == NULL)
		win = stdscr;
	getyx(win, y, x);
	if(startx != 0)
		x = startx;
	if(starty != 0)
		y = starty;
	if(width == 0)
		width = 80;

	length = strlen(string);
	temp = (width - length)/ 2;
	x = startx + (int)temp;
	wattron(win, color);
	mvwprintw(win, y, x, "%s", string);
	wattroff(win, color);
	refresh();
}

int printASCII(WINDOW* win, FILE* file, int y = 0, int x = 0){
  int old_y, old_x;
  getyx(curscr, old_y, old_x);
  wmove(win, y, x);

  int line_num = 0;
  char c;
  while((c = getc(file)) != EOF){
    waddch(win, c);
    if(c == '\n'){
      wmove(win, y + ++line_num, x);
    }
  }
  rewind(file);

  //Returns to original yx coordinate
  wmove(win, old_y, old_x);
  return 0;
}
