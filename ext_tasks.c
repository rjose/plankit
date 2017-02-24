/** \file ext_tasks.c

\brief Lexicon for tasks

The schema of the tasks.db is:

- CREATE TABLE tasks(is_done INTEGER, id INTEGER PRIMARY KEY, name TEXT);
- CREATE TABLE parent_child(parent INTEGER, child INTEGER);
- CREATE TABLE task_notes(task INTEGER, note INTEGER);


The *cur-task variable refers to the current task. This is an implicit
argument for many of the words in the lexicon. It may be NULL if the
current task is the root task.

\note You shouldn't set *cur-task directly since its memory management is handled
      by the lexicon (via set_cur_task). Also, the only function that should
      free memory involving *cur-task is set_cur_task.

A convention throughout is that if a word pushes a sequence or array onto the
stack, it is the responsibility of the caller to free that memory.

*/

#define MAX_NAME_LEN   256
#define MAX_ID_LEN   16

// -----------------------------------------------------------------------------
/** Represents a note from a database record
*/
// -----------------------------------------------------------------------------
typedef struct {
    gint64 id;     /**< \brief Record ID */
    gint64 parent_id;
    gchar name[MAX_NAME_LEN];
    gboolean is_done;
} Task;


// -----------------------------------------------------------------------------
/** Creates a new Task
*/
// -----------------------------------------------------------------------------
static Task *new_task(gint64 id, gint64 parent_id, const gchar* name, gboolean is_done) {
    Task *result = g_new(Task, 1);

    result->id = id;
    result->parent_id = parent_id;
    g_strlcpy(result->name, name, MAX_NAME_LEN);
    result->is_done = is_done;

    return result;
}


// -----------------------------------------------------------------------------
/** Creates a new copy of a Task.
*/
// -----------------------------------------------------------------------------
static Task *copy_task(Task *src) {
    if (!src) return NULL;

    Task *result = g_new(Task, 1);
    *result = *src;
    return result;
}


// -----------------------------------------------------------------------------
/** Returns a pointer to cur-task

\note This shouldn't be freed by the caller. The memory is freed when a new
      cur-task is set via set_cur_task.
*/
// -----------------------------------------------------------------------------
static Task *get_cur_task() {
    find_and_execute("*cur-task");
    find_and_execute("@");
    Param *param_task = pop_param();
    Task *result = param_task->val_custom;

    free_param(param_task);
    return result;
}


// -----------------------------------------------------------------------------
/** Sets the '*cur-task' variable.

The old version of *cur-task is freed during this call (this is the only place
where the cur-task gets freed).

*/
// -----------------------------------------------------------------------------
static void set_cur_task(Task *task) {
    Task *cur_task = get_cur_task();
    g_free(cur_task);

    Param *param_new = new_custom_param(task, "Task");
    push_param(param_new);
    find_and_execute("*cur-task");
    find_and_execute("!");
}


// -----------------------------------------------------------------------------
/** Returns the ID of the *cur-task
*/
// -----------------------------------------------------------------------------
static gint64 get_cur_task_id() {
    Task *cur_task = get_cur_task();
    gint64 result = 0;
    if (cur_task) {
        result = cur_task->id;
    }
    return result;
}



// -----------------------------------------------------------------------------
/** Gets a database connection from the "tasks-db" variable.
*/
// -----------------------------------------------------------------------------
static sqlite3 *get_db_connection() {
    find_and_execute("tasks-db");
    find_and_execute("@");
    Param *param_connection = pop_param();

    sqlite3 *result = param_connection->val_custom;

    free_param(param_connection);
    return result;
}


// -----------------------------------------------------------------------------
/** Callback used to write note records from an SQL query into a sequence of records.
*/
// -----------------------------------------------------------------------------
static int append_task_cb(gpointer gp_records, int num_cols, char **values, char **cols) {
    GSequence *records = gp_records;
    if (num_cols != 4) {
        handle_error(ERR_GENERIC_ERROR);
        fprintf(stderr, "-----> Unexpected num cols from task query\n");
        return 1;
    }

    gint64 id = g_ascii_strtoll(values[0], NULL, 10);
    gint64 parent_id = g_ascii_strtoll(values[1], NULL, 10);
    const gchar *name_text = values[2];
    gboolean is_done = g_ascii_strtoll(values[3], NULL, 10);

    Task *task_new = new_task(id, parent_id, name_text, is_done);

    g_sequence_append(records, task_new);
    return 0;
}


// -----------------------------------------------------------------------------
/** Returns a GSequence of tasks matching the conditions.

\note The caller is responsible for freeing the returned GSequence.
*/
// -----------------------------------------------------------------------------
static GSequence *select_tasks(const gchar *sql_conditions) {
    sqlite3 *connection = get_db_connection();

    gchar *select = "select id, pc.parent, name, is_done "
                    "from tasks inner join parent_child as pc on pc.child=id ";

    gchar *query = g_strconcat(select, sql_conditions, NULL);


    GSequence *result = g_sequence_new(g_free);

    char *error_message = NULL;
    sqlite3_exec(connection, query, append_task_cb, result, &error_message);

    if (error_message) {
        handle_error(ERR_GENERIC_ERROR);
        fprintf(stderr, "-----> Problem executing 'select_tasks'\n----->%s", error_message);

        g_free(result);
        result = NULL;
    }

    // Cleanup
    g_free(query);

    return result;
}



// -----------------------------------------------------------------------------
/** Adds a task to the tasks-db

*/
// -----------------------------------------------------------------------------
static void add_task(const gchar *name, gint64 parent_id) {
    gchar parent_id_str[MAX_ID_LEN];
    gchar child_id_str[MAX_ID_LEN];

    char* error_message = NULL;
    sqlite3 *connection = get_db_connection();

    // Insert new task
    gchar *sql = g_strconcat("insert into tasks(name, is_done) ",
                             "values(\"", name, "\", 0)", NULL);
    sqlite3_exec(connection, sql, NULL, NULL, &error_message);
    g_free(sql);

    if (error_message) {
        handle_error(ERR_GENERIC_ERROR);
        fprintf(stderr, "-----> Problem inserting task ==> %s\n", error_message);
        goto done;
    }

    // Get ID of task
    gint64 task_id = sqlite3_last_insert_rowid(connection);

    // Insert parent/child record
    snprintf(parent_id_str, MAX_ID_LEN, "%ld", parent_id);
    snprintf(child_id_str, MAX_ID_LEN, "%ld", task_id);
    sql = g_strconcat("insert into parent_child(parent, child) ",
                             "values(", parent_id_str, ", ", child_id_str, ")", NULL);
    sqlite3_exec(connection, sql, NULL, NULL, &error_message);
    g_free(sql);

    if (error_message) {
        handle_error(ERR_GENERIC_ERROR);
        fprintf(stderr, "-----> Problem inserting parent/child ==> %s\n", error_message);
    }

done:
    return;
}


// -----------------------------------------------------------------------------
/** Adds a new task as a sibling of cur-task.

If _cur_task is NULL, this adds a child task of NULL.

*/
// -----------------------------------------------------------------------------
static void EC_add_task(gpointer gp_entry) {
    Task *cur_task = get_cur_task();
    gint64 parent_id = 0;
    if (cur_task) {
        parent_id = cur_task->parent_id;
    }

    // Get the task name and create a new task
    Param *param_task_name = pop_param();
    add_task(param_task_name->val_string, parent_id);

    free_param(param_task_name);
}



// -----------------------------------------------------------------------------
/** Pops a task name and uses it to make a subtask of *cur-task.
*/
// -----------------------------------------------------------------------------
static void EC_add_subtask(gpointer gp_entry) {
    gint64 parent_id = get_cur_task_id();
    Param *param_task_name = pop_param();
    add_task(param_task_name->val_string, parent_id);

    free_param(param_task_name);
}




// -----------------------------------------------------------------------------
/** Sets *cur-task to be the first child of the previous *cur-task.
*/
// -----------------------------------------------------------------------------
static void EC_down(gpointer gp_entry) {
    gint64 parent_id = get_cur_task_id();
    gchar parent_id_str[MAX_ID_LEN];
    snprintf(parent_id_str, MAX_ID_LEN, "%ld", parent_id);

    gchar *sql_condition = g_strconcat("where pc.parent=", parent_id_str, " order by id asc limit 1", NULL);
    GSequence *records = select_tasks(sql_condition);
    g_free(sql_condition);
    if (g_sequence_get_length(records) != 1) {
        goto done;
    }

    Task *task_down = copy_task(g_sequence_get(g_sequence_get_begin_iter(records)));
    set_cur_task(task_down);

done:
    g_sequence_free(records);
}


// -----------------------------------------------------------------------------
/** Sets *cur-task to be the parent of the previous *cur-task
*/
// -----------------------------------------------------------------------------
static void EC_up(gpointer gp_entry) {
    Task *cur_task = get_cur_task();

    if (!cur_task) return;

    Param *param_int = new_int_param(cur_task->parent_id);
    push_param(param_int);
    find_and_execute("g");
}


// -----------------------------------------------------------------------------
/** Sets *cur-task to be the Task with the ID popped from the stack
*/
// -----------------------------------------------------------------------------
static void EC_go(gpointer gp_entry) {
    Param *param_id = pop_param();
    gchar id_str[MAX_ID_LEN];

    // Handle the root task
    if (param_id->val_int == 0) {
        set_cur_task(NULL);
        goto done;
    }

    snprintf(id_str, MAX_ID_LEN, "%ld", param_id->val_int);
    gchar *sql_condition = g_strconcat("where id=", id_str, NULL);
    GSequence *records = select_tasks(sql_condition);
    g_free(sql_condition);
    if (g_sequence_get_length(records) != 1) {
        fprintf(stderr, "Unknown task id: %ld\n", param_id->val_int);
        goto done;
    }

    Task *task = copy_task(g_sequence_get(g_sequence_get_begin_iter(records)));
    g_sequence_free(records);
    set_cur_task(task);

done:
    free_param(param_id);
}


static void print_task(gpointer gp_task, gpointer gp_cur_task) {
    Task *task = gp_task;
    Task *cur_task = gp_cur_task;

    if (!task) {
        printf("%c    0: Root task\n", !cur_task ? '*' : ' ');
    }
    else {
        printf("%c[%c] %ld: %s\n", cur_task && cur_task->id == task->id ? '*' : ' ',
                                   task->is_done ? 'X' : ' ',
                                   task->id,
                                   task->name);
    }
}


// -----------------------------------------------------------------------------
/** Prints a GSequence of Task

This also frees the memory associated with the GSequence.
*/
// -----------------------------------------------------------------------------
static void EC_print(gpointer gp_entry) {
    Task *cur_task = get_cur_task();
    Param *param_seq = pop_param();
    GSequence *seq = param_seq->val_custom;

    g_sequence_foreach(seq, print_task, cur_task);
    printf("\n");

    g_sequence_free(seq);
    free_param(param_seq);
}


// -----------------------------------------------------------------------------
/** Pushes copy of cur-task onto the stack as a seq

This allows functions that operate on sequences of Tasks to work on the
current task.
*/
// -----------------------------------------------------------------------------
static void EC_seq_cur_task(gpointer gp_entry) {
    Task *cur_task = copy_task(get_cur_task());
    GSequence *seq = g_sequence_new(g_free);

    g_sequence_append(seq, cur_task);

    Param *param_new = new_custom_param(seq, "[cur-task]");
    push_param(param_new);
}


// -----------------------------------------------------------------------------
/** Pushes a seq of the siblings of the current task (including the task)

\note Whoever pops this off the stack is responsible for freeing it.
*/
// -----------------------------------------------------------------------------
static void EC_siblings(gpointer gp_entry) {
    Task *cur_task = get_cur_task();
    GSequence *seq = NULL;

    if (!cur_task) {
        seq = g_sequence_new(g_free);
        g_sequence_append(seq, cur_task);
    }
    else {
        gchar parent_id_str[MAX_ID_LEN];
        snprintf(parent_id_str, MAX_ID_LEN, "%ld", cur_task->parent_id);

        gchar *sql_condition = g_strconcat("where pc.parent=", parent_id_str, " order by id asc", NULL);
        seq = select_tasks(sql_condition);
        g_free(sql_condition);
    }

    Param *param_new = new_custom_param(seq, "[siblings]");
    push_param(param_new);
}



// -----------------------------------------------------------------------------
/** Pushes a sequence of all tasks onto the stack
*/
// -----------------------------------------------------------------------------
static void EC_all(gpointer gp_entry) {
    GSequence *records = select_tasks("");
    Param *param_new = new_custom_param(records, "[all]");
    push_param(param_new);
}


// -----------------------------------------------------------------------------
/** Pops a GSequence of Task and selects only those tasks that are incomplete
    and pushes this new sequence back onto the stack

This is essentially applies a filter to a sequence of tasks to return only
those which are incomplete.
*/
// -----------------------------------------------------------------------------
static void EC_incomplete(gpointer gp_entry) {
    // Pop sequence
    Param *param_seq = pop_param();
    GSequence *seq = param_seq->val_custom;

    // Gather incomplete tasks
    GSequence *incomplete = g_sequence_new(g_free);
    GSequenceIter *iter = g_sequence_get_begin_iter(seq);
    while (!g_sequence_iter_is_end(iter)) {
        Task *task = g_sequence_get(iter);

        if (!task->is_done) {
            g_sequence_append(incomplete, copy_task(task));
        }
        iter = g_sequence_iter_next(iter);
    }
    g_sequence_free(seq);
    free_param(param_seq);

    // Push incomplete Tasks
    Param *param_new = new_custom_param(incomplete, "[incomplete]");
    push_param(param_new);
}


// -----------------------------------------------------------------------------
/** Pushes a sequence of all ancestors of *cur-task onto the stack.
*/
// -----------------------------------------------------------------------------
static void EC_ancestors(gpointer gp_entry) {
    GSequence *seq = g_sequence_new(g_free);
    gchar parent_id_str[MAX_ID_LEN];

    Task *cur_task = copy_task(get_cur_task());
    while (cur_task) {
        g_sequence_prepend(seq, cur_task);

        // If parent is root...
        if (!cur_task->parent_id) {
            cur_task = NULL;
            continue;
        }

        //...otherwise, look up parent
        snprintf(parent_id_str, MAX_ID_LEN, "%ld", cur_task->parent_id);
        gchar *sql_condition = g_strconcat("where id=", parent_id_str, NULL);
        GSequence *records = select_tasks(sql_condition);
        g_free(sql_condition);
        cur_task = copy_task(g_sequence_get(g_sequence_get_begin_iter(records)));

        g_sequence_free(records);
    }

    g_sequence_prepend(seq, cur_task);

    Param *param_new = new_custom_param(seq, "[ancestors]");
    push_param(param_new);
}


// -----------------------------------------------------------------------------
/** Pushes a sequence of all children of *cur-task onto the stack
*/
// -----------------------------------------------------------------------------
static void EC_children(gpointer gp_entry) {
    gchar parent_id_str[MAX_ID_LEN];

    gint64 parent_id = get_cur_task_id();
    snprintf(parent_id_str, MAX_ID_LEN, "%ld", parent_id);

    gchar *sql_condition = g_strconcat("where pc.parent=", parent_id_str, " order by id asc", NULL);
    GSequence *seq = select_tasks(sql_condition);
    g_free(sql_condition);

    Param *param_new = new_custom_param(seq, "[children]");
    push_param(param_new);
}


// -----------------------------------------------------------------------------
/** Pushes a sequence of all children of the root task onto the stack.
*/
// -----------------------------------------------------------------------------
static void EC_level_1(gpointer gp_entry) {
    GSequence *seq = select_tasks("where pc.parent=0 order by id asc");
    Param *param_new = new_custom_param(seq, "[level-1]");
    push_param(param_new);
}


// -----------------------------------------------------------------------------
/** Sets cur_task to be done/not done
*/
// -----------------------------------------------------------------------------
static void EC_set_is_done(gpointer gp_entry) {
    Task *cur_task = get_cur_task();

    if (!cur_task) return;


    Param *param_bool = pop_param();

    cur_task->is_done = param_bool->val_int;
    free_param(param_bool);

    gchar id_str[MAX_ID_LEN];
    snprintf(id_str, MAX_ID_LEN, "%ld", cur_task->id);
    sqlite3 *connection = get_db_connection();
    gchar *sql = g_strconcat("update tasks set is_done=", cur_task->is_done ? "1" : "0", " ",
                             "where id=", id_str,
                             NULL);

    char *error_message = NULL;
    sqlite3_exec(connection, sql, NULL, NULL, &error_message);
    g_free(sql);

    if (error_message) {
        handle_error(ERR_GENERIC_ERROR);
        fprintf(stderr, "-----> Problem executing 'set-is-done'\n----->%s", error_message);
    }
}


// -----------------------------------------------------------------------------
/** Pops a note id and links the current task to it.
*/
// -----------------------------------------------------------------------------
static void EC_link_note(gpointer gp_entry) {
    gint64 task_id = get_cur_task_id();

    Param *param_id = pop_param();
    gint64 note_id = param_id->val_int;
    free_param(param_id);

    gchar task_id_str[MAX_ID_LEN];
    gchar note_id_str[MAX_ID_LEN];

    snprintf(task_id_str, MAX_ID_LEN, "%ld", task_id);
    snprintf(note_id_str, MAX_ID_LEN, "%ld", note_id);
    gchar *sql = g_strconcat("insert into task_notes(task, note) ",
                             "values(", task_id_str, ", ", note_id_str, ")", NULL);

    char *error_message = NULL;
    sqlite3 *connection = get_db_connection();
    sqlite3_exec(connection, sql, NULL, NULL, &error_message);
    g_free(sql);

    if (error_message) {
        handle_error(ERR_GENERIC_ERROR);
        fprintf(stderr, "-----> Problem executing 'link-note'\n----->%s", error_message);
    }
}


// -----------------------------------------------------------------------------
/** Callback to append a note ID onto an array.
*/
// -----------------------------------------------------------------------------
static int append_note_id_cb(gpointer gp_note_ids, int num_cols, char **values, char **cols) {
    GArray *note_ids = gp_note_ids;

    if (num_cols != 1) {
        handle_error(ERR_GENERIC_ERROR);
        fprintf(stderr, "-----> Unexpected num cols from task_notes query\n");
        return 1;
    }

    gint64 id = g_ascii_strtoll(values[0], NULL, 10);

    g_array_append_val(note_ids, id);
    return 0;
}


// -----------------------------------------------------------------------------
/** Looks up all the notes associated with a task and pushes a GArray of their
    ids onto the stack.
*/
// -----------------------------------------------------------------------------
static void EC_task_note_ids(gpointer gp_entry) {
    gint64 task_id = get_cur_task_id();
    gchar id_str[MAX_ID_LEN];
    snprintf(id_str, MAX_ID_LEN, "%ld", task_id);
    gchar *sql = g_strconcat("select note from task_notes where task=", id_str, " order by note asc ", NULL);

    char *error_message = NULL;
    GArray *note_ids = g_array_new(FALSE, TRUE, sizeof(gint64));
    sqlite3 *connection = get_db_connection();
    sqlite3_exec(connection, sql, append_note_id_cb, note_ids, &error_message);
    g_free(sql);

    if (error_message) {
        handle_error(ERR_GENERIC_ERROR);
        fprintf(stderr, "-----> Problem executing 'task_note_ids'\n----->%s", error_message);
    }

    Param *param_new = new_custom_param(note_ids, "Array[note_ids]");
    push_param(param_new);
}


// -----------------------------------------------------------------------------
/** Sets *cur-task to NULL, effectively freeing its memory.
*/
// -----------------------------------------------------------------------------
static void EC_reset(gpointer gp_entry) {
    set_cur_task(NULL);
}


// -----------------------------------------------------------------------------
/** Defines the tasks lexicon and adds it to the dictionary.

The following words are defined for manipulating Tasks:

- tasks-db: This holds the sqlite database connection for tasks

- +: (string -- ) Creates a task that's the sibling of *cur-task (or a child if *cur-task is root)
- ++: (string -- ) Creates a subtask of *cur-task

- d ( -- ) Makes the first child of *cur-task the new *cur-task
- u ( -- ) Makes the parent of *cur-task the new *cur-task
- g (id -- ) Makes the Task with id the new *cur-task

- set-is-done (bool -- ) Marks *cur-task as done/not done

- [cur-task] ( -- [*cur-task]) Pushes *cur-task onto the stack as a sequence

- all ( -- [tasks]) Pushes a sequence of all Tasks onto the stack
- siblings ( -- [tasks]) Pushes a sequence of all siblings of the *cur-task onto the stack
- ancestors ( -- [tasks]) Pushes a sequence of all ancestors of the *cur-task onto the stack
- children ( -- [tasks]) Pushes a sequence of all children of the *cur-task onto the stack
- level-1 ( -- [tasks]) Pushes a sequence of all children of the root task onto the stack

- task-note-ids ( -- [note ids]) Pushes an array of note ids linked to *cur-task

- incomplete ([tasks] -- [tasks]) Selects only the incomplete tasks

- link-note ( note-id -- )  Links the note with id note-id to *cur-task

- print-tasks ( [tasks] -- )  Prints a sequence of tasks

- reset ( -- )  Sets *cur-task to root

*/
// -----------------------------------------------------------------------------
void EC_add_tasks_lexicon(gpointer gp_entry) {
    // Add the lexicons that this depends on
    find_and_execute("lex-notes");  // This also includes lex-sqlite

    add_variable("tasks-db");

    // Holds the current task
    add_variable("*cur-task");
    set_cur_task(NULL);

    add_entry("+")->routine = EC_add_task;
    add_entry("++")->routine = EC_add_subtask;

    add_entry("d")->routine = EC_down;
    add_entry("u")->routine = EC_up;
    add_entry("g")->routine = EC_go;

    add_entry("set-is-done")->routine = EC_set_is_done;

    add_entry("[cur-task]")->routine = EC_seq_cur_task;

    add_entry("all")->routine = EC_all;
    add_entry("siblings")->routine = EC_siblings;
    add_entry("ancestors")->routine = EC_ancestors;
    add_entry("children")->routine = EC_children;
    add_entry("level-1")->routine = EC_level_1;

    add_entry("task-note_ids")->routine = EC_task_note_ids;

    add_entry("incomplete")->routine = EC_incomplete;

    add_entry("link-note")->routine = EC_link_note;

    add_entry("print-tasks")->routine = EC_print;
    add_entry("reset")->routine = EC_reset;
}
