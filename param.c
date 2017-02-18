/** \file param.c

\brief Functions for creating and destroying Param objects.

A Param can be pushed onto the parameter stack. They can also be added to the
params array of an Entry.

See \ref param_types "Param types" for a description of each type of parameter.
*/


// -----------------------------------------------------------------------------
/** \brief Creates a new Param.

\returns newly allocated Param
*/
// -----------------------------------------------------------------------------
Param *new_param() {
    Param *result = g_new(Param, 1);
    result->val_str = NULL;
    return result;
}


// -----------------------------------------------------------------------------
/** \brief Creates a new int-valued Param.

\param val_int: Value of the Param being created
\returns newly allocated Param with the specified int value
*/
// -----------------------------------------------------------------------------
Param *new_int_param(gint64 val_int) {
    Param *result = new_param();
    result->type = 'I';
    result->val_int = val_int;
    return result;
}


// -----------------------------------------------------------------------------
/** \brief Creates a new double-valued Param.

\param val_double: Value of the Param being created
\returns newly allocated Param with the specified double value
*/
// -----------------------------------------------------------------------------
Param *new_double_param(gdouble val_double) {
    Param *result = new_param();
    result->type = 'D';
    result->val_double = val_double;
    return result;
}


// -----------------------------------------------------------------------------
/** \brief Creates a new routine-valued Param.

\param val_routine: Value of the Param being created
\returns newly allocated Param with the specified routine value
*/
// -----------------------------------------------------------------------------
Param *new_routine_param(routine_ptr val_routine) {
    Param *result = new_param();
    result->type = 'R';
    result->val_routine = val_routine;
    return result;
}


// -----------------------------------------------------------------------------
/** \brief Creates a new entry-valued Param.

\param val_routine: Value of the Param being created
\returns newly allocated Param with the specified entry value

This is typically used when creating new dictionary entries.
*/
// -----------------------------------------------------------------------------
Param *new_entry_param(Entry *val_entry) {
    Param *result = new_param();
    result->type = 'E';
    result->val_entry = val_entry;
    return result;
}


// -----------------------------------------------------------------------------
/** \brief Creates a new pseudoentry-valued Param.

\param val_routine: Value of the Param being created
\returns newly allocated Param with the specified pseudoentry value

This is used during the compilation of definitions. Pseudo entries are used
to implement branching during a definition as well as doing things like pushing
constants from a definition onto the param stack.
*/
// -----------------------------------------------------------------------------
Param *new_pseudo_entry_param(const gchar *word, routine_ptr routine) {
    Param *result = new_param();
    result->type = 'P';
    g_strlcpy(result->val_pseudo_entry.word, word, MAX_WORD_LEN);
    result->val_pseudo_entry.routine = routine;
    return result;
}


void copy_param(Param *dst, Param *src) {
    *dst = *src;

    // Make a copy of the string since the dst needs to own it
    dst->val_str = g_strdup(src->val_str);
}


// -----------------------------------------------------------------------------
/** \brief Prints a parameter

\param param: Param to print

*/
// -----------------------------------------------------------------------------
void print_param(Param *param) {
    Entry *entry;

    switch (param->type) {
        case 'I':
            printf("I: %ld\n", param->val_int);
            break;

        case 'D':
            printf("D: %lf\n", param->val_double);
            break;

        case 'S':
            printf("S: %s\n", param->val_str);
            break;

        case 'E':
            entry = param->val_entry;
            printf("E: %s\n", entry->word);
            break;

        case 'R':
            printf("R: %ld\n", (gint64) param->val_routine);
            break;

        case 'P':
            printf("P: %s\n", param->val_pseudo_entry.word);
            break;
    }
}


// -----------------------------------------------------------------------------
/** \brief Frees memory for a param.

\param gp_param: Pointer to a Param to free.
*/
// -----------------------------------------------------------------------------
void free_param(gpointer gp_param) {
    Param *param = gp_param;
    g_free(param->val_str);
    g_free(param);
}
