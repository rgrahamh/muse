#include <curses.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>

int printASCII(FILE* file, int y, int x);
int getASCIILength(FILE* file);
int getASCIIHeight(FILE* file);

int main(int argc, char** argv){
  //initscr();
  newterm(NULL, stderr, stdin);
  keypad(stdscr, TRUE);
  nonl();
  cbreak();
  setlocale(LC_ALL, "");

  FILE*  file = fopen("ASCII.txt", "r");
  if(file != NULL){
    printASCII(file, 0, 33);
    //attron(A_ITALIC);
    //addch('A');
    //attroff(A_ITALIC);
  }
  getch();

  endwin();
  return 0;
}

int printASCII(FILE* file, int y = 0, int x = 0){
  int old_y, old_x;
  getyx(curscr, old_y, old_x);
  move(y, x);

  int line_num = 0;
  char c;
  while((c = getc(file)) != EOF){
    addch(c);
    if(c == '\n'){
      move(y + ++line_num, x);
    }
  }
  rewind(file);

  //Returns to original yx coordinate
  move(old_y, old_x);
  return 0;
}

int getASCIILength(FILE* file){
  int i = 0;
  char c = getc(file);
  for(; c != '\n' && c != EOF; i++){
    c = getc(file);
  }
  return i;
}

int getASCIIHeight(FILE* file){
  int i = 0;
  for(char c; (c = getc(file)) != EOF;){
    if(c == '\n'){
      i++;
    }
  }
  return i;
}
