/** \file return_stack.c

\brief Defines functions for creating, manipulating, and freeing the return stack.

The return stack is used to keep track of execution stack frames when definitions
are executed.

*/


// -----------------------------------------------------------------------------
/** Creates a new return stack
*/
// -----------------------------------------------------------------------------
void create_stack_r() {
    _return_stack = g_queue_new();
}


// -----------------------------------------------------------------------------
/** Frees memory for a return stack
*/
// -----------------------------------------------------------------------------
void destroy_stack_r() {
    g_queue_free(_return_stack);
}


// -----------------------------------------------------------------------------
/** Pushes a param pointer onto the return stack.

*/
// -----------------------------------------------------------------------------
void push_param_r(GSequenceIter* param) {
    g_queue_push_tail(_return_stack, param);
}


// -----------------------------------------------------------------------------
/** Pops a parameter off the return stack.

*/
// -----------------------------------------------------------------------------
GSequenceIter *pop_param_r() {
    return g_queue_pop_tail(_return_stack);
}