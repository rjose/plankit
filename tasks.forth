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

: c  chunk-notes print-notes ;
: today  today-notes print-notes ;
: t  time ;

: ?   [cur-task] print-tasks ;
: l   siblings print-tasks ;
: w   ancestors print-tasks ;
: sub   children print-tasks ;
: x   1 set-is-done ;
: /x  0 set-is-done ;
: todo    all incomplete print-tasks ;
: l1    level-1 incomplete print-tasks ;

: notes
       task-note_ids
       note_ids-to-notes
       print-notes
;

: .q   reset close-db .q ;

: n    N ;
: s    S ;
: m    M ;
: e    E ;

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
