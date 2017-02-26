## \file tasks.forth
#
# \brief App for managing tasks

## Prints task name
: app   "tasks.forth" . ;

# ======================================
# Add notes lexicon, aliasing anything that collides
# ======================================
lex-notes

: n    N ;  # Generic note
: s    S ;  # Start of work chunk
: mid    M ;  # Middle of work chunk
: e    E ;  # End of work chunk

## Add tasks lexicon
lex-tasks


# ======================================
# Specifies db files for this app
# ======================================

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


# ======================================
# Integrate with notes
# ======================================

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


# ======================================
# Printing notes
# ======================================

## Prints notes for current chunk of work
: c  chunk-notes print-notes ;

## Prints notes logged today
: today  today-notes print-notes ;

## Prints time since latest 'S' or 'E' note
#  This is useful because it shows the elapsed time for a chunk of work
#  or the amount of time of a break.
: t  time ;


## Prints all notes associated with a task
: notes
       task-note_ids
       note_ids-to-notes
       print-notes
;



# ======================================
# Printing tasks
# ======================================
## Prints incomplete siblings of the current task
: l     siblings incomplete
        "task_value" descending
        print-tasks ;

## Prints all siblings of the current task
: la     siblings
        "task_value" descending
        print-tasks ;

## Prints hierarchy of a task
#  (task-id -- )
: ph    hierarchy print-task-hierarchy ;


## Prints hierarchy of a task (incomplete tasks)
#  (task-id -- )
: pih    hierarchy incomplete print-task-hierarchy ;


## Prints ancestors of current task ('w'here)
: w   ancestors print-task-hierarchy ;

## Prints children of current task
: sub   children
        "task_value" descending
        print-tasks ;

## (str -- tasks)
: /     search
        "task_value" descending
        print-tasks ;

## Lists all incomplete tasks
: todo    all incomplete
          "task_value" descending
          print-task-hierarchy
          ;

: to      todo ;

## Lists all incomplete top level tasks
: l1    level-1 incomplete
        "task_value" descending
        print-tasks ;

## Prints all tasks as a tree
: ap    all "task_value" descending print-task-hierarchy ;


# ======================================
# Updating tasks
# ======================================

## Helper function to pull fresh cur-task data from db after a change
: refresh-cur-task  *cur-task @ task_id g ;

## Marks current task as done
: x
    *cur-task @ task_id   # (task id)
    1 is-done!            # (task)
    pop                   # ()
    refresh-cur-task
    "Marking task complete" N
;

## Marks current task as not done
: /x
    *cur-task @ task_id   # (task id)
    0 is-done!            # (task)
    pop                   # ()
    refresh-cur-task
    "Marking task incomplete" N
;


# ======================================
# Misc
# ======================================

## Redefine 'm'ove task so the cur-task gets updated
: m    m refresh-cur-task ;

## Redefines .q to close the databases first
: .q   reset close-db .q ;


## Goes to the task with the most recent note
: active    last-active-id g ;


# ======================================
# STARTUP
# ======================================

# Open the databases
open-db

# Go to last active task
active

# Become interactive
.i
