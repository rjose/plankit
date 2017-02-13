/** \file
*/

Param *new_param() {
    Param *result = g_new(Param, 1);
    return result;
}


Param *new_int_param(gint64 val_int) {
    Param *result = new_param();
    result->type = 'I';
    result->val_int = val_int;
    return result;
}


Param *new_double_param(gdouble val_double) {
    Param *result = new_param();
    result->type = 'D';
    result->val_double = val_double;
    return result;
}


void free_param(gpointer param) {
    g_free(param);
}
