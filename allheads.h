/** \file allheads.h

\brief Includes union of all headers required by the C files in this project.

This is automatically included by the Makefile during the compiler invocation
for all files so that we don't have to include any headers in any file.
*/

#pragma once

#include <stdio.h>
#include <setjmp.h>
#include <stdlib.h>

#include <gsl/gsl_cdf.h>
#include <glib.h>

#include "error.h"
#include "globals.h"
#include "param.h"
#include "entry.h"
#include "dictionary.h"
#include "stack.h"
#include "ec_basic.h"
