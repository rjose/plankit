/** \file ext_notes.c
*/

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


static void EC_today(gpointer gp_entry) {
    printf("TODO: Print all the notes for today\n");
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
