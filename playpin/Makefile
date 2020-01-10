C=g++
CFLAGS=-Wall -lmenu -lncurses -g

muse:
	$(C) ncurses-text.cpp -o test $(CFLAGS)

menu: menu-test.cpp
	$(C) menu-test.cpp -o menu $(CFLAGS)

menub: menu-with-borders.cpp
	$(C) menu-with-borders.cpp -o menub $(CFLAGS)

characters: character_demo.cpp
	$(C) character_demo.cpp -o characters $(CFLAGS)