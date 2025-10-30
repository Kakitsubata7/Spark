%{
#include "parser.tab.h"
#include "frontend/lexer_utils.hpp"

extern YYSTYPE yylval;
%}

%option noyywrap yylineno

%%
[[:alpha:]_][[:alnum:]_]*   { return  }

(\-?)[[:digit:]_]+          { return spk_(); }

0[bB][01_]+

0[oO][01234567_]+

0[xX][[:xdigit:]_]+

[ \t\r\n]+

.
%%
