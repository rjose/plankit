#pragma once

typedef struct {
    gchar type;  // vals: i, d, s, e

    gint64 int_val;
    gdouble double_val;
    gchar *str_val;
    gpointer entry_val;
} Param;

