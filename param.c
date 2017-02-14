/** \file param.c

\brief Functions for creating and destroying Param objects.

A Param can be pushed onto the parameter stack. They can also be added to the
params array of an Entry.

Each Param can represent different types of data. The type of data is indicated
by the type field:

\anchor param_types

- 'I': Integer value
- 'D': Double value
- 'S': String value (*must* be dynamically allocated)
- 'E': Points to an Entry in _dictionary (*must not* be dynamically allocated)

*/


// -----------------------------------------------------------------------------
/** Creates a new Param.

\returns newly allocated Param
*/
// -----------------------------------------------------------------------------
Param *new_param() {
    Param *result = g_new(Param, 1);
    return result;
}


// -----------------------------------------------------------------------------
/** Creates a new int-valued Param.

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
/** Creates a new double-valued Param.

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
/** Frees memory for a param.

\param gp_param: Pointer to a Param to free.
*/
// -----------------------------------------------------------------------------
void free_param(gpointer gp_param) {
    Param *param = gp_param;
    g_free(param->val_str);
    g_free(param);
}
