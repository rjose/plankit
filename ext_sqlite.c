/** \file ext_sqlite.c
*/

#if 0
int sqlite3_open(
  const char *filename,   /* Database filename (UTF-8) */
  sqlite3 **ppDb          /* OUT: SQLite db handle */
);
#endif


static void EC_sqlite3_open(gpointer gp_entry) {
    Param *db_file = pop_param();

    sqlite3 *connection;
    int sqlite_status = sqlite3_open(db_file->val_string, &connection);
    if (sqlite_status != SQLITE_OK) {
        handle_error(ERR_GENERIC_ERROR);
        fprintf(stderr, "-----> sqlite3_open failed\n");
        return;
    }
    Param *param_new = new_custom_param(connection, "sqlite3 connection");
    push_param(param_new);

    free_param(db_file);
}


static void EC_sqlite3_close(gpointer gp_entry) {
    Param *param_connection = pop_param();

    sqlite3 *connection = param_connection->val_custom;

    int sqlite_status = sqlite3_close(connection);
    if (sqlite_status != SQLITE_OK) {
        handle_error(ERR_GENERIC_ERROR);
        fprintf(stderr, "-----> sqlite3_close failed\n");
        return;
    }

    free_param(param_connection);
}



void EC_add_sqlite_lexicon(gpointer gp_entry) {
    add_entry("sqlite3-open")->routine = EC_sqlite3_open; 
    add_entry("sqlite3-close")->routine = EC_sqlite3_close; 
}
