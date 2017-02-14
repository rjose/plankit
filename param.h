/** \file param.h
*/

#pragma once

typedef struct {
    gchar type;           /**< \brief Indicates type of Param (see \ref param_types "Param types") */

    gint64 val_int;       /**< \brief Integer value of an 'I' param */
    gdouble val_double;   /**< \brief Double value of a 'D' param */
    gchar *val_str;       /**< \brief String value of an 'S' param */
    gpointer val_entry;   /**< \brief Entry pointer value of an 'E' param */
} Param;


Param *new_param();
Param *new_int_param(gint64 val_int);
Param *new_double_param(gdouble val_double);
void free_param(gpointer param);
