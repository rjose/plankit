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

    // NOTE: When we add the popped param to the entry, its memory is
    //       managed by that entry.
    Entry *entry_new = add_entry(token.word);
    entry_new->routine = EC_push_param0;
    add_entry_param(entry_new, param0);
}


// -----------------------------------------------------------------------------
/** Pushes the first parameter of an entry onto the stack.

\param: gp_entry: The entry with the parameter to be pushed.
*/
// -----------------------------------------------------------------------------
void EC_push_param0(gpointer gp_entry) {
    Entry *entry = gp_entry;
    GSequenceIter *begin = g_sequence_get_begin_iter(entry->params);
    Param *param0 = g_sequence_get(begin);
    push_param(param0);
}


static void gfunc_print_param(gpointer gp_param, gpointer user_data) {
    Param *param = gp_param;
    print_param(param);
}


// -----------------------------------------------------------------------------
/** Prints stack nondestructively. Top of stack is at the right.

*/
// -----------------------------------------------------------------------------
void EC_print_stack(gpointer gp_entry) {
    // NOTE: We're assuming that this goes from the first element to the last
    g_queue_foreach(_stack, gfunc_print_param, NULL);
    printf("\n");
}


// -----------------------------------------------------------------------------
/** Routine for the define word (":")

\param gp_entry: The ":" entry

We read the next token, which will be the word for the new definition. Then we
switch to compile mode so each word we read can be added to the parameters of
the new entry as part of its definition. Different categories of tokens are
compiled differently:

- Dictionary entries: An "E" parameter is created and added to the new definition.
                      On execution, the entry is simply executed.

- Literals:           A "P" parameter is created with its first parameter
                      being the literal. The routine of the "P" parameter
                      pushes the first param onto the stack. 

- Immediate words:    These are words like ';' that are executed during a
                      compilation. Macros are an example of an immediate
                      word.

The first parameter is a pointer to the routine for executing a defined word. By
allowing this to be dynamically specified, we change the behavior of defined
words.
*/
// -----------------------------------------------------------------------------
void EC_define(gpointer gp_entry) {
    // Create new entry
    Token token = get_token();
    Entry *entry_new = add_entry(token.word);
    entry_new->routine = EC_execute;

    _mode = 'C';
}

void EC_pop_return_stack(gpointer gp_entry) {
    _ip = pop_param_r();
}

void EC_end_define(gpointer gp_entry) {
    Entry *entry_latest = latest_entry();
    Param *pseudo_param = new_pseudo_entry_param(";", EC_pop_return_stack);
    add_entry_param(entry_latest, pseudo_param);

    _mode = 'E';
}


void EC_print_definition(gpointer gp_entry) {
    Token token = get_token();
    Entry *entry = find_entry(token.word);
    if (!entry) {
        longjmp(_error_jmp_buf, ERR_UNKNOWN_WORD);
        return;
    }

    printf("Entry: %s\n", entry->word);
    for (GSequenceIter *iter = g_sequence_get_begin_iter(entry->params);
         !g_sequence_iter_is_end(iter);
         iter = g_sequence_iter_next(iter)) {

        Param *p = g_sequence_get(iter);
        print_param(p);
    }
}

void EC_execute(gpointer gp_entry) {
    Entry *entry = gp_entry;
    Param *cur_param;
    Entry *pseudo_entry;

    push_param_r(_ip);

    _ip = g_sequence_get_begin_iter(entry->params);

    while (_ip) {
        cur_param = g_sequence_get(_ip);
        _ip = g_sequence_iter_next(_ip);

        switch(cur_param->type) {
            case 'E':
                execute(cur_param->val_entry);
                break;

            case 'P':
                pseudo_entry = &cur_param->val_pseudo_entry;
                pseudo_entry->routine(pseudo_entry);
                break;

            default:
                longjmp(_error_jmp_buf, ERR_UNKNOWN_WORD);
                break;
        }
    }
}
