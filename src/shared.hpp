#ifndef SHARED_HPP
#define SHARED_HPP
#include <curses.h>
#include <menu.h>
#include <stdlib.h>

void printASCII(FILE* file, int y, int x);
void wprintASCII(WINDOW* win, FILE* file, int y, int x);
int getASCIILength(FILE* file);
int getASCIIHeight(FILE* file);
void dummy();
#endif
