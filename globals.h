#pragma once

// Defines
#define MAX_WORD_LEN  128
#define ERR_UNKNOWN_WORD  1

extern int yylex();
extern FILE* yyin;
extern char *yytext;

typedef struct {
    gchar type;  // W, I, D, S
    const gchar *word;
} Token;


extern GList *_dictionary;
extern gchar _mode;
extern jmp_buf _error_jmp_buf;

const gchar *error_type_to_string(gint error_type);
Token get_token();
