# musicIndexer

A program which indexes music with their tag, and store it into a database.

Using libraries :
	-**SQLite** (database)
	-**TagLib** (library for reading metadata of audio format)

If there's a problem while getting the BPM of .mp3 (or other) files, check if your tabglib library was compiled with the --enable-avcodec flag (++ avcodec libraries) -> Got a problem with my Arch linux
