Entry* find_entry(const gchar* word) {
    for (GList* l=g_list_last(_dictionary); l != NULL; l = l->prev) {
        Entry *entry = l->data;
        if (g_strcmp0(word, entry->word) == 0) return entry;
    }
    return NULL;
}


// Allocates new entry, adds it to the dictionary, and returns it.
Entry *add_entry(const gchar *word) {
    Entry *result = g_new(Entry, 1);
    g_strlcpy(result->word, word, MAX_WORD_LEN);
    _dictionary = g_list_append(_dictionary, result);
    return result;
}


// -----------------------------------------------------------------------------
// Builds initial dictionary
//
// NOTE: Entries are allocated and so it's appropriate to do a g_list_free_full
//       if the dictionary should be rebuilt.
// -----------------------------------------------------------------------------
void build_dictionary() {
    Entry *entry;

    entry = add_entry(":");
    entry->routine = NULL;  // TODO: Set this

    entry = add_entry(";");
    entry->routine = NULL;  // TODO: Set this
}


void destroy_dictionary() {
    g_list_free_full(_dictionary, free_entry);
}
