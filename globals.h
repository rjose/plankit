/** \file globals.h
*/

#pragma once

// =============================================================================
// Defines
// =============================================================================
// -----------------------------------------------------------------------------
// Error codes
// -----------------------------------------------------------------------------

#define ERR_UNKNOWN_WORD  1
#define ERR_UNKNOWN_TOKEN_TYPE  2
#define ERR_STACK_UNDERFLOW  3
#define ERR_INVALID_PARAM  4


// =============================================================================
// External functions
// =============================================================================
extern int yylex();          /**< \brief Gets next token from the input stream */
extern int yylex_destroy();  /**< \brief Gets next token from the input stream */
extern FILE* yyin;           /**< \brief Points to the input stream */
extern char *yytext;         /**< \brief Current token */




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
extern GQueue *_stack;
extern GQueue *_return_stack;
extern gchar _mode;
extern jmp_buf _error_jmp_buf;
extern GSequenceIter *_ip;

const gchar *error_type_to_string(gint error_type);
Token get_token();
void handle_error(gint error_type);
