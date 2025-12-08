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

#define RAISE_ERROR(start, end, msg) raiseError(*this, (start), (end), (msg));
}

%token <Spark::FrontEnd::TokenValue> Identifier Discard
%token <Spark::FrontEnd::TokenValue> Integer Real String
%token <Spark::FrontEnd::TokenValue> Alias As Break Case Catch Class Const Constructor Continue Cref Destructor Do Else End Enum Export Extension False Fn For From Global If Import In Is Let Match Module Nil Operator Ref Return Self Struct Super Then Throw Trait True Try Typeof Undefined While Yield
%token <Spark::FrontEnd::TokenValue> Add Sub Mul Div Mod
%token <Spark::FrontEnd::TokenValue> Tide And VBar Caret
%token <Spark::FrontEnd::TokenValue> Bang LogAnd LogOr
%token <Spark::FrontEnd::TokenValue> Eq Ne Lt Gt Le Ge
%token <Spark::FrontEnd::TokenValue> StrictEq StrictNe
%token <Spark::FrontEnd::TokenValue> Range RangeExcl
%token <Spark::FrontEnd::TokenValue> Question Coalesce
%token <Spark::FrontEnd::TokenValue> Pipe
%token <Spark::FrontEnd::TokenValue> Assign AddAssign SubAssign MulAssign DivAssign ModAssign BitAndAssign BitOrAssign BitXorAssign CoalesceAssign
%token <Spark::FrontEnd::TokenValue> Dot
%token <Spark::FrontEnd::TokenValue> Comma Colon Arrow FatArrow
%token <Spark::FrontEnd::TokenValue> Semicolon LParen RParen LBracket RBracket LBrace RBrace
%token <Spark::FrontEnd::TokenValue> At
%token <Spark::FrontEnd::TokenValue> Dollar
%token <Spark::FrontEnd::TokenValue> LineComment BlockComment

%token EndOfFile 0
%token Error 1

%type <Spark::FrontEnd::Node*> program
%type <Spark::FrontEnd::Node*> term
%type <Spark::FrontEnd::Node*> name basename
%type <Spark::FrontEnd::Node*> literal
%type <Spark::FrontEnd::Node*> paren_terms
%type <Spark::FrontEnd::Node*> tuple collection
%type <Spark::FrontEnd::Node*> tuple_terms collection_terms
%type <Spark::FrontEnd::Node*> type_annot
%type <Spark::FrontEnd::Node*> operator
%type <Spark::FrontEnd::Node*> assign
%type <Spark::FrontEnd::Node*> fn fnmod fnbody
%type <Spark::FrontEnd::Node*> vardef varmod typedef
%type <Spark::FrontEnd::Node*> block
%type <Spark::FrontEnd::Node*> if
%type <Spark::FrontEnd::Node*> match cases case
%type <std::vector<Spark::FrontEnd::Node*>> block_list

%%
%start program;

program:
      /* empty */
        {
            auto* root = ctx.ast().root();
            root->nodes.clear();
            $$ = root;
        }
    | program term
        {
            if ($2 != nullptr) {
                ctx.ast().root()->nodes.push_back($2);
            }
            $$ = $1;
        }
    ;

term:
      name
    | literal
    | paren_terms
    | collection
    | type_annot
    | operator
    | assign
    | fn
    | vardef
    | typedef
    | block
    | if
    | match
    | While term block { $$ = nullptr; }
    | Do block While term
{
    $$ = nullptr;
}
    | For term In term block
        {
            $$ = nullptr;
        }
    | Break            { $$ = nullptr; }
    | Continue         { $$ = nullptr; }
    | Return           { $$ = nullptr; }
    | Throw            { $$ = nullptr; }
    | Try              { $$ = nullptr; }
    | Catch            { $$ = nullptr; }
    | Is               { $$ = nullptr; }
    | Typeof LParen name RParen
        {
            $$ = nullptr;
        }
    | Module block     { $$ = nullptr; }
    | Export term      { $$ = nullptr; }
    | From term        { $$ = nullptr; }
    | Import term      { $$ = nullptr; }
    | At term          { $$ = nullptr; }
    | Semicolon        { $$ = nullptr; }
    | LineComment      { $$ = nullptr; }
    | BlockComment     { $$ = nullptr; }
    ;

literal:
      Integer   { $$ = ctx.makeNode<IntLiteral>($1.start, $1.end, BigInt($1.lexeme)); }
    | Real      { $$ = ctx.makeNode<RealLiteral>($1.start, $1.end, BigReal($1.lexeme)); }
    | True      { $$ = ctx.makeNode<BoolLiteral>($1.start, $1.end, true); }
    | False     { $$ = ctx.makeNode<BoolLiteral>($1.start, $1.end, false); }
    | String    { $$ = ctx.makeNode<StringLiteral>($1.start, $1.end, std::move($1.lexeme)); }
    | Nil       { $$ = ctx.makeNode<NilLiteral>($1.start, $1.end); }
    | Undefined { $$ = nullptr; }
    ;

/**
  * identifier, $identifier.identifier, ...
  */
name:
      basename
    | name Dot basename { $$ = nullptr; }
    ;

/**
  * identifier, _, $identifier, $_, global, self, super
  */
basename:
      Identifier        { $$ = nullptr; }
    | Discard           { $$ = nullptr; }
    | Dollar Identifier { $$ = nullptr; }
    | Dollar Discard    { $$ = nullptr; }
    | Global            { $$ = nullptr; }
    | Self              { $$ = nullptr; }
    | Super             { $$ = nullptr; }
    ;

/**
  * (), (term), (term, term, ...)
  */
paren_terms:
      LParen RParen         { $$ = nullptr; }
    | LParen term RParen    { $$ = nullptr; }
    | tuple
    ;

/**
  * (term, term, ...)
  */
tuple:
      LParen tuple_terms RParen { $$ = nullptr; }
    ;

tuple_terms:
      term Comma term
    | tuple_terms Comma term
    ;

/**
  * [], [term], [term, ...]
  */
collection:
      LBracket collection_terms RBracket { $$ = nullptr; }
    ;

collection_terms:
      /* empty */                 { $$ = nullptr; }
    | term                        { $$ = nullptr; }
    | collection_terms Comma term { $$ = nullptr; }
    ;

/**
  * identifier : name
  */
type_annot:
      Identifier Colon name { $$ = nullptr; }
    ;

operator:
      Add       { $$ = nullptr; }
    | Sub       { $$ = nullptr; }
    | Mul       { $$ = nullptr; }
    | Div       { $$ = nullptr; }
    | Mod       { $$ = nullptr; }
    | Tide      { $$ = nullptr; }
    | And       { $$ = nullptr; }
    | VBar      { $$ = nullptr; }
    | Caret     { $$ = nullptr; }
    | Bang      { $$ = nullptr; }
    | LogAnd    { $$ = nullptr; }
    | LogOr     { $$ = nullptr; }
    | Eq        { $$ = nullptr; }
    | Ne        { $$ = nullptr; }
    | Lt        { $$ = nullptr; }
    | Gt        { $$ = nullptr; }
    | Le        { $$ = nullptr; }
    | Ge        { $$ = nullptr; }
    | StrictEq  { $$ = nullptr; }
    | StrictNe  { $$ = nullptr; }
    | Range     { $$ = nullptr; }
    | RangeExcl { $$ = nullptr; }
    | Question  { $$ = nullptr; }
    | Coalesce  { $$ = nullptr; }
    | Pipe      { $$ = nullptr; }
    ;

assign:
      Assign         { $$ = nullptr; }
    | AddAssign      { $$ = nullptr; }
    | SubAssign      { $$ = nullptr; }
    | MulAssign      { $$ = nullptr; }
    | DivAssign      { $$ = nullptr; }
    | ModAssign      { $$ = nullptr; }
    | BitAndAssign   { $$ = nullptr; }
    | BitOrAssign    { $$ = nullptr; }
    | BitXorAssign   { $$ = nullptr; }
    | CoalesceAssign { $$ = nullptr; }
    ;

fn:
      fnmod paren_terms fnbody
        {
            $$ = nullptr;
        }
    | fnmod paren_terms Throw fnbody
        {
            $$ = nullptr;
        }
    | fnmod paren_terms Throw paren_terms fnbody
        {
            $$ = nullptr;
        }
    | fnmod paren_terms Arrow name fnbody
        {
            $$ = nullptr;
        }
    | fnmod paren_terms Arrow name Throw fnbody
        {
            $$ = nullptr;
        }
    | fnmod paren_terms Arrow name Throw paren_terms fnbody
        {
            $$ = nullptr;
        }
    ;

fnmod:
      Fn             { $$ = nullptr; }
    | fnmod operator { $$ = nullptr; }
    ;

fnbody:
      block         { $$ = nullptr; }
    | FatArrow term { $$ = nullptr; }

vardef:
      varmod Identifier { $$ = nullptr; }
    | varmod type_annot { $$ = nullptr; }
    ;

varmod:
      Let             { $$ = nullptr; }
    | Const           { $$ = nullptr; }
    | Ref             { $$ = nullptr; }
    | Cref            { $$ = nullptr; }
    | varmod operator { $$ = nullptr; }
    ;

typedef:
      Class name block       { $$ = nullptr; }
    | Struct name block      { $$ = nullptr; }
    | Enum name block        { $$ = nullptr; }
    | Enum Class name block  { $$ = nullptr; }
    | Enum Struct name block { $$ = nullptr; }
    | Trait name block       { $$ = nullptr; }
    | Alias name block       { $$ = nullptr; }
    | Extension name block   { $$ = nullptr; }

block:
      LBrace block_list RBrace
        {
            auto* block = ctx.makeNode<Block>($1.start, $3.end);
            block->nodes.insert(block->nodes.end(), $2.begin(), $2.end());
            $$ = block;
        }
    ;

block_list:
      /* empty */    { $$.clear(); }
    | block_list term
        {
            $1.push_back($2);
            $$ = $1;
        }
    ;

if:
      If term Then term Else term { $$ = nullptr; }
    | If term block                     { $$ = nullptr; }
    | Else                                 { $$ = nullptr; }
    ;

match:
      Match term LBrace cases RBrace { $$ = nullptr; }
    ;

cases:
      /* empty */ { $$ = nullptr;}
    | cases case  { $$ = nullptr; }

case:
      Case term FatArrow term    { $$ = nullptr; }
    | Case If term FatArrow term { $$ = nullptr; }
    | Case term block            { $$ = nullptr; }
    | Case If term block         { $$ = nullptr; }
    ;
%%

void yy::parser::error(const yy::location& loc, const std::string& msg) {
    Location start(static_cast<size_t>(loc.begin.line), static_cast<size_t>(loc.begin.column));
    Location end(static_cast<size_t>(loc.end.line), static_cast<size_t>(loc.end.column));
    ctx.addError(msg, start, end);
}
