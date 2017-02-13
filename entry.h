#pragma once

typedef void (*entry_routine_ptr)(gpointer entry);

typedef struct {
    gchar word[MAX_WORD_LEN];
    GArray *params;  // Param[]
    entry_routine_ptr routine;
} Entry;


Entry *new_entry();
void execute(gpointer entry);
void free_entry(gpointer entry);
