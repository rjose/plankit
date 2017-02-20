/** \file ext_notes.c
*/


static void EC_start_chunk(gpointer gp_entry) {
    Param *param = pop_param();
    printf("TODO: Store start chunk string (with timestamp): \"%s\"\n", param->val_string);
    free_param(param);
}


static void EC_middle_chunk(gpointer gp_entry) {
    Param *param = pop_param();
    printf("TODO: Store middle chunk string (with timestamp): \"%s\"\n", param->val_string);
    free_param(param);
}


static void EC_end_chunk(gpointer gp_entry) {
    Param *param = pop_param();
    printf("TODO: Store end chunk string (with timestamp): \"%s\"\n", param->val_string);
    free_param(param);
}


static void EC_generic_note(gpointer gp_entry) {
    Param *param = pop_param();
    printf("TODO: Store generic note string (with timestamp): \"%s\"\n", param->val_string);
    free_param(param);
}


static void EC_today(gpointer gp_entry) {
    printf("TODO: Print all the notes for today\n");
}


void EC_add_notes_lexicon(gpointer gp_entry) {
    add_entry("S")->routine = EC_start_chunk;
    add_entry("M")->routine = EC_middle_chunk;
    add_entry("N")->routine = EC_generic_note;
    add_entry("E")->routine = EC_end_chunk;
    add_entry("today")->routine = EC_today;
}

