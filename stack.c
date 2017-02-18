/** \file stack.c

\brief Defines functions for creating, manipulating, and freeing the parameter stack.

The parameter stack is used to pass arguments and results between words and entry
routines.
*/


// -----------------------------------------------------------------------------
/** Creates a new stack
*/
// -----------------------------------------------------------------------------
void create_stack() {
    _stack = g_queue_new();
}

static void _free_param(gpointer param, gpointer unused) {
    free_param(param);
}

void clear_stack() {
    g_queue_foreach(_stack, _free_param, NULL);
    g_queue_clear(_stack);
}

// -----------------------------------------------------------------------------
/** Frees memory for a stack
*/
// -----------------------------------------------------------------------------
void destroy_stack() {
    g_queue_free(_stack);
}


// -----------------------------------------------------------------------------
/** Converts a non-Word token into a Param and pushes it onto the param stack.

\param token: Token to convert and push
*/
// -----------------------------------------------------------------------------
void push_token(Token token) {
    // Can't push a Word token
    if (token.type == 'W') {
        longjmp(_error_jmp_buf, ERR_UNKNOWN_WORD);
        return;  // Not really needed...
    }

    Param *param_new;
    gint64 val_int;
    gdouble val_double;

    switch(token.type) {
        case 'I':
            val_int = g_ascii_strtoll(token.word, NULL, 10);
            param_new = new_int_param(val_int);
            push_param(param_new);
            break;

        case 'D':
            val_double = g_ascii_strtod(token.word, NULL);
            param_new = new_double_param(val_double);
            push_param(param_new);
            break;

        default:
            longjmp(_error_jmp_buf, ERR_UNKNOWN_TOKEN_TYPE);
            break;
    }
}


// -----------------------------------------------------------------------------
/** Pushes a param onto the stack.

*/
// -----------------------------------------------------------------------------
void push_param(Param* param) {
    g_queue_push_tail(_stack, param);
}


// -----------------------------------------------------------------------------
/** Pops a parameter off the stack.

\note The caller of this is responsible for freeing the param when done with it.
*/
// -----------------------------------------------------------------------------
Param *pop_param() {
    return g_queue_pop_tail(_stack);
}
