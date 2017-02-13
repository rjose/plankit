#pragma once

void build_dictionary();
Entry *add_entry(const gchar *word);
Entry* find_entry(const gchar* word);
void destroy_dictionary();
