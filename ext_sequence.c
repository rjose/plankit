/** \file sequence.c

\brief Defines words for operating on sequences

*/


static Param *get_sort_value(gconstpointer const_obj, Entry *sort_entry) {
    gpointer obj = (gpointer) const_obj;

    push_param(new_custom_param(obj, "?"));    // (obj)
    execute(sort_entry);                       // (obj val)
    Param *result = pop_param();               // (obj)
    free_param(pop_param());                   // ()
    return result;
}


static gint sort_sequence_asc_cmp(gconstpointer l, gconstpointer r, gpointer gp_sort_entry) {
    Entry *sort_entry = gp_sort_entry;
    Param *param_l_val = get_sort_value(l, sort_entry);
    Param *param_r_val = get_sort_value(r, sort_entry);

    gint result = 0;

    if (param_l_val->type == 'I') {
        if (param_l_val->val_int == param_r_val->val_int) {
            result = 0;
        }
        else {
            result = param_l_val->val_int < param_r_val->val_int ? -1 : 1;
        }
    }
    else if (param_l_val->type == 'D') {
        if (param_l_val->val_double == param_r_val->val_double) {
            result = 0;
        }
        else {
            result = param_l_val->val_double < param_r_val->val_double ? -1 : 1;
        }
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


static gint sort_sequence_desc_cmp(gconstpointer l, gconstpointer r, gpointer gp_sort_entry) {
    return -sort_sequence_asc_cmp(l, r, gp_sort_entry);
}


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



static void  EC_ascending(gpointer gp_entry) {
    sort_sequence(sort_sequence_asc_cmp);
}

static void  EC_descending(gpointer gp_entry) {
    sort_sequence(sort_sequence_desc_cmp);
}


void EC_add_sequence_lexicon(gpointer gp_entry) {
    add_entry("ascending")->routine = EC_ascending;
    add_entry("descending")->routine = EC_descending;
}
