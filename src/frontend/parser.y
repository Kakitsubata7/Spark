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
%token <Spark::FrontEnd::TokenValue> Semicolon Newline LParen RParen LBracket RBracket LBrace RBrace LABracket RABracket
%token <Spark::FrontEnd::TokenValue> At
%token <Spark::FrontEnd::TokenValue> Dollar
%token <Spark::FrontEnd::TokenValue> LineComment BlockComment

%token EndOfFile 0
%token Error 1

%%
%start program;

program:
      stmts
    ;

/**
 * Zero or more statements, separated by at least one statement terminator.
 */
stmts:
      /* empty */
    | some_stmt
    | some_stmt terminator
    ;

/**
 * One or more statements, separated by a statement terminator.
 */
some_stmt:
      stmt
    | some_stmt terminator stmt
    ;

terminator:
      Newline
    | Semicolon
    ;

return:
      Return
    | Return expr
    ;

throw:
      Throw expr
    ;

stmt:
      block
    | assign
    | typedef
    | ifelse
    | match_stmt
    | While expr block
    | Do block While expr
    | For pattern In expr block
    | Break
    | Continue
    | return
    | throw
    | trycatch_stmt
    ;

block:
      LBrace stmts RBrace
    ;

assign:
      pattern Assign expr
/* TODO:    | lhs AddAssign expr
    | lhs SubAssign expr
    | lhs MulAssign expr
    | lhs DivAssign expr
    | lhs ModAssign expr
    | lhs BitAndAssign expr
    | lhs BitOrAssign expr
    | lhs BitXorAssign expr
    | lhs CoalesceAssign expr*/
    ;

typedef:
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
    | typemod Caret
    ;

ifelse:
      If expr block elseifs
    | If expr block elseifs Else block
    ;

elseifs:
      /* empty */
    | elseifs Else If expr block
    ;

match_stmt:
      Match expr LBrace case_stmts RBrace
    | Match pattern Assign expr LBrace case_stmts RBrace
    ;

/**
 * One or more case statements.
 */
case_stmts:
      case_stmt
    | case_stmts case_stmt
    ;

case_stmt:
      Case pattern block
    | Case pattern If expr block
    ;

trycatch_stmt:
      Try block catch_stmts
    ;

/**
 * One or more catch statements.
 */
catch_stmts:
      catch_stmt
    | catch_stmts catch_stmt
    ;

catch_stmt:
      Catch pattern block
    | Catch pattern If expr block
    ;



expr:
      If expr Then expr Else expr
    | match_expr
    | Try expr Else expr
    | trycatch_expr
    | throw
    | assign_expr
    ;

match_expr:
      Match expr LBrace case_exprs RBrace
    | Match pattern Assign expr LBrace case_exprs RBrace
    ;

/**
 * One or more case expressions.
 */
case_exprs:
      case_expr
    | case_exprs case_expr
    ;

case_expr:
      Case pattern FatArrow expr
    | Case pattern If expr FatArrow expr
    ;

trycatch_expr:
      Try expr LBrace catch_exprs RBrace
    ;

/**
 * One or more catch expressions.
 */
catch_exprs:
      catch_expr
    | catch_exprs catch_expr
    ;

catch_expr:
      Catch pattern FatArrow expr
    | Catch pattern If expr FatArrow expr
    ;

assign_expr:
      // TODO: lhs Assign assign_expr
      binary
    ;

binary:
      binary Add prefix
    | binary Sub prefix
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
      Caret
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
    | Identifier
    | Discard
    | Dollar Identifier
    | Dollar Discard
    | LParen expr RParen
    | Typeof LParen expr RParen
    | seq
    ;

seq:
      LBrace seq_exprs RBrace
    ;

/**
 * One or more expr, separated by ';'.
 */
seq_exprs:
      expr
    | seq_exprs Semicolon expr
    ;



pattern:
      postfix
    | binding
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
    | varmod Caret
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



literal:
      Integer
    | Real
    | True
    | False
    | String
    | Nil
    | LParen RParen
    ;



lhs:
      lhs_expr
    | binding
    | LParen lhs Comma lhses RParen
    | lhs_collection
    ;

lhs_expr:
      lhs_primary postfix_ops
    ;

lhs_primary:
      Identifier
    | Discard
    | Dollar Identifier
    | Dollar Discard
    | LParen lhs RParen
    ;

/**
 * One or more left-hand-side patterns, separated by commas.
 */
lhses:
      lhs
    | lhses Comma lhs
    ;

/**
 * [], [...], [lhses], [..., lhses], [lhses, ...], [lhses, ..., lhses]
 */
lhs_collection:
      // []
      LBracket RBracket
      // [...]
    | LBracket Range RBracket
      // [lhses]
    | LBracket lhses RBracket
      // [..., lhses]
    | LBracket Range Comma lhses RBracket
      // [lhses, ...]
    | LBracket lhses Comma Range RBracket
      // [lhses, ..., lhses]
    | LBracket lhses Comma Range Comma lhses RBracket
    ;
%%

void yy::parser::error(const yy::location& loc, const std::string& msg) {
    Location start(static_cast<size_t>(loc.begin.line), static_cast<size_t>(loc.begin.column));
    Location end(static_cast<size_t>(loc.end.line), static_cast<size_t>(loc.end.column));
}
