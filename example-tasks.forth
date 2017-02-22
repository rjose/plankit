# 'print' should always take a sequence of Tasks

"task" constant app-name
: app   app-name . ;

: ? [cur-task] print ;

: l siblings print ;

: w ancestors print ;

10 g  # make Task 10 the cur-task
u   # Goes up to the parent task
d   # Goes to first child task, if exits

0 set-is-complete
x  # Mark cur task as complete
/x  # Mark cur task as incomplete


all incomplete print   # Prints all incomplete tasks

siblings incomplete print  # Prints all incomplete siblings of cur task

# -------------------------------
"Subtask" ++

# Add ability to log a note against a task
"This is an interesting problem" N
"This is an interesting problem" S
"This is an interesting problem" M
"This is an interesting problem" E


# -------------------------------

# Dependencies
10 12 ->   # Task 10 depends on Task 12. This adds a note.
10 12 /->  # Task 10 does not depend on Task 12. This adds a note.


critical-path first seq print  # Prints first item in the critical path


10 g cur blockers print   # Prints all blockers for Task 10



# External dependencies

# Task 10 depends on JIRA with ID of 12345
10
12345 JIRA
>

# Task 10 depends on LMS-1020 in Ratchet
10
LMS-1020 ratchet
>

10 g cur status  # Gets the status of Task 10

12345 JIRA status  # Gets status of JIRA ticket
LMS-1020 ratchet status   # Gets status of Ratchet item
