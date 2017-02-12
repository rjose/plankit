#pragma once

typedef void (*entry_routine_ptr)(gpointer dict_entry);

typedef struct {
    gchar word[MAX_WORD_LEN];
    GArray *params;  // Param[]
    entry_routine_ptr routine;   // What to execute for entry
} Entry;


void free_entry(gpointer entry);
