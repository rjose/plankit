// =============================================================================
/** \file

\brief Defines main function and control loop.

A program that implements a Forth interpreter that is used to construct
application languages for Rino.

\mainpage

The design of this program is based on the unpublished book "Programming a
Problem-oriented-language" by Chuck Moore in 1970.

The main function sets up the initial dictionary and the main control loop that
basically gets a token from the input stream and executes it.

Errors are handled via a long jump.

*/
// =============================================================================


// -----------------------------------------------------------------------------
/** This is the programwide error handler.

It prints the error that occured and then resets the interpreter to an initial
state.

\param error_type: Indicates what kind of error occured.
\param token: Current token being executed/compiled.

*/
// -----------------------------------------------------------------------------
void handle_error(gint error_type, Token token) {
    fprintf(stderr, "%s: %s\n", error_type_to_string(error_type), token.word);

    // Reset stacks, _ip, and _mode
    _ip = NULL;
    clear_stack();
    clear_stack_r();

    _mode = 'E';
}



// -----------------------------------------------------------------------------
/** Sets up the interpreter and then runs the main control loop.
*/
// -----------------------------------------------------------------------------
int main() {
    Entry *entry;
    gint error_type;

    build_dictionary();
    create_stack();
    create_stack_r();

    // Control loop
    while(1) {
        Token token = get_token();

        if (token.type == EOF) break;

        // If, executing...
        if (_mode == 'E') {
            entry = find_entry(token.word);
            if (entry) {
                execute(entry);
            }
            else {
                push_token(token);
            }
        }

        // ...otherwise, we're compiling
        else {
            compile(token);
        }

        error_type = setjmp(_error_jmp_buf);
        if (error_type != 0) {
            handle_error(error_type, token);
        }

    }

    // Clean up
    destroy_dictionary();
    destroy_stack();
    destroy_stack_r();
    yylex_destroy();
}
