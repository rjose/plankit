/** \file
*/

/** Entry code for "constant"
*/
void EC_constant(gpointer gp_entry) {
    Token token = get_token();
    Param *param0 = pop_param();

    Entry *entry_new = add_entry(token.word);
    entry_new->routine = EC_push_param0;
    add_entry_param(entry_new, *param0);
    free_param(param0);
}


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
