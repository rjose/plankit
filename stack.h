#pragma once

void push_token(Token token);
void push_param(Param *param);
Param *pop_param();

void create_stack();
void destroy_stack();
