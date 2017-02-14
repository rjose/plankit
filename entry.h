/** \file entry.h
*/

#pragma once

typedef void (*entry_routine_ptr)(gpointer entry);

typedef struct {
    gchar word[MAX_WORD_LEN];   /**< \brief Key used for Dictionary lookup */
    GArray *params;             /**< \brief Array of Param objects */
    entry_routine_ptr routine;  /**< \brief Code to be run when Entry is executed */
} Entry;


Entry *new_entry();
void add_entry_param(Entry *entry, Param param);
void execute(gpointer entry);
void free_entry(gpointer entry);
