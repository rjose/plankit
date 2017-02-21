/** \file ext_notes.c
*/

#define MAX_TIMESTAMP_LEN 48

typedef struct {
    gint64 id;
    gchar *note;

    gchar timestamp_text[MAX_TIMESTAMP_LEN];
    gchar date_text[MAX_TIMESTAMP_LEN];
    struct tm timestamp;
} Note;


static Note *new_note(gint64 id, const gchar *note, const gchar *timestamp_text, const gchar *date_text) {
    Note *result = g_new(Note, 1);
    result->id = id;
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

static void free_note(gpointer gp_note) {
    Note *note = gp_note;
    g_free(note->note);
    g_free(note);
}



static sqlite3 *get_db_connection() {
    find_and_execute("notes-db");
    find_and_execute("@");
    Param *param_connection = pop_param();

    sqlite3 *result = param_connection->val_custom;

    free_param(param_connection);
    return result;
}

static void store_note(const gchar *type) {
    Param *param_note = pop_param();

    sqlite3 *connection = get_db_connection();

    char* error_message = NULL;
    gchar *sql = g_strconcat("insert into notes(note, type, timestamp, date)",
                             "values(\"", param_note->val_string, "\", ",
                             "'", type, "', ",
                             "datetime('now', 'localtime'), date('now'))",
                             NULL);

    sqlite3_exec(connection, sql, NULL, NULL, &error_message);

    if (error_message) {
        handle_error(ERR_GENERIC_ERROR);
        fprintf(stderr, "-----> Problem storing '%s' note ==> %s\n", type, error_message);
    }

    free_param(param_note);
    g_free(sql);
}


static void EC_start_chunk(gpointer gp_entry) {
    store_note("S");
}


static void EC_middle_chunk(gpointer gp_entry) {
    store_note("M");
}


static void EC_end_chunk(gpointer gp_entry) {
    store_note("E");
}


static void EC_generic_note(gpointer gp_entry) {
    store_note("N");
}


static int append_note_cb(gpointer gp_records, int num_cols, char **values, char **cols) {
    GSequence *records = gp_records;
    if (num_cols != 4) {
        handle_error(ERR_GENERIC_ERROR);
        fprintf(stderr, "-----> Unexpected num cols from note query\n");
        return 1;
    }
    gint64 id = g_ascii_strtoll(values[0], NULL, 10);
    const gchar *note_text = values[1];
    const gchar *timestamp_text = values[2];
    const gchar *date_text = values[3];

    Note *note_new = new_note(id, note_text, timestamp_text, date_text);

    g_sequence_append(records, note_new);
    return 0;
}



static void EC_today(gpointer gp_entry) {
    sqlite3 *connection = get_db_connection();
    gchar *query = "select id, note, timestamp, date from notes where date = date('now')";
    char *error_message = NULL;

    GSequence *records = g_sequence_new(free_note);

    sqlite3_exec(connection, query, append_note_cb, records, &error_message);

    if (error_message) {
        handle_error(ERR_GENERIC_ERROR);
        fprintf(stderr, "-----> Problem executing 'today'\n");
    }

    GSequenceIter *iter = g_sequence_get_begin_iter(records);
    while (!g_sequence_iter_is_end(iter)) {
        Note *note = g_sequence_get(iter);
        printf("TODO: Format this:\n>> %s\n\n", note->note);
        iter = g_sequence_iter_next(iter);
    }
    g_sequence_free(records);
}

void EC_add_notes_lexicon(gpointer gp_entry) {
    // Add the lexicons that this depends on
    find_and_execute("lex-sqlite");

    add_variable("notes-db");

    add_entry("S")->routine = EC_start_chunk;
    add_entry("M")->routine = EC_middle_chunk;
    add_entry("N")->routine = EC_generic_note;
    add_entry("E")->routine = EC_end_chunk;
    add_entry("today")->routine = EC_today;
}
