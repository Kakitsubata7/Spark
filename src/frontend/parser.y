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
%token <Spark::FrontEnd::TokenValue> Alias As Break Case Catch Class Const Constructor Continue Cref Destructor Do Else End Enum Export Extension False Fn For From Global If Import In Is Let Match Module Nil Operator Ref Return Self Struct Super Then Throw Trait True Try Typeof Undefined While Yield
%token <Spark::FrontEnd::TokenValue> Add Sub Mul Div Mod
%token <Spark::FrontEnd::TokenValue> Tide And VBar Caret
%token <Spark::FrontEnd::TokenValue> Bang LogAnd LogOr
%token <Spark::FrontEnd::TokenValue> Eq Ne Lt Le Gt Ge
%token <Spark::FrontEnd::TokenValue> StrictEq StrictNe
%token <Spark::FrontEnd::TokenValue> Shl Shr
%token <Spark::FrontEnd::TokenValue> Range RangeExcl
%token <Spark::FrontEnd::TokenValue> Backtick Question Coalesce
%token <Spark::FrontEnd::TokenValue> Pipe
%token <Spark::FrontEnd::TokenValue> Assign AddAssign SubAssign MulAssign DivAssign ModAssign BitAndAssign BitOrAssign BitXorAssign CoalesceAssign
%token <Spark::FrontEnd::TokenValue> Dot
%token <Spark::FrontEnd::TokenValue> Comma Colon Arrow FatArrow
%token <Spark::FrontEnd::TokenValue> Semicolon Newline LParen RParen LBracket RBracket LBrace RBrace LABracket RABracket
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
    | some_stmts
    | some_stmts terminators
    ;

some_stmts:
      stmt
    | some_stmts terminators stmt
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
      if_stmt
    | typedef_stmt
    | While expr block
    | For pattern In expr block
    | Break
    | Continue
    | Return
    | Return expr
    | expr
    ;

if_stmt:
      If expr block elseif_stmts
    | If expr block elseif_stmts Else block
    ;

elseif_stmts:
      /* empty */
    | elseif_stmts Else If expr block
    ;

typedef_stmt:
      typemod Identifier block
    | typemod Identifier Colon postfixes block
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

expr:
      If expr Then expr Else expr
    | match
    | trycatch
    | Throw expr
    | block
    | binary
    ;

block:
      LBrace stmts RBrace
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

binary:
      binary_assign
    ;

binary_assign:
      pattern Assign binary_assign
    | binary_is
    ;

binary_is:
      binary_type Is binary_type
    | binary_type
    ;

binary_type:
      binary_logor Arrow binary_type
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
    | And prefix
    | postfix
    ;

postfix:
      primary postfix_ops
    ;

/**
 * One or more postfix expressions, separated by ','.
 */
postfixes:
      postfix
    | postfixes Comma postfix
    ;

postfix_ops:
      /* empty */
    | postfix_ops postfix_op
    ;

postfix_op:
      Backtick
    | Question
    | Bang
    | Dot Identifier
    | Dot Discard
    | LParen args RParen
    | LBracket args RBracket
    ;

args:
      /* empty */
    | expr
    | args Comma expr
    ;

primary:
      literal
    | identifier
    | LParen expr RParen
    | Typeof LParen expr RParen
    ;

identifier:
      dollars Identifier
    | dollars Discard
    ;

dollars:
      /* empty */
    | dollars Dollar
    ;



pattern:
      binding
    | LParen pattern Comma patterns RParen
    | collection_pattern
    ;

/**
 * One or more patterns, separated by commas.
 */
patterns:
      pattern
    | patterns Comma pattern
    ;

binding:
      varmod Identifier
    | varmod Discard
    | varmod Identifier Colon expr
    | varmod Discard Colon expr
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

/**
 * [], [...], [patterns], [..., patterns], [patterns, ...], [patterns, ..., patterns]
 */
collection_pattern:
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



lhs:
      postfix
    | binding
    | LParen pattern Comma patterns RParen
    | collection_pattern
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
%%

void yy::parser::error(const yy::location& loc, const std::string& msg) {
    Location start(static_cast<size_t>(loc.begin.line), static_cast<size_t>(loc.begin.column));
    Location end(static_cast<size_t>(loc.end.line), static_cast<size_t>(loc.end.column));
}
