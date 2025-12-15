%require "3.5"

%skeleton "lalr1.cc"

%language "c++"

%define api.token.raw
%define api.value.type variant
%define parse.error verbose

%locations

%lex-param {yyscan_t scanner}
%parse-param {yyscan_t scanner} {yy::parser::location_type* yylloc} {Spark::FrontEnd::ParserContext& ctx}

%code requires {
#include <vector>

#include "frontend/ast.hpp"
#include "frontend/lexer/lexer_state.hpp"
#include "frontend/lexer/token_value.hpp"
#include "frontend/parser/parser_context.hpp"

typedef void* yyscan_t;
}

%code {
using namespace Spark;
using namespace Spark::FrontEnd;

int yylex(yy::parser::semantic_type*, yy::parser::location_type*, yyscan_t);

inline void raiseError(yy::parser& parser, Location start, Location end, const std::string& msg) {
    yy::position b;
    b.line = start.lineno;
    b.column = start.columnno;
    yy::position e;
    e.line = end.lineno;
    e.column = end.columnno;
    parser.error(yy::location(b, e), msg);
}

#define ast ctx.ast()
#define RAISE_ERROR(start, end, msg) raiseError(*this, (start), (end), (msg));
}

%token <Spark::FrontEnd::TokenValue> Identifier Discard
%token <Spark::FrontEnd::TokenValue> Integer Real String
%token <Spark::FrontEnd::TokenValue> Alias As Break Case Catch Class Const Constructor Continue Cref Destructor Do Else End Enum Export Extension False Fn For From Global If Import In Is Let Match Module Nil Operator Ref Return Self Struct Super Then Throw Trait True Try Typeof Undefined While Yield
%token <Spark::FrontEnd::TokenValue> Add Sub Mul Div Mod
%token <Spark::FrontEnd::TokenValue> Tide And VBar Caret
%token <Spark::FrontEnd::TokenValue> Bang LogAnd LogOr
%token <Spark::FrontEnd::TokenValue> Eq Ne Lt Le Gt Ge
%token <Spark::FrontEnd::TokenValue> StrictEq StrictNe
%token <Spark::FrontEnd::TokenValue> Shl Shr
%token <Spark::FrontEnd::TokenValue> Range RangeExcl
%token <Spark::FrontEnd::TokenValue> Question Coalesce
%token <Spark::FrontEnd::TokenValue> Pipe
%token <Spark::FrontEnd::TokenValue> Assign AddAssign SubAssign MulAssign DivAssign ModAssign BitAndAssign BitOrAssign BitXorAssign CoalesceAssign
%token <Spark::FrontEnd::TokenValue> Dot
%token <Spark::FrontEnd::TokenValue> Comma Colon Arrow FatArrow
%token <Spark::FrontEnd::TokenValue> Semicolon LParen RParen LBracket RBracket LBrace RBrace LABracket RABracket
%token <Spark::FrontEnd::TokenValue> At
%token <Spark::FrontEnd::TokenValue> Dollar
%token <Spark::FrontEnd::TokenValue> LineComment BlockComment

%token EndOfFile 0
%token Error 1

%%
%start program;

program:
      terms
    ;

terms:
      /* empty */
    | terms term

term:
      expr
    | prattop
    | delimiter
    | keyword
    | vardef
    | Fn
    | typedef
    ;

expr:
      Identifier
    | Discard
    | Dollar Identifier
    | Dollar Discard
    | Identifier template
    | Discard template
    | Dollar Identifier template
    | Dollar Discard template
    | literal
    | LParen comma_terms RParen
    | LBracket comma_terms RBracket
    | LBrace terms RBrace
    | Constructor
    | Destructor
    | Operator operator
    | Global
    | Self
    | Super
    ;

comma_terms:
      terms
    | comma_terms Comma terms
    ;

template:
      LABracket template_terms RABracket
    ;

template_terms:
      terms
    | template_terms Comma terms
    ;

/**
  * Overloadable Operator
  */
operator:
      Add
    | Sub
    | Mul
    | Div
    | Mod
    | Tide
    | And
    | VBar
    | Caret
    | Bang
    | LogAnd
    | LogOr
    | Eq
    | Ne
    | Lt
    | Le
    | Gt
    | Ge
    | Assign
    | AddAssign
    | SubAssign
    | MulAssign
    | DivAssign
    | ModAssign
    | BitAndAssign
    | BitOrAssign
    | BitXorAssign
    | LParen RParen
    | LBracket RBracket
    ;
    
literal:
      Integer
    | Real
    | True
    | False
    | String
    | Nil
    | Undefined
    ;

/**
  * Pratt Operations
  */
prattop:
      Add
    | Sub
    | Mul
    | Div
    | Mod
    | Tide
    | And
    | VBar
    | Caret
    | Bang
    | LogAnd
    | LogOr
    | Eq
    | Ne
    | Lt
    | Le
    | Gt
    | Ge
    | StrictEq
    | StrictNe
    | Shl
    | Shr
    | Range
    | RangeExcl
    | Question
    | Coalesce
    | Pipe
    | Is
    | Dot
    | Colon
    | Arrow
    | Assign
    | AddAssign
    | SubAssign
    | MulAssign
    | DivAssign
    | ModAssign
    | BitAndAssign
    | BitOrAssign
    | BitXorAssign
    | CoalesceAssign
    ;

delimiter:
      FatArrow
    | Semicolon
    ;

keyword:
      Break
    | Case
    | Catch
    | Continue
    | Do
    | Else
    | Export
    | For
    | From
    | If
    | Import
    | In
    | Match
    | Module
    | Return
    | Then
    | Throw
    | Try
    | Typeof
    | While
    ;

vardef:
      Let
    | Const
    | Ref
    | Cref
    ;

typedef:
      Struct
    | Class
    | Enum
    | Alias
    | Extension
    ;
%%

void yy::parser::error(const yy::location& loc, const std::string& msg) {
    Location start(static_cast<size_t>(loc.begin.line), static_cast<size_t>(loc.begin.column));
    Location end(static_cast<size_t>(loc.end.line), static_cast<size_t>(loc.end.column));
    ctx.addError(msg, start, end);
}
