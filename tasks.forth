## \file tasks.forth
#
# \brief App for managing tasks

## Add tasks lexicon
lex-tasks

## Prints what app this is
: app   "tasks.forth" . ;

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


## Alias the note words
: n    N ;  # Generic note
: s    S ;  # Start of work chunk
: m    M ;  # Middle of work chunk
: e    E ;  # End of work chunk


## Redefine N, S, M, E to link the note to the current task
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


## Prints notes for current chunk of work
: c  chunk-notes print-notes ;

## Prints notes logged today
: today  today-notes print-notes ;

## Prints time since latest 'S' or 'E' note
#  This is useful because it shows the elapsed time for a chunk of work
#  or the amount of time of a break.
: t  time ;

## Prints the current task
: ?   [cur-task] print-tasks ;

## Prints siblings of the current task
: l   siblings print-tasks ;

## Prints ancestors of current task ('w'here)
: w   ancestors print-tasks ;

## Prints children of current task
: sub   children print-tasks ;

## Marks current task as done
: x
    1 set-is-done
    "Marking task complete" N
;

## Marks current task as not done
: /x
    0 set-is-done
    "Marking task incomplete" N
;

## Lists all incomplete tasks
: todo    all incomplete print-tasks ;

## Lists all incomplete top level tasks
: l1    level-1 incomplete print-tasks ;

## Prints all notes associated with a task
: notes
       task-note_ids
       note_ids-to-notes
       print-notes
;

## Redefines .q to close the databases first
: .q   reset close-db .q ;

# Open the databases
open-db

# Become interactive
.i
