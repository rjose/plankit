
%{
int fileno(FILE *stream);
void handle_word(const char *word);
%}

DIGIT  [0-9]

%%

#.*\n                  /* Skip comments */

\"[^"]*\"              {return 'S';}

[[:space:]]+           /* Skip whitespace */

{DIGIT}+               {return 'I';}
{DIGIT}+"."{DIGIT}*    {return 'D';}
[^[:space:]]+          {return 'W';}

<<EOF>>                {return EOF;}


%%

// This is just to silence the unused function warnings
void use_unused() {
    input();
    yyunput(0, NULL);
}
