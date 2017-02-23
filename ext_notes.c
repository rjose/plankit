/** \file ext_notes.c

The associated schema of a notes.db is this:

- CREATE TABLE notes(type TEXT, id INTEGER PRIMARY KEY, note TEXT, timestamp TEXT, date TEXT);

*/

#define MAX_TIMESTAMP_LEN 48
#define MAX_ELAPSED_LEN 16
#define MAX_ID_LEN 16
#define NOTE_FIELDS   "id, type, note, timestamp, date from notes "

// -----------------------------------------------------------------------------
/** Represents a note from a database record
*/
// -----------------------------------------------------------------------------
typedef struct {
    gint64 id;     /**< \brief Record ID */
    gchar *note;   /**< \brief Text of note */
    gchar type;    /**< \brief 'S', 'M', 'E', or 'N'  */

    gchar timestamp_text[MAX_TIMESTAMP_LEN];  /**< Note timestamp string */
    gchar date_text[MAX_TIMESTAMP_LEN];       /**< Note date string */
    struct tm timestamp;                      /**< Timestamp as tm struct */
} Note;


// -----------------------------------------------------------------------------
/** Creates a new Note.
*/
// -----------------------------------------------------------------------------
static Note *new_note(gint64 id, gchar type, const gchar *note, const gchar *timestamp_text, const gchar *date_text) {
    Note *result = g_new(Note, 1);
    result->id = id;
    result->type = type;
    result->note = g_strdup(note);
    g_strlcpy(result->timestamp_text, timestamp_text, MAX_TIMESTAMP_LEN);
    g_strlcpy(result->date_text, date_text, MAX_TIMESTAMP_LEN);
    struct tm *timestamp = getdate(timestamp_text);
    if (!timestamp) {
        handle_error(ERR_GENERIC_ERROR);
        fprintf(stderr, "----->Unable to parse timestamp: getdate_err: %d\n", getdate_err);
    }
    result->timestamp = *timestamp;

    return result;
}


static Note *copy_note(Note *src) {
    Note *result = new_note(src->id, src->type, src->note, src->timestamp_text, src->date_text);
    return result;
}

// -----------------------------------------------------------------------------
/** Frees an allocated Note.
*/
// -----------------------------------------------------------------------------
static void free_note(gpointer gp_note) {
    Note *note = gp_note;
    g_free(note->note);
    g_free(note);
}



// -----------------------------------------------------------------------------
/** Gets a database connection from the "notes-db" variable.
*/
// -----------------------------------------------------------------------------
static sqlite3 *get_db_connection() {
    find_and_execute("notes-db");
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
static int append_note_cb(gpointer gp_records, int num_cols, char **values, char **cols) {
    GSequence *records = gp_records;
    if (num_cols != 5) {
        handle_error(ERR_GENERIC_ERROR);
        fprintf(stderr, "-----> Unexpected num cols from note query\n");
        return 1;
    }
    gint64 id = g_ascii_strtoll(values[0], NULL, 10);
    const gchar *type_text = values[1];
    const gchar *note_text = values[2];
    const gchar *timestamp_text = values[3];
    const gchar *date_text = values[4];

    Note *note_new = new_note(id, type_text[0], note_text, timestamp_text, date_text);

    g_sequence_append(records, note_new);
    return 0;
}


// -----------------------------------------------------------------------------
/** Returns a GSequence of notes matching the conditions.

\note The caller is responsible for freeing the returned GSequence.
*/
// -----------------------------------------------------------------------------
static GSequence *select_notes(const gchar *sql_conditions) {
    sqlite3 *connection = get_db_connection();

    gchar *select = "select " NOTE_FIELDS;
    gchar *query = g_strconcat(select, sql_conditions, NULL);


    GSequence *result = g_sequence_new(free_note);

    char *error_message = NULL;
    sqlite3_exec(connection, query, append_note_cb, result, &error_message);

    if (error_message) {
        handle_error(ERR_GENERIC_ERROR);
        fprintf(stderr, "-----> Problem executing 'select_notes'\n----->%s", error_message);

        g_free(result);
        result = NULL;
    }

    // Cleanup
    g_free(query);

    return result;
}


// -----------------------------------------------------------------------------
/** Helper function to write notes to the database.
*/
// -----------------------------------------------------------------------------
static void store_note(const gchar *type) {
    Param *param_note = pop_param();

    sqlite3 *connection = get_db_connection();

    char* error_message = NULL;
    gchar *sql = g_strconcat("insert into notes(note, type, timestamp, date)",
                             "values(\"", param_note->val_string, "\", ",
                             "'", type, "', ",
                             "datetime('now', 'localtime'), date('now', 'localtime'))",
                             NULL);

    sqlite3_exec(connection, sql, NULL, NULL, &error_message);

    if (error_message) {
        handle_error(ERR_GENERIC_ERROR);
        fprintf(stderr, "-----> Problem storing '%s' note ==> %s\n", type, error_message);
    }

    free_param(param_note);
    g_free(sql);
}


// -----------------------------------------------------------------------------
/** Stores a start note in the notes database.
*/
// -----------------------------------------------------------------------------
static void EC_start_chunk(gpointer gp_entry) {
    store_note("S");
}


// -----------------------------------------------------------------------------
/** Stores a middle note in the notes database.
*/
// -----------------------------------------------------------------------------
static void EC_middle_chunk(gpointer gp_entry) {
    store_note("M");
}


// -----------------------------------------------------------------------------
/** Stores an end note in the notes database.
*/
// -----------------------------------------------------------------------------
static void EC_end_chunk(gpointer gp_entry) {
    store_note("E");
}


// -----------------------------------------------------------------------------
/** Stores a generic note in the notes database.
*/
// -----------------------------------------------------------------------------
static void EC_generic_note(gpointer gp_entry) {
    store_note("N");
}


// -----------------------------------------------------------------------------
/** Computes the elapsed minutes between two time_t structs.
*/
// -----------------------------------------------------------------------------
static gint64 elapsed_min(time_t time_l, time_t time_r) {
    double delta_s = difftime(time_l, time_r);
    gint64 result = ceil(delta_s/60.0);
    return result;
}


// -----------------------------------------------------------------------------
/** Returns the number of minutes between two notes.
*/
// -----------------------------------------------------------------------------
static gint64 get_minute_difference(Note *note_l, Note *note_r) {
    time_t time_l = mktime(&note_l->timestamp);
    time_t time_r = mktime(&note_r->timestamp);
    return elapsed_min(time_l, time_r);
}


// -----------------------------------------------------------------------------
/** Writes the elapsed minutes between two notes into a char buffer.
*/
// -----------------------------------------------------------------------------
static void write_elapsed_minutes(gchar *dst, gint64 len, Note *note_l, Note *note_r) {
    if (!note_r) {
        g_strlcpy(dst, "?", len);
    }
    else {
        gint64 num_minutes = get_minute_difference(note_l, note_r);
        snprintf(dst, len, "%ld", num_minutes);
    }
}


// -----------------------------------------------------------------------------
/** Gets all notes for today and pushes onto stack
*/
// -----------------------------------------------------------------------------
static void EC_today_notes(gpointer gp_entry) {
    GSequence *records = select_notes("where date = date('now', 'localtime')");
    Param *param_new = new_custom_param(records, "GSequence of Note");
    push_param(param_new);
}



static Note *get_latest_S_note() {
    GSequence *records = select_notes("where type = 'S' order by id desc limit 1");

    Note *result = NULL;
    if (g_sequence_get_length(records) == 1) {
        result = copy_note(g_sequence_get(g_sequence_get_begin_iter(records)));
    }

    // Cleanup
    g_sequence_free(records);

    return result;
}


static Note *get_latest_SE_note() {
    GSequence *records = select_notes("where type = 'S' or type = 'E' order by id desc limit 1");

    Note *result = NULL;
    if (g_sequence_get_length(records) == 1) {
        result = copy_note(g_sequence_get(g_sequence_get_begin_iter(records)));
    }

    // Cleanup
    g_sequence_free(records);

    return result;
}


// -----------------------------------------------------------------------------
/** Prints the elapsed time since the last 'S' note.
*/
// -----------------------------------------------------------------------------
static void EC_time(gpointer gp_entry) {
    Note *note = get_latest_SE_note();

    if (!note) {
        printf("? min\n");
    }
    else {
        time_t start_note_time = mktime(&note->timestamp);
        time_t now = time(NULL);
        gint64 minutes = elapsed_min(now, start_note_time);
        printf("%ld min\n", minutes);
    }

    free_note(note);
}


static void EC_chunk_notes(gpointer gp_entry) {
    GSequence *records = NULL;
    const int CONDITION_LEN = 128;
    gchar sql_condition[CONDITION_LEN];

    Note *note = get_latest_S_note();

    // If no starting note, then create an empty sequence of records
    if (!note) {
        records = g_sequence_new(free_note);
    }
    else {
        snprintf(sql_condition, CONDITION_LEN, "where id >= %ld", note->id);
        records = select_notes(sql_condition);
        free_note(note);
    }
    Param *param_new = new_custom_param(records, "GSequence of Notes");
    push_param(param_new);
}


static void EC_print(gpointer gp_entry) {
    // Pop Note sequence
    Param *param_note_sequence = pop_param();
    GSequence *records = param_note_sequence->val_custom;

    // Print each note
    Note *current_start_note = NULL;
    GSequenceIter *iter = g_sequence_get_begin_iter(records);
    gchar elapsed_min_text[MAX_ELAPSED_LEN];

    while (!g_sequence_iter_is_end(iter)) {
        Note *note = g_sequence_get(iter);

        switch(note->type) {
            case 'N':
                printf("%s\n%s\n\n", note->timestamp_text, note->note);
                break;

            case 'S':
                current_start_note = note;
                printf("\n>> %s\n%s\n\n", note->timestamp_text, note->note);
                break;

            case 'M':
                write_elapsed_minutes(elapsed_min_text, MAX_ELAPSED_LEN, note, current_start_note);
                printf("(%s min) %s\n%s\n\n", elapsed_min_text, note->timestamp_text, note->note);
                break;

            case 'E':
                write_elapsed_minutes(elapsed_min_text, MAX_ELAPSED_LEN, note, current_start_note);
                printf("<< (%s min) %s\n%s\n\n", elapsed_min_text, note->timestamp_text, note->note);
                current_start_note = NULL;
                break;

            default:
                printf("TODO: Format this:\n--> %s\n\n", note->note);
                break;
        }

        iter = g_sequence_iter_next(iter);
    }

    // Cleanup
    g_sequence_free(records);
    free_param(param_note_sequence);
}


// -----------------------------------------------------------------------------
/** Pops an Array of int note IDs from the stack and converts them into Notes.

We want to convert the Array of gint64 into a string like this: "(1,2,3,4)". If
we assume the maximum number of digits for each ID is MAX_ID_LEN, then the size of this
string should be:

    MAX_ID_LEN*n +   # digits
    (n-1) +  # commas
    2        # parens
    1        # NUL

We'll allocate a string, start at the beginning, and snprintf the numbers into
it as we go along. We'll put parens and commas as needed and then end with the NUL.
When this works, we should move it into the sqlite lexicon.
*/
// -----------------------------------------------------------------------------
static void EC_note_ids_to_notes(gpointer gp_entry) {
    Param *param_note_ids = pop_param();
    GArray *note_ids = param_note_ids->val_custom;

    GSequence *notes = NULL;
    if (note_ids->len == 0) {
        notes = g_sequence_new(free_note);
        goto done;
    }

    guint num_bytes = MAX_ID_LEN*note_ids->len + (note_ids->len - 1) + 2 + 1;
    gchar *id_list = g_malloc0(num_bytes);
    guint cur_pos = 0;

    // Build up list
    id_list[cur_pos++] = '(';
    for (guint i=0; i < note_ids->len; i++) {
       gint64 note_id = g_array_index(note_ids, gint64, i);
       guint num_digits = snprintf(id_list + cur_pos, MAX_ID_LEN, "%ld", note_id);
       cur_pos += num_digits;
       id_list[cur_pos++] = ',';
    }

    // Change the last ',' to a closing paren
    id_list[cur_pos-1] = ')';
    id_list[cur_pos++] = 0;


    // Select notes
    gchar *sql_condition = g_strconcat("where id in ", id_list, NULL);
    notes = select_notes(sql_condition);
    g_free(sql_condition);
    g_free(id_list);

    Param *param_new = NULL;

done:
    param_new = new_custom_param(notes, "[notes]");
    push_param(param_new);

    g_array_free(note_ids, TRUE);
    free_param(param_note_ids);
}


// -----------------------------------------------------------------------------
/** Defines the notes lexicon and adds it to the dictionary.

The following words are defined for manipulating notes:

- S: Creates a note that starts a work chunk
- M: Creates a note in the middle of a work chunk
- E: Creates a note that ends a work chunk
- N: Creates a generic note
- time: Prints the elapsed time since the last start note

*/
// -----------------------------------------------------------------------------
void EC_add_notes_lexicon(gpointer gp_entry) {
    // Add the lexicons that this depends on
    find_and_execute("lex-sqlite");

    add_variable("notes-db");

    add_entry("S")->routine = EC_start_chunk;
    add_entry("M")->routine = EC_middle_chunk;
    add_entry("N")->routine = EC_generic_note;
    add_entry("E")->routine = EC_end_chunk;
    add_entry("time")->routine = EC_time;
    add_entry("today-notes")->routine = EC_today_notes;
    add_entry("chunk-notes")->routine = EC_chunk_notes;
    add_entry("print-notes")->routine = EC_print;
    add_entry("note_ids-to-notes")->routine = EC_note_ids_to_notes;
}
