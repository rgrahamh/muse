#include "shared.hpp"

/**Prints ASCII art
 * @param file The file containing the ASCII art
 * @param y The y coordinate of the top left character
 * @param x The x coordinate of the top left character
 */
void printASCII(FILE* file, int y = 0, int x = 0){
  //Get the x and y coordinates so it can return the cursor to its original location
  int old_y, old_x;
  getyx(curscr, old_y, old_x);
  move(y, x);

  //Print the ASCII art
  int line_num = 0;
  char c;
  while((c = getc(file)) != EOF){
    addch(c);

    //Adjusting for the x offset
    if(c == '\n'){
      move(y + ++line_num, x);
    }
  }
  rewind(file);

  //Returns to original yx coordinate
  move(old_y, old_x);
}


/**Prints ASCII art in a specified window
 * @param win The window that you want to print in
 * @param file The file containing the ASCII art
 * @param y The y coordinate of the top left character
 * @param x The x coordinate of the top left character
 */
void wprintASCII(WINDOW* win, FILE* file, int y = 0, int x = 0){
  //Get the x and y coordinates so it can return the cursor to its original location
  int old_y, old_x;
  getyx(curscr, old_y, old_x);
  wmove(win, y, x);

  //Print the ASCII art
  int line_num = 0;
  char c;
  while((c = getc(file)) != EOF){
    waddch(win, c);

    //Adjusting for the x offset
    if(c == '\n'){
      wmove(win, y + ++line_num, x);
    }
  }
  rewind(file);

  //Returns to original yx coordinate
  wmove(win, old_y, old_x);
}

/**Gets the length of the ASCII art (assuming the art is a block)
 * @param file The file containing the ASCII art
 * @return The length of the ASCII art
 */
int getASCIILength(FILE* file){
  int i = 0;
  char c = getc(file);
  for(; c != '\n' && c != EOF; i++){
    c = getc(file);
  }
  return i;
}

/**Gets the height of the ASCII art (assuming the art is a block)
 * @param file The file containing the ASCII art
 * @return The height of the ASCII art
 */
int getASCIIHeight(FILE* file){
  int i = 0;
  for(char c; (c = getc(file)) != EOF;){
    if(c == '\n'){
      i++;
    }
  }
  return i;
}
