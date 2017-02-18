/** \file ec_basic.h
*/

#pragma once

void EC_constant(gpointer gp_entry);
void EC_variable(gpointer gp_entry);
void EC_push_entry_address(gpointer gp_entry);
void EC_push_param0(gpointer gp_entry);
void EC_print_stack(gpointer gp_entry);
void EC_define(gpointer gp_entry);
void EC_execute(gpointer gp_entry);
void EC_end_define(gpointer gp_entry);

void EC_print_definition(gpointer gp_entry);
