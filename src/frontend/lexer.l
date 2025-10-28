%{
#include "lexer_utils.hpp"

using namespace Spark::Frontend;
%}

%option noyywrap yylineno c++

%%
[[:alpha:]_][[:alnum:]_]*   { return classifyWord(yytext); }

(\-?)[[:digit:]_]+          { return  }

0[bB][01_]+                 {}

0[oO][01234567_]+           {}

0[xX][[:xdigit:]_]+         {}

[ \t\n]+

.
%%
