CREATE TABLE artist(
	artist_id INTEGER PRIMARY KEY AUTOINCREMENT,
	name VARCHAR(256) NOT NULL
);

CREATE TABLE album(
	album_id INTEGER PRIMARY KEY AUTOINCREMENT,
	album_name VARCHAR(256) NOT NULL,
	artist_id INTEGER,
	year INTEGER,
	FOREIGN KEY (artist_id) REFERENCES artist(artist_id)
);

CREATE TABLE song(
	song_id INTEGER PRIMARY KEY AUTOINCREMENT,
	song_name VARCHAR(256) NOT NULL,
	album_id INTEGER,
	artist_id INTEGER,
	track_num INTEGER,
	genre INTEGER,
	FOREIGN KEY (album_id) REFERENCES album(album_id),
	FOREIGN KEY (artist_id) REFERENCES artist(artist_id)
);

