/** \file ext_sqlite.c
*/

// -----------------------------------------------------------------------------
/** Pops a db filename, opens an sqlite3 connection to it, and pushes the
connection onto the stack.

*/
// -----------------------------------------------------------------------------
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


// -----------------------------------------------------------------------------
/** Pops a database connection and closes it.
*/
// -----------------------------------------------------------------------------
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



// -----------------------------------------------------------------------------
/** Defines sqlite3 lexicon and adds it to the dictionary.


The following words are defined:

- sqlite3-open: (db-name -- db-connection) Opens a connection to a database
- sqlite3-close: (db-connection -- ) Closes a connection to a database

*/
// -----------------------------------------------------------------------------
void EC_add_sqlite_lexicon(gpointer gp_entry) {
    add_entry("sqlite3-open")->routine = EC_sqlite3_open; 
    add_entry("sqlite3-close")->routine = EC_sqlite3_close; 
}
