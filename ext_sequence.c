/** \file ext_sequence.c

\brief Defines words for operating on sequences

*/


// -----------------------------------------------------------------------------
/** Helper function to get the value of an object given a word that can extract it.

This pushes the object onto the stack and then executes the word. It then
pops the value and the object and returns the value.

TODO: Fix 'execute' so it manages the return stack
*/
// -----------------------------------------------------------------------------
static Param *get_sort_value(gconstpointer const_obj, Entry *sort_entry) {
    gpointer obj = (gpointer) const_obj;

    push_param(new_custom_param(obj, "?"));    // (obj)
    execute(sort_entry);                       // (obj val)
    Param *result = pop_param();               // (obj)
    free_param(pop_param());                   // ()
    return result;
}



// -----------------------------------------------------------------------------
/** Comparator for generic objects using a sort word (ascending order)
*/
// -----------------------------------------------------------------------------
static gint sort_sequence_asc_cmp(gconstpointer l, gconstpointer r, gpointer gp_sort_entry) {
    Entry *sort_entry = gp_sort_entry;
    Param *param_l_val = get_sort_value(l, sort_entry);
    Param *param_r_val = get_sort_value(r, sort_entry);

    gint result = 0;

    if (param_l_val->type == 'I') {
        result = param_l_val->val_int - param_r_val->val_int;
    }
    else if (param_l_val->type == 'D') {
        result = param_l_val->val_double - param_r_val->val_double;
    }
    else if (param_l_val->type == 'S') {
        result = g_strcmp0(param_l_val->val_string, param_r_val->val_string);    
    }
    else {
        fprintf(stderr, "Don't know how to compare '%c'\n", param_l_val->type);
    }

    free_param(param_l_val);
    free_param(param_r_val);
    return result;
}



// -----------------------------------------------------------------------------
/** Comparator for generic objects using a sort word (descending order)
*/
// -----------------------------------------------------------------------------
static gint sort_sequence_desc_cmp(gconstpointer l, gconstpointer r, gpointer gp_sort_entry) {
    return -sort_sequence_asc_cmp(l, r, gp_sort_entry);
}



// -----------------------------------------------------------------------------
/** Sorts a sequence using a word that gets the value from an object

(seq sort-word -- seq)
*/
// ----------------------------------------------------------------------------
static void sort_sequence(GCompareDataFunc cmp_func) {
    // Pop the sort word
    Param *param_word = pop_param();

    // Pop the sequence
    Param *param_seq = pop_param();
    GSequence *sequence = param_seq->val_custom;

    // Get the word to sort by
    Entry *entry = find_entry(param_word->val_string); 

    if (!entry) {
        handle_error(ERR_UNKNOWN_WORD);
        fprintf(stderr, "----> %s\n", param_word->val_string);
        fprintf(stderr, "----> Unable to sort by this\n");
        free_param(param_seq);
        goto done;
    }

    g_sequence_sort(sequence, cmp_func, entry);
    push_param(param_seq);

done:
    free_param(param_word);
    return;
}



// -----------------------------------------------------------------------------
/** Sorts sequence in ascending order (see sort_sequence)
*/
// -----------------------------------------------------------------------------
static void EC_ascending(gpointer gp_entry) {
    sort_sequence(sort_sequence_asc_cmp);
}



// -----------------------------------------------------------------------------
/** Sorts sequence in descending order (see sort_sequence)
*/
// -----------------------------------------------------------------------------
static void EC_descending(gpointer gp_entry) {
    sort_sequence(sort_sequence_desc_cmp);
}


// -----------------------------------------------------------------------------
/** Gets length of sequence

(seq -- seq int)
*/
// -----------------------------------------------------------------------------
static void EC_len(gpointer gp_entry) {
    const Param *param_seq = top();

    GSequence *seq = param_seq->val_custom;
    push_param(new_int_param(g_sequence_get_length(seq)));
}


static void EC_pop_seq(gpointer gp_entry) {
    Param *param_seq = pop_param();
    g_sequence_free(param_seq->val_custom);
    free_param(param_seq);
}


// -----------------------------------------------------------------------------
/** Defines the sequence lexicon

- ascending (seq sort-word -- seq-sorted) Sorts seq in ascending order
- descending (seq sort-word -- seq-sorted) Sorts seq in descending order

*/
// -----------------------------------------------------------------------------
void EC_add_sequence_lexicon(gpointer gp_entry) {
    add_entry("ascending")->routine = EC_ascending;
    add_entry("descending")->routine = EC_descending;

    add_entry("len")->routine = EC_len;
    add_entry("pop-seq")->routine = EC_pop_seq;
}
