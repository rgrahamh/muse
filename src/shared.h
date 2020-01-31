#ifndef SHARED_H
#define SHARED_H
#include <stdlib.h>
#include <string.h>

#define ARR_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

#define QUERY_SEG_SIZE 4096

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
	ORDGNR = 6, /**< Denotes that the client wants to order by the genre */
	ORDYR = 2 /**< Denotes that the client wants to order by the year (SHOULD ONLY BE USED IN ALBUM) */
};

#define REQ_TYPE_MASK 0xf8

/** An enum of the request types the client can make */
enum REQ_TYPE{
	TERMCON = 0, /**< Terminates the connection */
	REQSNG = 8, /**< Requests a song */
	QWRYSNG = 16, /**< Sends a query for all songs */
	QWRYSNGINFO = 24, /**< Sends a query for a song's information */
	QWRYSNGBRST = 32, /**< Sends a query for a burst of songs */
	QWRYALBM = 40, /**< Sends a query for all albums */
	QWRYALBMSNG = 48, /**< Sends a query for all songs in an album */
	QWRYALBMBRST = 56, /**< Sends a query for a burst of albums */
	QWRYART = 64, /**< Sends a query for all artists */
	QWRYARTALBM = 72, /**< Sends a query for all albums associated with an an artist */
	QWRYARTBRST = 80, /**< Sends a query for a burst of artists */
	QWRYGNR = 88, /**< Sends a query for all genres */
	QWRYGNRSNG = 96, /**< Sends a query for all songs in a genre */
	QWRYGNRBRST = 104, /**< Sends a query for a burst of genres */
	QWRYGNRSNGBRST = 112 /**< Sends a query for all songs in a genre */
};

char* escapeApostrophe(char* str);
void dummy();
#endif
