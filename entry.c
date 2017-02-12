/** \file
*/


void execute(gpointer gp_entry) {
    Entry *entry = gp_entry;
    entry->routine(entry);
}

void free_entry(gpointer gp_entry) {
    Entry *entry = gp_entry;
    printf("TODO: Free entry: %s\n", entry->word);
}
