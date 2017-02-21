# App for creating notes

lex-notes

## Opens a database connection and stores it in the variable notes-db
: notes-open  "notes.db" sqlite3-open   notes-db ! ;

## Closes a database connection
: notes-close  notes-db @  sqlite3-close ;

: c  chunk-notes print ;
: today  today-notes print ;
: t  time ;

: .q  notes-close .q ;

notes-open

.i
