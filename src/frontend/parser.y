%require "3.5"

%skeleton "lalr1.cc"

%language "c++"
%define api.value.type {TokenValue}
%define api.token.raw

%lex-param { yyscan_t scanner }
%parse-param { yyscan_t scanner } { ParserContext& ctx }

%code requires {
#include "frontend/semantic_value.hpp"
#include "frontend/parser_context.hpp"

using namespace Spark::FrontEnd;

typedef void* yyscan_t;
}

%code {
int yylex(TokenValue* yylval, yyscan_t scanner);
void yyerror(yyscan_t scanner, ParserContext& ctx, const char* msg);
}

%token Identifier Discard
%token Integer Real String
%token Alias As Break Case Catch Class Const Constructor Continue Cref Destructor Do Else End Enum Export Extension False Fn For From Global If Import In Is Let Match Module Nil Operator Ref Return Self Struct Super Then Throw Trait True Try Typeof Undefined While Yield
%token Add Sub Mul Div Mod
%token BitNot BitAnd BitOr BitXor BitShl BitShr
%token LogNot LogAnd LogOr
%token Eq Ne Lt Gt Le Ge
%token StrictEq StrictNe
%token Range RangeExcl
%token Question Coalesce
%token Assign AddAssign SubAssign MulAssign DivAssign ModAssign BitAndAssign BitOrAssign BitXorAssign BitShlAssign BitShrAssign CoalesceAssign
%token Dot
%token Comma Colon Arrow FatArrow
%token Semicolon LParen RParen LBracket RBracket LBrace RBrace
%token At
%token Dollar
%token LineComment BlockComment

%token Error
%token EndOfFile 0

%%
%start program;

program:

%%
