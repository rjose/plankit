extern int yylex();
extern FILE* yyin;

#define MAX_WORD_LEN 128

typedef struct {
    gchar type;  // vals: i, d, s, e

    gint64 int_val;
    gdouble double_val;
    gchar *str_val;
    gpointer entry_val;
} EntryParam;

typedef void (*entry_routine_ptr)(gpointer dict_entry);

typedef struct {
    gchar word[MAX_WORD_LEN];
    GArray *params;  // EntryParams
    entry_routine_ptr routine;   // What to execute for entry
} DictEntry;


// Task: Make a constant
//     Create parameter stack
//     Create return stack
//     Return value from rules in lexer so it stops parsing at each word
//     Change our main loop to one that reads tokens in a loop
//     Implement make_constant
//     Create dict entry for CONSTANT
//     Hook make constant up to dict entry



// Task: Make DictEntry a complete entity
//     Should create dict_entry.[ch] files
//     Create a DictEntry
//     Free a DictEntry


void make_constant(DictEntry *entry) {
    // . Read next word from input
    // . Create new entry
    // . Pop value from stack and add to entries param
    // . Set entry's routine to add_entry_param0_to_stack
}


GList *G_dictionary = NULL;



DictEntry* find_entry(const char* word) {
    for (GList* l=g_list_last(G_dictionary); l != NULL; l = l->prev) {
        DictEntry *entry = l->data;
        if (g_strcmp0(word, entry->word) == 0) return entry;
    }
    return NULL;
}


// This is the main entry point from the interpreter
void handle_word(const char *word) {
    DictEntry *entry = find_entry(word);
    if (entry == NULL) {
        printf("TODO: Handle word not found: %s\n", word);
    }
    else {
        printf("Found entry: %s\n", entry->word);
    }
}


// -----------------------------------------------------------------------------
// Builds initial dictionary
//
// NOTE: Entries are allocated and so it's appropriate to do a g_list_free_full
//       if the dictionary should be rebuilt.
// -----------------------------------------------------------------------------
void build_dictionary() {
    DictEntry *entry;

    entry = g_new(DictEntry, 1);
    g_strlcpy(entry->word, ":", MAX_WORD_LEN);
    G_dictionary = g_list_append(G_dictionary, entry);

    entry = g_new(DictEntry, 1);
    g_strlcpy(entry->word, ";", MAX_WORD_LEN);
    G_dictionary = g_list_append(G_dictionary, entry);
}


int main() {
    build_dictionary();
    yylex();
}
