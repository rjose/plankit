# App for creating tasks

"task" constant app-name
: app   app-name . ;

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

: ?   [cur-task] print ;
: l   siblings print ;
: w   ancestors print ;
: x   1 set-is-done ;
: /x  0 set-is-done ;
: .q   reset close-db .q ;


open-db

.i
