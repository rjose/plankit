void EC_constant(Entry *entry) {
    // . Read next word from input
    // . Create new entry
    // . Pop value from stack and add to entries param
    // . Set entry's routine to add_entry_param0_to_stack
}


void EC_print_hi(gpointer gp_entry) {
    Entry *entry = gp_entry;
    printf("(%s) -> Howdy!\n", entry->word);
}
