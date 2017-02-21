# App for creating notes

lex-notes

## Opens a database connection and stores it in the variable notes-db
: notes-open  "notes.db" sqlite3-open   notes-db ! ;

## Closes a database connection
: notes-close  notes-db @  sqlite3-close ;

notes-open

today
.i
