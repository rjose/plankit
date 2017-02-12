
%{
void handle_word(const char *word);
%}

%%

[[:space:]]+           /* Skip whitespace */

[^[:space:]]+          handle_word(yytext);


%%

