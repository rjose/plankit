// =============================================================================
/** \file

This defines the functions for manipulating the global Forth dictionary: _dictionary.
*/
// =============================================================================


// -----------------------------------------------------------------------------
/** Searches for an entry in the dictionary, starting from the most recent entry
and moving backwards.

\param word: The string to search for
\returns A pointer to the entry or NULL if not found
*/
// -----------------------------------------------------------------------------
Entry* find_entry(const gchar* word) {
    for (GList* l=g_list_last(_dictionary); l != NULL; l = l->prev) {
        Entry *entry = l->data;
        if (g_strcmp0(word, entry->word) == 0) return entry;
    }
    return NULL;
}


// -----------------------------------------------------------------------------
/** Allocates new Entry, adds it to the dictionary, and returns it.

\param word: The word to use for the new entry 
\returns The newly created entry
*/
// -----------------------------------------------------------------------------
Entry *add_entry(const gchar *word) {
    Entry *result = g_new(Entry, 1);
    g_strlcpy(result->word, word, MAX_WORD_LEN);
    _dictionary = g_list_append(_dictionary, result);
    return result;
}


// -----------------------------------------------------------------------------
/** Builds the dictionary for the interpreter.

This defines the basic words for the interpreter and will allow loading
of custom extensions for various applications. This is TBD, but the intent is
that we can control the extensions dynamically.

\note Entries are new'd and so it's appropriate to do a g_list_free_full
      if the dictionary should be rebuilt.
*/
// -----------------------------------------------------------------------------
void build_dictionary() {
    Entry *entry;

    entry = add_entry("hi");
    entry->routine = EC_print_hi;
}


// -----------------------------------------------------------------------------
/** Deallocates dictionary and all of its entries.
*/
// -----------------------------------------------------------------------------
void destroy_dictionary() {
    g_list_free_full(_dictionary, free_entry);
}
