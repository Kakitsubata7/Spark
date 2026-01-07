%require "3.5"

%skeleton "lalr1.cc"

%language "c++"

%define api.token.raw
%define api.value.type variant
%define parse.error verbose

%locations

%lex-param {yyscan_t scanner}
%parse-param {yyscan_t scanner} {yy::parser::location_type* yylloc}

%code requires {
#include <vector>

#include "frontend/lexer/token_value.hpp"

typedef_stmt void* yyscan_t;
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

#define RAISE_ERROR(start, end, msg) raiseError(*this, (start), (end), (msg));
}

%token <Spark::FrontEnd::TokenValue> Identifier Discard
%token <Spark::FrontEnd::TokenValue> Integer Real String
%token <Spark::FrontEnd::TokenValue> Alias As Break Case Catch Class Const Constructor Continue Cref Destructor Do Else End Enum Export Extension False Fn For From Global If Import In Is Let Match Module Nil Operator Ref Return Self Struct Super Then Throw Trait True Try Typeof Undefine While
%token <Spark::FrontEnd::TokenValue> Add Sub Mul Div Mod
%token <Spark::FrontEnd::TokenValue> Tide And VBar Caret
%token <Spark::FrontEnd::TokenValue> Bang LogAnd LogOr
%token <Spark::FrontEnd::TokenValue> Eq Ne StrictEq StrictNe Lt Le Gt Ge
%token <Spark::FrontEnd::TokenValue> Shl Shr
%token <Spark::FrontEnd::TokenValue> Range RangeExcl
%token <Spark::FrontEnd::TokenValue> Backtick Question Coalesce
%token <Spark::FrontEnd::TokenValue> Pipe
%token <Spark::FrontEnd::TokenValue> Assign AddAssign SubAssign MulAssign DivAssign ModAssign BitAndAssign BitOrAssign BitXorAssign CoalesceAssign
%token <Spark::FrontEnd::TokenValue> Dot
%token <Spark::FrontEnd::TokenValue> Comma Colon Arrow FatArrow
%token <Spark::FrontEnd::TokenValue> Semicolon Newline LParen RParen LBracket RBracket LBrace RBrace
%token <Spark::FrontEnd::TokenValue> At
%token <Spark::FrontEnd::TokenValue> Dollar
%token <Spark::FrontEnd::TokenValue> LineComment BlockComment

%token EndOfFile 0
%token Error 1

%right Assign AddAssign SubAssign MulAssign DivAssign ModAssign BitAndAssign BitOrAssign BitXorAssign
%right CoalesceAssign

%%
%start program;

program:
      stmts
    ;

stmts:
      /* empty */
    | stmt_list
    | stmt_list terminators
    ;

stmt_list:
      stmt
    | stmt_list terminators stmt
    ;

terminator:
      Newline
    | Semicolon
    ;

terminators:
      terminator
    | terminators terminator
    ;

stmt:
      binding
    | assign_stmt
    | fndef_stmt
    | typedef_stmt
    | enumcase_stmt
    | if_stmt
    | While expr block
    | For expr In expr block
    | Break
    | Continue
    | Return
    | Return expr
    | Module postfix block
    | Export stmt
    | import_stmt
    | Undefine postfix
    | expr
    ;

assign_stmt:
      pattern Assign expr
    | pattern Assign assign_stmt
    | pattern AddAssign expr
    | pattern AddAssign assign_stmt
    | pattern SubAssign expr
    | pattern SubAssign assign_stmt
    | pattern MulAssign expr
    | pattern MulAssign assign_stmt
    | pattern DivAssign expr
    | pattern DivAssign assign_stmt
    | pattern ModAssign expr
    | pattern ModAssign assign_stmt
    | pattern BitAndAssign expr
    | pattern BitAndAssign assign_stmt
    | pattern BitOrAssign expr
    | pattern BitOrAssign assign_stmt
    | pattern CoalesceAssign expr
    | pattern CoalesceAssign assign_stmt
    ;

fndef_stmt:
      fn fn_name params captures fn_ret fn_throw block
    | fn fn_name params captures fn_ret fn_throw FatArrow expr
    ;

fn_name:
      identifier
    ;

fn:
      Fn
    | fn Backtick
    ;

params:
      LParen RParen
    | LParen param_list RParen
    ;

param_list:
      param
    | param_list Comma param
    ;

param:
      identifier
    | identifier Colon type
    | varmod identifier
    | varmod identifier Colon type
    ;

captures:
      LBracket RBracket
    | LBracket capture_list RBracket
    ;

capture_list:
      capture
    | capture_list Comma capture
    ;

capture:
      postfix
    | varmod postfix
    | Range
    | varmod Range
    ;

fn_ret:
      /* empty */
    | Arrow type
    | Arrow Ref type
    | Arrow Cref type
    ;

fn_throw:
      /* empty */
    | Throw
    | Throw type
    ;

typedef_stmt:
      typemod Identifier block
    | typemod Identifier Colon exprs block
    ;

enumcase_stmt:
      Case identifier
    | Case identifier Assign expr
    | Case adt_constructor
    ;

adt_constructor:
      identifier LParen RParen
    | identifier LParen positional_adt_members RParen
    | identifier LParen named_adt_members RParen
    ;

positional_adt_members:
      type
    | positional_adt_members Comma type
    ;

named_adt_members:
      named_adt_member
    | named_adt_members Comma named_adt_member
    ;

named_adt_member:
      identifier Colon type
    ;

if_stmt:
      If expr block elseif_stmts
    | If expr block elseif_stmts Else block
    ;

elseif_stmts:
      /* empty */
    | elseif_stmts Else If expr block
    ;

typemod:
      Struct
    | Class
    | Enum
    | Enum Struct
    | Enum Class
    | Alias
    | Extension
    | typemod Backtick
    ;

import_stmt:
      From postfix Import imports
    | From postfix Import Mul
    ;

imports:
      import
    | imports Comma import
    ;

import:
      postfix
    | postfix As Identifier
    ;

expr:
      lambda
    | If expr Then expr Else expr
    | match
    | trycatch
    | Throw expr
    | block
    | binary
    ;

exprs:
      expr
    | exprs Comma expr
    ;

type:
      binary
    ;

lambda:
      fn params captures fn_ret fn_throw FatArrow expr
    ;

match:
      Match expr LBrace cases RBrace
    ;

/**
 * One or more case expressions.
 */
cases:
      case
    | cases case
    ;

case:
      Case pattern FatArrow expr
    | Case If expr FatArrow expr
    | Case pattern If expr FatArrow expr
    ;

trycatch:
      Try expr LBrace catches RBrace
    ;

/**
 * One or more catch expressions.
 */
catches:
      catch
    | catches catch
    ;

catch:
      Catch pattern FatArrow expr
    | Catch If expr FatArrow expr
    | Catch pattern If expr FatArrow expr
    ;

block:
      LBrace stmts RBrace
    ;

binary:
      binary_is
    ;

binary_is:
      binary_pipe Is binding
    | binary_pipe Is binary_pipe
    | binary_pipe
    ;

binary_pipe:
      binary_type Pipe binary_pipe
    | binary_type
    ;

binary_type:
      binary_range Arrow binary_type
    | binary_range
    ;

binary_range:
      binary_coalesce Range binary_coalesce
    | binary_coalesce RangeExcl binary_coalesce
    | binary_coalesce
    ;

binary_coalesce:
      binary_logor Coalesce binary_coalesce
    | binary_logor
    ;

binary_logor:
      binary_logor LogOr binary_logand
    | binary_logand
    ;

binary_logand:
      binary_logand LogAnd binary_bitor
    | binary_bitor
    ;

binary_bitor:
      binary_bitor VBar binary_bitxor
    | binary_bitxor
    ;

binary_bitxor:
      binary_bitxor Caret binary_bitand
    | binary_bitand
    ;

binary_bitand:
      binary_bitand And binary_eq
    | binary_eq
    ;

binary_eq:
      binary_compare Eq binary_compare
    | binary_compare Ne binary_compare
    | binary_compare StrictEq binary_compare
    | binary_compare StrictNe binary_compare
    | binary_compare
    ;

binary_compare:
      binary_bitshift Lt binary_bitshift
    | binary_bitshift Le binary_bitshift
    | binary_bitshift Gt binary_bitshift
    | binary_bitshift Ge binary_bitshift
    | binary_bitshift
    ;

binary_bitshift:
      binary_bitshift Shl binary_additive
    | binary_bitshift Shr binary_additive
    | binary_additive
    ;

binary_additive:
      binary_additive Add binary_multiplicative
    | binary_additive Sub binary_multiplicative
    | binary_multiplicative
    ;

binary_multiplicative:
      binary_multiplicative Mul prefix
    | binary_multiplicative Div prefix
    | binary_multiplicative Mod prefix
    | prefix
    ;

prefix:
      Add prefix
    | Sub prefix
    | Tide prefix
    | Bang prefix
    | Dollar prefix
    | postfix
    ;

postfix:
      primary postfix_ops
    ;

postfix_ops:
      /* empty */
    | postfix_ops postfix_op
    ;

postfix_op:
      Backtick
    | Question
    | Bang
    | Dot identifier
    | LParen call_args RParen
    | LBracket subscript_args RBracket
    ;

call_args:
      /* empty */
    | positional_args
    | positional_args Comma named_args
    | named_args
    ;

positional_args:
      pattern
    | positional_args Comma pattern
    ;

named_args:
      named_arg
    | named_args Comma named_arg
    ;

named_arg:
      identifier Colon pattern
    ;

subscript_args:
      expr
    | expr Comma expr
    ;

primary:
      literal
    | identifier
    | Self
    | Global
    | LParen expr RParen
    | tuple
    | collection
    | Typeof LParen expr RParen
    ;

literal:
      Integer
    | Real
    | True
    | False
    | String
    | Nil
    | LParen RParen
    ;

identifier:
      Identifier
    | Discard
    ;

tuple:
      LParen pattern Comma patterns RParen
    ;

/**
 * [], [...], [patterns], [..., patterns], [patterns, ...], [patterns, ..., patterns]
 */
collection:
      // []
      LBracket RBracket
      // [...]
    | LBracket Range RBracket
      // [patterns]
    | LBracket patterns RBracket
      // [..., patterns]
    | LBracket Range Comma patterns RBracket
      // [patterns, ...]
    | LBracket patterns Comma Range RBracket
      // [patterns, ..., patterns]
    | LBracket patterns Comma Range Comma patterns RBracket
    ;



pattern:
      binding
    | expr
    ;

patterns:
      pattern
    | patterns Comma pattern
    ;

binding:
      varmod identifier
    | varmod identifier Colon type
    ;

varmod:
      Let
    | Const
    | Ref
    | Cref
    | varmod Backtick
    | varmod Question
    | varmod Coalesce
    | varmod Bang
    ;



annotation:
      At identifier
    | At identifier LParen call_args RParen
    ;

annotations:
      /* empty */
    | annotations annotation
    ;
%%

void yy::parser::error(const yy::location& loc, const std::string& msg) {
    Location start(static_cast<size_t>(loc.begin.line), static_cast<size_t>(loc.begin.column));
    Location end(static_cast<size_t>(loc.end.line), static_cast<size_t>(loc.end.column));
}
