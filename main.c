Token get_token() {
    Token result;
    result.type = yylex();
    result.word = yytext;
    return result;
}

void handle_error(gint error_type, Token token) {
    fprintf(stderr, "%s: %s\n", error_type_to_string(error_type), token.word);
    printf("TODO: Clear stacks and IC\n\n");

    _mode = 'E';
}


int main() {
    Entry *entry;
    gint error_type;

    build_dictionary();

    // Control loop
    while(1) {
        Token token = get_token();
        printf("Token (%c): %s\n", token.type, token.word);

        if (token.type == EOF) break;

        // If, executing...
        if (_mode == 'E') {
            entry = find_entry(token.word);
            if (entry) {
                // TODO: execute(entry);
                printf("TODO: Execute entry: %s\n", entry->word);
            }
            else {
                push_param(token);
            }
        }

        // ...otherwise, we're compiling
        else {
            printf("TODO: Compile word: %s\n", token.word);
        }

        error_type = setjmp(_error_jmp_buf);
        if (error_type != 0) {
            handle_error(error_type, token);
        }

    }

    // Clean up
    destroy_dictionary();
}
