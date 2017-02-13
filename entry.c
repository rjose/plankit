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
    return result;
}


// -----------------------------------------------------------------------------
/** Frees the memory allocated for an Entry

\param gp_entry: A pointer to an Entry to be freed.
*/
// -----------------------------------------------------------------------------
void free_entry(gpointer gp_entry) {
    g_free(gp_entry);
}
