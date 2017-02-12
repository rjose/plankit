/** \file
*/

#pragma once

// =============================================================================
// Defines
// =============================================================================
/**
Longest word we can have in an entry.
*/
#define MAX_WORD_LEN  128

// -----------------------------------------------------------------------------
// Error codes
// -----------------------------------------------------------------------------

/**
Indicates that we couldn't find the token in the dictionary.
*/
#define ERR_UNKNOWN_WORD  1


// =============================================================================
// External functions
// =============================================================================
extern int yylex();
extern FILE* yyin;
extern char *yytext;




// =============================================================================
// Global entities
// =============================================================================

/** Represents a token from the input stream.

*/
typedef struct {
    /** The type of the token is represented by the following characters:

        - 'W': Word
        - 'I': Integer
        - 'D': Double
        - 'S': String
        - EOF: End of input

        The type is determined by the lexer.
    */
    gchar type;

    /** Characters parsed from the input stream.
    */
    gchar word[MAX_WORD_LEN];
} Token;


extern GList *_dictionary;
extern gchar _mode;
extern jmp_buf _error_jmp_buf;

const gchar *error_type_to_string(gint error_type);
Token get_token();
