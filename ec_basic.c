/** \file ec_basic.c

\brief Basic entry routines for builtin words.

These are entry routines for the basic builtin words as well as generic
routines used when defining entries dynamically.

*/

// -----------------------------------------------------------------------------
/** Creates a new constant entry.

This reads the next token and uses this as the word for the entry. It pops a
parameter off the stack and uses this as the value of the constant. The
routine for the new constant pushes this value onto the stack.

\param gp_entry: Unused entry for the "constant" word
*/
// -----------------------------------------------------------------------------
void EC_constant(gpointer gp_entry) {
    Token token = get_token();
    Param *param0 = pop_param();

    Entry *entry_new = add_entry(token.word);
    entry_new->routine = EC_push_param0;
    add_entry_param(entry_new, *param0);
    free_param(param0);
}


// -----------------------------------------------------------------------------
/** Pushes the first parameter of an entry onto the stack.

\param: gp_entry: The entry with the parameter to be pushed.
*/
// -----------------------------------------------------------------------------
void EC_push_param0(gpointer gp_entry) {
    Entry *entry = gp_entry;
    Param *param0 = new_param();
    *param0 = g_array_index(entry->params, Param, 0);
    push_param(param0);
}


void EC_print_hi(gpointer gp_entry) {
    Entry *entry = gp_entry;
    printf("(%s) -> Howdy!\n", entry->word);
}
