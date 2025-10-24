%{

%}

%option yylineno c++

%%
[[:alpha:]_][[:alnum:]_]*

(\-?)[[:digit:]_]+

0[bB][01_]+

0[oO][01234567_]+

0[xX][[:xdigit:]_]+

[ \t\n]+    

.
%%
