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

: ?   [cur-task] print-tasks ;
: l   siblings print-tasks ;
: w   ancestors print-tasks ;
: sub   children print-tasks ;
: x   1 set-is-done ;
: /x  0 set-is-done ;
: todo    all incomplete print-tasks ;
: l1    level-1 incomplete print-tasks ;

: .q   reset close-db .q ;

: N    N
       notes-db @ sqlite3-last-id
       link-note
;

: S    S
       notes-db @ sqlite3-last-id
       link-note
;

: M    M
       notes-db @ sqlite3-last-id
       link-note
;

: E    E
       notes-db @ sqlite3-last-id
       link-note
;

open-db

.i
