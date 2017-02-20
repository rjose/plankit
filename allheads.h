/** \file allheads.h

\brief Includes all headers required by the C files in this project. This also
defines the key structs used throughout.

This is automatically included by the Makefile during the compiler invocation
for all files so that.
*/

#pragma once

#include <stdio.h>
#include <setjmp.h>
#include <stdlib.h>
#include <string.h>

#include <gsl/gsl_cdf.h>
#include <glib.h>

#define MAX_WORD_LEN  128       /**< \brief Longest entry word */


typedef void (*routine_ptr)(gpointer entry);  /**< \brief Function pointer type for the routine of an Entry */

/** \brief Structure of Dictionary entries
*/
typedef struct {
    gchar word[MAX_WORD_LEN];   /**< \brief Key used for Dictionary lookup */
    gshort immediate;           /**< \brief 1 if should be executed during compilation; 0 otherwise */
    GSequence *params;          /**< \brief Sequence of Param objects */
    routine_ptr routine;        /**< \brief Code to be run when Entry is executed */
} Entry;


/** \brief Structure of objects that go onto the stack or are part of an Entry

A Param struct has all possible fields for values that a parameter might have.
Its "type" is a character that indicates which field holds the parameter's value:

\anchor param_types

- 'I': Integer value
- 'D': Double value
- 'S': String value (*must* be dynamically allocated because it will be freed when the parameter is freed)
- 'E': Points to an Entry in _dictionary
- 'R': Routine pointer
- 'P': Pseudo entry

*/
typedef struct {
    gchar type;               /**< \brief Indicates type of Param (see \ref param_types "Param types") */

    gint64 val_int;           /**< \brief Integer value of an 'I' param */
    gdouble val_double;       /**< \brief Double value of a 'D' param */
    gchar *val_string;        /**< \brief String value of an 'S' param */
    gpointer val_entry;       /**< \brief Entry pointer value of an 'E' param */
    routine_ptr val_routine;  /**< \brief Routine ptr of an 'R' param */
    Entry val_pseudo_entry;   /**< \brief Pseudo Entry of a 'P' param */
} Param;



#include "error.h"
#include "globals.h"
#include "param.h"
#include "entry.h"
#include "dictionary.h"
#include "stack.h"
#include "return_stack.h"
#include "ec_basic.h"
#include "ext_notes.h"
