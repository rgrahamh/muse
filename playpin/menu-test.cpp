#include <stdlib.h>
#include <curses.h>
#include <string.h>
#include <menu.h>
#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define CTRLD 	4
char *choices[] = {
                        "Choice 1",
                        "Choice 2",
                        "Choice 3",
                        "Choice 4",
                        "Exit",
                  };
void endFunct();
void dummy();
int main()
{	ITEM **my_items;
	int c;
	MENU *my_menu;
	int n_choices, i;
	ITEM *cur_item;	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);	n_choices = ARRAY_SIZE(choices);
	my_items = (ITEM **)calloc(n_choices + 1, sizeof(ITEM *));
  for(i = 0; i < n_choices; ++i){
    char* str = (char*)malloc(50);
    memset(str, 0, 50);
    sprintf(str, "%d:", i+1);
    my_items[i] = new_item(str, choices[i]);
    if(i == n_choices - 1){
      set_item_userptr(my_items[i], (void*)endFunct);
    }
    else{
      set_item_userptr(my_items[i], (void*)dummy);
    }
  }
	my_items[n_choices] = (ITEM *)NULL;	my_menu = new_menu((ITEM **)my_items);
	mvprintw(LINES - 2, 0, "F1 to Exit");
	post_menu(my_menu);
	refresh();
  while((c = getch()) != KEY_F(1))
	{   switch(c)
	    {	case KEY_DOWN:
		        menu_driver(my_menu, REQ_DOWN_ITEM);
				break;
			case KEY_UP:
				menu_driver(my_menu, REQ_UP_ITEM);
				break;
      case 10:
        ITEM* item = current_item(my_menu);
        void (*p)(char *);

        p = (void (*)(char*))item_userptr(item);
        p((char*) item_name(item));
        pos_menu_cursor(my_menu);
        break;
		}
	}		free_item(my_items[0]);
	free_item(my_items[1]);
	free_menu(my_menu);
	endwin();
}

void endFunct(){
  endwin();
  exit(0);
}
void dummy(){
  return;
}
