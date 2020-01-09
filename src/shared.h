#ifndef SHARED_HPP
#define SHARED_HPP
#include <curses.h>
#include <menu.h>
#include <stdlib.h>

#define ARR_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

#define ORD_DIR_MASK 0x01

/** An enum of if the client wants results in an ascending or descending order */
enum ORD_DIR{
	ASC = 0, /**< Denotes that the client wants the results in an ascending order */
	DESC = 1 /**< Denotes that the client wants things in a descending order */
};

#define ORD_BY_MASK 0x06

/** An enum of the ordering options the client can request*/
enum ORD_BY{
	ORDSNG = 0, /**< Denotes that the client wants to order by song title */
	ORDALBM = 2, /**< Denotes that the client wants to order by album title */
	ORDART = 4, /**< Denotes that the client wants to order by the artist name */
	ORDGNR = 6 /**< Denotes that the client wants to order by the genre */
};

#define REQ_TYPE_MASK 0xf8

/** An enum of the request types the client can make */
enum REQ_TYPE{
	TERMCON = 0, /**< Terminates the connection */
	REQSNG = 8, /**< Requests a song */
	QWRYSNG = 16, /**< Sends a query for all songs */
	QWRYALBM = 24, /**< Sends a query for all albums */
	QWRYALBMSNG = 32, /**< Sends a query for all songs in an album */
	QWRYART = 40, /**< Sends a query for all artists */
	QWRYARTSNG = 48, /**< Sends a query for all songs associated with an an artist */
	QWRYGNR = 56, /**< Sends a query for all genres */
	QWRYGNRSNG = 64 /**< Sends a query for all songs in a genre */
};

void printASCII(FILE* file, int y, int x);
void wprintASCII(WINDOW* win, FILE* file, int y, int x);
int getASCIILength(FILE* file);
int getASCIIHeight(FILE* file);
void dummy();
#endif
