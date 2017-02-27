# Map

## (num -- num)
: double   2 * ; 

[ 1 2 3 4 ] "double" map

# We want this:
[  1  double
   2  double
   3  double
   4  double
]

# However, the elements of the sequence may not be representable as strings


# This could work:
[  seq 0 nth  double
   seq 1 nth  double
   seq 2 nth  double
   seq 3 nth  double
]


# Maybe a loop?
[
    0 dup len for
        dup i nth double swap
    pop
]

"double"
seq

# Here's what it would look like:

# (seq word -- seq)
: map
    "pop
     [
        0 dup len 'i' for
            dup i nth `0 swap
        pop
     ]" ,
;





Here's the expansion of a couple of steps

seq

val0
seq

seq
val0

seq
seq
val0

val1
seq
val0

seq
val1
val0

# Then maybe we could define a macro for this

# The for loop needs work

# (seq word -- seq)
: map
    "pop
     [
        '`0 swap' foreach
     ]" ,
     pop
;

[ 1 2 ] "double" map
 
# Here's the expansion
pop  # (seq -- )
[
"double" foreach
]
pop

# foreach
(seq word -- n1 n2 n3 ... nk)
0 nth word swap  #  (val0 seq)
1 nth word swap  #  (val0 val1 seq)
2 nth word swap  #  (val0 val1 val2 seq)

"dup" foreach
