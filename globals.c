/** \file
*/

// =============================================================================
// Globals
// =============================================================================

/** Global forth dictionary
*/
GList *_dictionary = NULL;


/** Global parameter stack
*/
GQueue *_stack = NULL;


/** Global return stack
*/
GQueue *_return_stack = NULL;


/** Global interpreter mode. The legal values are:

    - E: Execution mode (normal)
    - C: Compilation mode (during word definition)
*/
gchar _mode = 'E';


/** Global jump buffer used for long jumping to the error handler.
*/
jmp_buf _error_jmp_buf;


// =============================================================================
// Error reporting
// =============================================================================

// Error strings
static gchar unknown_word[] = "Unknown word";
static gchar unknown_error[] = "Unknown error";
static gchar unknown_token_type[] = "Unknown token type";


// -----------------------------------------------------------------------------
/** Returns an error string for the given error code.

\param error_type  An integer error code
\returns A pointer to a static error string

*/
// -----------------------------------------------------------------------------
const gchar *error_type_to_string(gint error_type) {
    gchar *result;
    switch(error_type) {
        case ERR_UNKNOWN_WORD:
            result = unknown_word;
            break;

        case ERR_UNKNOWN_TOKEN_TYPE:
            result = unknown_token_type;
            break;

        default:
            result = unknown_error;
            break;
    }
    return result;
}


// -----------------------------------------------------------------------------
/** Returns the next token from the input stream. If at the EOF, this returns a
token with type equal to EOF.


\returns A Token representing the next token
*/
// -----------------------------------------------------------------------------
Token get_token() {
    Token result;
    result.type = yylex();
    g_strlcpy(result.word, yytext, MAX_WORD_LEN);
    return result;
}
