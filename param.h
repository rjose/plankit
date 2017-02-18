/** \file param.h
*/

#pragma once


Param *new_param();
Param *new_int_param(gint64 val_int);
Param *new_double_param(gdouble val_double);
Param *new_entry_param(Entry *val_entry);
Param *new_routine_param(routine_ptr val_routine);
Param *new_pseudo_entry_param(const gchar *word, routine_ptr routine);
void print_param(Param *param);
void free_param(gpointer param);
