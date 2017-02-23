## \file notes.forth
#
# \brief App for managing notes

## Add notes lexicon
lex-notes

## Opens a database connection and stores it in the variable notes-db
: notes-open  "notes.db" sqlite3-open   notes-db ! ;

## Closes a database connection
: notes-close  notes-db @  sqlite3-close ;

## Prints notes for current chunk of work
: c  chunk-notes print-notes ;

## Prints notes logged today
: today  today-notes print-notes ;

## Prints time since latest 'S' or 'E' note
#  This is useful because it shows the elapsed time for a chunk of work
#  or the amount of time of a break.
: t  time ;

## Redefines .q to close the databases first
: .q  notes-close .q ;

## Opens database connections
notes-open

# Become interactive
.i
