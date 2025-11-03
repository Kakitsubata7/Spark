%option bison-bridge reentrant noyywrap noyylineno
%x IN_COMMENT

%{

#include "parser.tab.hpp"
#include "frontend/lexer_utils.hpp"

using namespace Spark::FrontEnd;

extern YYSTYPE yylval;

%}

%%

\n {
    whenNewline(*static_cast<LexerState*>(yyextra));
}

[ \t\r]+ {
    consumeCharacters(*static_cast<LexerState*>(yyextra), static_cast<size_t>(yyleng));
}

([0-9_]*\.[0-9_]+) {
    return makeReal(std::string_view(yytext, yyleng), *static_cast<LexerState*>(yyextra));
}

([0-9_]+|0[bB][0-9_]+|0[oO][0-9_]+|0[xX][0-9A-Za-z_]+) {
    return makeInteger(std::string_view(yytext, yyleng), *static_cast<LexerState*>(yyextra));
}

[A-Za-z_][A-Za-z0-9_]* {
    return makeWord(std::string_view(yytext, yyleng), *static_cast<LexerState*>(yyextra));
}

"//"[^\n]* {
    return makeLineComment(std::string_view(yytext, yyleng).substr(2), *static_cast<LexerState*>(yyextra));
}

"+=" { return SPK_ADD_ASSIGN; }
"+" { return SPK_ADD; }

"->" { return SPK_ARROW; }
"-=" { return SPK_SUB_ASSIGN; }
"-" { return SPK_SUB; }

"*=" { return SPK_MUL_ASSIGN; }
"*" { return SPK_MUL; }

"/=" { return SPK_DIV_ASSIGN; }
"/" { return SPK_DIV; }

"%=" { return SPK_MOD_ASSIGN; }
"%" { return SPK_MOD; }

"~" { return SPK_BIT_NOT; }

"&&" { return SPK_LOG_AND; }
"&=" { return SPK_BIT_AND_ASSIGN; }
"&" { return SPK_BIT_AND; }

"||" { return SPK_LOG_OR; }
"|=" { return SPK_BIT_OR_ASSIGN; }
"|" { return SPK_BIT_OR; }

"^=" { return SPK_BIT_XOR_ASSIGN; }
"^" { return SPK_BIT_XOR; }

"<<=" { return SPK_BIT_SHL_ASSIGN; }
"<<" { return SPK_BIT_SHL; }
"<=" { return SPK_LE; }
"<" { return SPK_LT; }

">>=" { return SPK_BIT_SHR_ASSIGN; }
">>" { return SPK_BIT_SHR; }
">=" { return SPK_GE; }
">" { return SPK_GT; }

"!=" { return SPK_NE; }
"!" { return SPK_LOG_NOT; }

"=>" { return SPK_FAT_ARROW; }
"==" { return SPK_EQ; }
"=" { return SPK_ASSIGN; }

"..<" { return SPK_RANGE_EXCL; }
"..." { return SPK_RANGE; }
"." { return SPK_DOT; }

"," { return SPK_COMMA; }
":" { return SPK_COLON; }
";" { return SPK_SEMICOLON; }
"(" { return SPK_LPAREN; }
")" { return SPK_RPAREN; }
"[" { return SPK_LBRACKET; }
"]" { return SPK_RBRACKET; }
"{" { return SPK_LBRACE; }
"}" { return SPK_RBRACE; }

. {

}

%%
