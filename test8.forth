# Macros

## Creates a definition (str -- )
: redefine-note 
          "word" pop-store

          ": `word" ,
          "`word notes-db @ sqlite3-last-id" ,
          "link-note" ,
;


# The word ',' takes a string and does a replacement for any
# words that are prefaced by a '`'. It then runs this against
# the string

# This...
"3 4 +" ,

#...is the same as this
3 4 +

# How would I comma execute this:

"Hello" print

# How about this
"\"Hello\" print"

# This could work:
"'Hello'"

# The ',' word could convert "'Hello'" to "Hello"



# Usage
"N" redefine-note


# This creates the following definition:
#
#    : N    N
#           notes-db @ sqlite3-last-id
#           link-note
#    ;


[ "N" "S" "E" ] "redefine-note" map
