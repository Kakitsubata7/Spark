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

%type <Spark::FrontEnd::Node*> program element operators assign block literal
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
    | program element
        {
            if ($2 != nullptr) {
                ctx.ast().root()->nodes.push_back($2);
            }
            $$ = $1;
        }
    ;

element:
      literal
    | Identifier            { $$ = nullptr; }
    | Discard               { $$ = nullptr; }
    | Dollar Identifier     { $$ = nullptr; }
    | Dollar Discard        { $$ = nullptr; }
    | operators
    | LParen element RParen { $$ = nullptr; }
    | assign
    | block
    | While element block   { $$ = nullptr; }
    | Break                 { $$ = nullptr; }
    | Continue              { $$ = nullptr; }
    | Return                { $$ = nullptr; }
    | Throw                 { $$ = nullptr; }
    | LineComment           { }
    | BlockComment          { }
    ;

literal:
      Integer { $$ = ctx.makeNode<IntLiteral>($1.start, $1.end, BigInt($1.lexeme)); }
    | Real    { $$ = ctx.makeNode<RealLiteral>($1.start, $1.end, BigReal($1.lexeme)); }
    | True    { $$ = ctx.makeNode<BoolLiteral>($1.start, $1.end, true); }
    | False   { $$ = ctx.makeNode<BoolLiteral>($1.start, $1.end, false); }
    | String  { $$ = ctx.makeNode<StringLiteral>($1.start, $1.end, std::move($1.lexeme)); }
    | Nil     { $$ = ctx.makeNode<NilLiteral>($1.start, $1.end); }
    ;

operators:
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

block:
      Do block_list End
        {
            auto* block = ctx.makeNode<Block>($1.start, $3.end);
            block->nodes.insert(block->nodes.end(), $2.begin(), $2.end());
            $$ = block;
        }
    ;

block_list:
      /* empty */    { $$.clear(); }
    | block_list element
        {
            $1.push_back($2);
            $$ = $1;
        }
    ;
%%

void yy::parser::error(const yy::location& loc, const std::string& msg) {
    Location start(static_cast<size_t>(loc.begin.line), static_cast<size_t>(loc.begin.column));
    Location end(static_cast<size_t>(loc.end.line), static_cast<size_t>(loc.end.column));
    ctx.addError(msg, start, end);
}
