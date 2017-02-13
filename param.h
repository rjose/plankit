#pragma once

typedef struct {
    gchar type;  // vals: i, d, s, e

    gint64 val_int;
    gdouble val_double;
    gchar *val_str;
    gpointer val_entry;
} Param;


Param *new_param();
Param *new_int_param(gint64 val_int);
Param *new_double_param(gdouble val_double);
void free_param(gpointer param);
