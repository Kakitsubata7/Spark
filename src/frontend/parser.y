%require "3.5"

%language "c++"
%define api.value.type {TokenValue}
%define api.token.raw

%lex-param { yyscan_t scanner }
%parse-param { yyscan_t scanner }

%code requires {
#include "frontend/token_value.hpp"

using namespace Spark::FrontEnd;

typedef void* yyscan_t;
}

%code {
int yylex(TokenValue* yylval, yyscan_t scanner);
void yyerror(yyscan_t, const char* msg);
}

%token Identifier Discard
%token Integer Real String
%token Alias As Break Case Catch Class Const Constructor Continue Cref Destructor Do Else End Enum Export Extension False Fn For From Global If Import In Is Let Match Module Nil Operator Ref Return Self Struct Super Then Throw Trait True Try Typeof Undefined While Yield
%token Add Sub Mul Div Mod
%token BitNot BitAnd BitOr BitXor BitShl BitShr
%token LogNot LogAnd LogOr
%token Eq Ne Lt Gt Le Ge
%token Assign AddAssign SubAssign MulAssign DivAssign ModAssign BitAndAssign BitOrAssign BitXorAssign BitShlAssign BitShrAssign
%token Dot
%token Comma Colon Arrow FatArrow
%token Semicolon LParen RParen LBracket RBracket LBrace RBrace
%token Range RangeExcl
%token At
%token Dollar
%token LineComment BlockComment

%token Error
%token EndOfFile 0

%%
start:
;
%%
