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

*/
// =============================================================================


// -----------------------------------------------------------------------------
/** Sets up the interpreter and then runs the main control loop.
*/
// -----------------------------------------------------------------------------
int main() {
    Entry *entry;

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

    }

    // Clean up
    destroy_dictionary();
    destroy_stack();
    destroy_stack_r();
    yylex_destroy();
}
