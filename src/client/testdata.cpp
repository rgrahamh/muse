#include "testdata.h"

/**
 * @brief getTestSongs Allocates some test data
 * @return songinfolist* A pointer to the linked-list of newly created data
 */
struct songinfolst* getTestSongs() {
    // declarations
    struct songinfolst* song1 = (struct songinfolst*) calloc(1, sizeof(struct songinfolst));
    struct songinfolst* song2 = (struct songinfolst*) calloc(1, sizeof(struct songinfolst));
    struct songinfolst* song3 = (struct songinfolst*) calloc(1, sizeof(struct songinfolst));
    struct songinfolst* song4 = (struct songinfolst*) calloc(1, sizeof(struct songinfolst));

    // string allocations
    char* song1title = (char*) calloc(100, sizeof(char));
    char* song1artist = (char*) calloc(100, sizeof(char));
    char* song1album = (char*) calloc(100, sizeof(char));
    char* song1genre = (char*) calloc(100, sizeof(char));

    char* song2title = (char*) calloc(100, sizeof(char));
    char* song2artist = (char*) calloc(100, sizeof(char));
    char* song2album = (char*) calloc(100, sizeof(char));
    char* song2genre = (char*) calloc(100, sizeof(char));

    char* song3title = (char*) calloc(100, sizeof(char));
    char* song3artist = (char*) calloc(100, sizeof(char));
    char* song3album = (char*) calloc(100, sizeof(char));
    char* song3genre = (char*) calloc(100, sizeof(char));

    char* song4title = (char*) calloc(100, sizeof(char));
    char* song4artist = (char*) calloc(100, sizeof(char));
    char* song4album = (char*) calloc(100, sizeof(char));
    char* song4genre = (char*) calloc(100, sizeof(char));

    // string assignment
    strcpy(song1title, "Splishy Splashy Fun");
    strcpy(song1artist, "Cats in the Tub");
    strcpy(song1album, "Kitty Pool");
    strcpy(song1genre, "Acoustic");

    strcpy(song2title, "I Hate Being Wet");
    strcpy(song2artist, "Cats in the Tub");
    strcpy(song2album, "Kitty Pool");
    strcpy(song2genre, "Acoustic");

    strcpy(song3title, "Keyboards are Better Than Bubbles");
    strcpy(song3artist, "Cats in the Tub");
    strcpy(song3album, "Kitty Pool");
    strcpy(song3genre, "Acoustic");

    strcpy(song4title, "Hackin' up a Hairball");
    strcpy(song4artist, "Cats in the Tub");
    strcpy(song4album, "Kitty Pool");
    strcpy(song4genre, "Acoustic");

    // song assignment
    song1->id = 0;
    song1->next = song2;
    song1->title = song1title;
    song1->artist = song1artist;
    song1->album = song1album;
    song1->genre = song1genre;
    song1->year = 2020;
    song1->track_num = 1;

    song2->id = 1;
    song2->next = song3;
    song2->title = song2title;
    song2->artist = song2artist;
    song2->album = song2album;
    song2->genre = song2genre;
    song2->year = 2020;
    song2->track_num = 2;

    song3->id = 2;
    song3->next = song4;
    song3->title = song3title;
    song3->artist = song3artist;
    song3->album = song3album;
    song3->genre = song3genre;
    song3->year = 2020;
    song3->track_num = 3;

    song4->id = 3;
    song4->next = NULL;
    song4->title = song4title;
    song4->artist = song4artist;
    song4->album = song4album;
    song4->genre = song4genre;
    song4->year = 2020;
    song4->track_num = 4;

    // return linked list
    return song1;
}

/**
 * @brief getTestArtists Allocates some test data
 * @return artistinfolist* A pointer to the linked-list of newly created data
 */
struct artistinfolst* getTestArtists() {
    // declarations
    struct artistinfolst* artist1 = (struct artistinfolst*) calloc(1, sizeof(struct artistinfolst));
    struct artistinfolst* artist2 = (struct artistinfolst*) calloc(1, sizeof(struct artistinfolst));
    struct artistinfolst* artist3 = (struct artistinfolst*) calloc(1, sizeof(struct artistinfolst));
    struct artistinfolst* artist4 = (struct artistinfolst*) calloc(1, sizeof(struct artistinfolst));

    // string allocations
    char* artist1name = (char*) calloc(100, sizeof(char));
    char* artist2name = (char*) calloc(100, sizeof(char));
    char* artist3name = (char*) calloc(100, sizeof(char));
    char* artist4name = (char*) calloc(100, sizeof(char));

    // string assignment
    strcpy(artist1name, "Cats in the Tub");
    strcpy(artist2name, "Dogs on the Lawn");
    strcpy(artist3name, "Birds in a Cave");
    strcpy(artist4name, "Ants on a Log");

    // artist assignment
    artist1->id = 0;
    artist1->name = artist1name;
    artist1->next = artist2;

    artist2->id = 1;
    artist2->name = artist2name;
    artist2->next = artist3;

    artist3->id = 2;
    artist3->name = artist3name;
    artist3->next = artist4;

    artist4->id = 3;
    artist4->name = artist4name;
    artist4->next = NULL;

    // return linked list
    return artist1;
}

/**
 * @brief getTestAlbums Allocates some test data
 * @return albuminfolist* A pointer to the linked-list of newly created data
 */
struct albuminfolst* getTestAlbums() {
    // declarations
    struct albuminfolst* album1 = (struct albuminfolst*) calloc(1, sizeof(struct albuminfolst));
    struct albuminfolst* album2 = (struct albuminfolst*) calloc(1, sizeof(struct albuminfolst));
    struct albuminfolst* album3 = (struct albuminfolst*) calloc(1, sizeof(struct albuminfolst));
    struct albuminfolst* album4 = (struct albuminfolst*) calloc(1, sizeof(struct albuminfolst));

    // string allocations
    char* album1title = (char*) calloc(100, sizeof(char));
    char* album2title = (char*) calloc(100, sizeof(char));
    char* album3title = (char*) calloc(100, sizeof(char));
    char* album4title = (char*) calloc(100, sizeof(char));

    // string assignment
    strcpy(album1title, "Kitty Pool");
    strcpy(album2title, "Sunbathin'");
    strcpy(album3title, "Still Be Battin'");
    strcpy(album4title, "We Go Marching On");

    // album assignment
    album1->id = 0;
    album1->title = album1title;
    album1->year = 2017;
    album1->next = album2;

    album2->id = 1;
    album2->title = album2title;
    album2->year = 2009;
    album2->next = album3;

    album3->id = 2;
    album3->title = album3title;
    album3->year = 2004;
    album3->next = album4;

    album4->id = 3;
    album4->title = album4title;
    album4->year = 2020;
    album4->next = NULL;

    // return linked list
    return album1;
}

/**
 * @brief getTestGenres Allocates some test data
 * @return genreinfolist* A pointer to the linked-list of newly created data
 */
struct genreinfolst* getTestGenres() {
    // declarations
    struct genreinfolst* genre1 = (struct genreinfolst*) calloc(1, sizeof(struct genreinfolst));
    struct genreinfolst* genre2 = (struct genreinfolst*) calloc(1, sizeof(struct genreinfolst));
    struct genreinfolst* genre3 = (struct genreinfolst*) calloc(1, sizeof(struct genreinfolst));
    struct genreinfolst* genre4 = (struct genreinfolst*) calloc(1, sizeof(struct genreinfolst));

    // string allocations
    char* genre1name = (char*) calloc(100, sizeof(char));
    char* genre2name = (char*) calloc(100, sizeof(char));
    char* genre3name = (char*) calloc(100, sizeof(char));
    char* genre4name = (char*) calloc(100, sizeof(char));

    // string assignment
    strcpy(genre1name, "Alternative");
    strcpy(genre2name, "Pop / Indie");
    strcpy(genre3name, "Instrumental / Metal");
    strcpy(genre4name, "Smooth Polka");

    // genre assignment
    genre1->genre = genre1name;
    genre1->next = genre2;


    genre2->genre = genre2name;
    genre2->next = genre3;

    genre3->genre = genre3name;
    genre3->next = genre4;

    genre4->genre = genre4name;
    genre4->next = NULL;

    // return linked list
    return genre1;
}
