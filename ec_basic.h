/** \file ec_basic.h
*/

#pragma once

void EC_quit(gpointer gp_entry);
void EC_interactive(gpointer gp_entry);

void EC_constant(gpointer gp_entry);
void EC_variable(gpointer gp_entry);

void EC_store_variable_value(gpointer gp_entry);
void EC_fetch_variable_value(gpointer gp_entry);

void EC_push_entry_address(gpointer gp_entry);
void EC_push_param0(gpointer gp_entry);

void EC_print_stack(gpointer gp_entry);
void EC_print_definition(gpointer gp_entry);

void EC_if(gpointer gp_entry);
void EC_then(gpointer gp_entry);
void EC_else(gpointer gp_entry);

void EC_jmp_if_false(gpointer gp_entry);
void EC_jmp(gpointer gp_entry);

void EC_define(gpointer gp_entry);
void EC_execute(gpointer gp_entry);
void EC_end_define(gpointer gp_entry);

void add_variable(const gchar *word);
void find_and_execute(const gchar *word);
