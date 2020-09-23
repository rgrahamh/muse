rm muse.db
echo ".quit" | sqlite3 -init db_setup.sql muse.db
