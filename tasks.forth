# App for creating tasks

lex-tasks

## Opens database connections
: open-db
    "notes.db" sqlite3-open   notes-db !
    "tasks.db" sqlite3-open   tasks-db !
;

## Closes database connections
: close-db
    tasks-db @  sqlite3-close
    notes-db @  sqlite3-close
;

: ? [cur-task] print ;

: .q   reset close-db .q ;


open-db

.i
