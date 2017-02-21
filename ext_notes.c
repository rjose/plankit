/** \file ext_notes.c
*/

#define MAX_TIMESTAMP_LEN 48
#define MAX_ELAPSED_LEN 16

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
/** Computes the elapsed minutes between two time_t structs.
*/
// -----------------------------------------------------------------------------
static gint64 elapsed_min(time_t time_l, time_t time_r) {
    double delta_s = difftime(time_l, time_r);
    gint64 result = delta_s/60.0;
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
/** Prints all notes for today
*/
// -----------------------------------------------------------------------------
static void EC_today(gpointer gp_entry) {
    sqlite3 *connection = get_db_connection();
    gchar *query = "select id, type, note, timestamp, date from notes where date = date('now')";
    char *error_message = NULL;
    gchar elapsed_min[MAX_ELAPSED_LEN];

    GSequence *records = g_sequence_new(free_note);

    sqlite3_exec(connection, query, append_note_cb, records, &error_message);

    if (error_message) {
        handle_error(ERR_GENERIC_ERROR);
        fprintf(stderr, "-----> Problem executing 'today'\n");
    }

    Note *current_start_note = NULL;
    GSequenceIter *iter = g_sequence_get_begin_iter(records);
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
                write_elapsed_minutes(elapsed_min, MAX_ELAPSED_LEN, note, current_start_note);
                printf("(%s min) %s\n%s\n\n", elapsed_min, note->timestamp_text, note->note);
                break;

            case 'E':
                write_elapsed_minutes(elapsed_min, MAX_ELAPSED_LEN, note, current_start_note);
                printf("<< (%s min) %s\n%s\n\n", elapsed_min, note->timestamp_text, note->note);
                current_start_note = NULL;
                break;

            default:
                printf("TODO: Format this:\n--> %s\n\n", note->note);
                break;
        }

        iter = g_sequence_iter_next(iter);
    }
    g_sequence_free(records);
}


// -----------------------------------------------------------------------------
/** Prints the elapsed time since the last 'S' note.
*/
// -----------------------------------------------------------------------------
static void EC_time(gpointer gp_entry) {
    // ---------------------------------
    // Get latest 'S' note
    // ---------------------------------
    sqlite3 *connection = get_db_connection();
    gchar *query = "select id, type, note, timestamp, date from notes where type = 'S' "
                   "order by id desc "
                   "limit 1 ";
    char *error_message = NULL;

    GSequence *records = g_sequence_new(free_note);

    sqlite3_exec(connection, query, append_note_cb, records, &error_message);

    if (error_message) {
        handle_error(ERR_GENERIC_ERROR);
        fprintf(stderr, "-----> Problem executing 'time'\n----->%s", error_message);
    }

    // ---------------------------------
    // Figure out elapsed time and print
    // ---------------------------------
    if (g_sequence_get_length(records) != 1) {
        printf("? min\n");
    }
    else {
        Note *start_note = g_sequence_get(g_sequence_get_begin_iter(records));
        time_t start_note_time = mktime(&start_note->timestamp);
        time_t now = time(NULL);
        gint64 minutes = elapsed_min(now, start_note_time);
        printf("%ld min\n", minutes);
    }

    // Cleanup
    g_sequence_free(records);
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
    add_entry("today")->routine = EC_today;
    add_entry("time")->routine = EC_time;
}
