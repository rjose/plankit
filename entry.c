/** \file
*/



// -----------------------------------------------------------------------------
/** Executes the code associated with an Entry.

\param gp_entry: A pointer to an Entry
*/
// -----------------------------------------------------------------------------
void execute(gpointer gp_entry) {
    Entry *entry = gp_entry;
    entry->routine(entry);
}


// -----------------------------------------------------------------------------
/** Allocates memory for an Entry and returns a pointer to it

\returns A pointer to a newly allocated Entry
*/
// -----------------------------------------------------------------------------
Entry *new_entry() {
    Entry *result = g_new(Entry, 1);
    result->params = g_array_new(FALSE, TRUE, sizeof(Param));
    return result;
}


void add_entry_param(Entry *entry, Param param) {
    g_array_append_val(entry->params, param);
}


// -----------------------------------------------------------------------------
/** Frees the memory allocated for an Entry

\param gp_entry: A pointer to an Entry to be freed.
*/
// -----------------------------------------------------------------------------
void free_entry(gpointer gp_entry) {
    Entry *entry = gp_entry;
    g_array_free(entry->params, TRUE);
    g_free(gp_entry);
}
