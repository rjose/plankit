GList *_dictionary = NULL;
gchar _mode = 'E';

jmp_buf _error_jmp_buf;

static gchar unknown_word[] = "Unknown word";
static gchar unknown_error[] = "Unknown error";


const gchar *error_type_to_string(gint error_type) {
    gchar *result;
    switch(error_type) {
        case ERR_UNKNOWN_WORD:
            result = unknown_word;
            break;

        default:
            result = unknown_error;
            break;
    }
    return result;
}

Token get_token() {
    Token result;
    result.type = yylex();
    result.word = yytext;
    return result;
}
