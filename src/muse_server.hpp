#ifndef MUSE_SERVER_HPP
#define MUSE_SERVER_HPP
#include <curses.h>
#include <menu.h>
#include <stdlib.h>

#include "shared.hpp"

#define ARR_SIZE(arr) (sizeof(arr)/sizeof(arr[0]))

enum page {
  MAIN_PAGE,
  PORT_PAGE,
  LIBRARY_PAGE,
  SERVER_PAGE
};

int curr_page = MAIN_PAGE;

char* label_arr[][] = {{"1.", "2.", "3.", "4.", "5.", "6."},
                       {"1.", "2."},
                       {"1.", "2.", "3."},
                       {"1.", "2."}};

char* option_arr[][] = {{"Start In Background", "Change Port", "Library Location", "Refresh Database", "Start Server", "Exit"},
                       {"Specify Port", "Back"},
                       {"Add Path", "Remove Path", "Back"},
                       {"Kill Server", "Run In Background"}};

void* funct_arr[][] = {{(void*)dummy, (void*)dummy, (void*)dummy, (void*)dummy, (void*)dummy, (void*)dummy},
                       {(void*)dummy, (void*)dummy},
                       {(void*)dummy, (void*)dummy, (void*)dummy},
                       {(void*)dummy, (void*)dummy}};

#endif
