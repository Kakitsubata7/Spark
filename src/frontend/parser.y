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

%type program
%type <Spark::FrontEnd::Terms*> terms
%type <Spark::FrontEnd::Node*> term
%type <Spark::FrontEnd::Node*> literal
%type <Spark::FrontEnd::Node*> paren_terms
%type <Spark::FrontEnd::Node*> tuple collection
%type <Spark::FrontEnd::Node*> tuple_terms collection_terms
%type <Spark::FrontEnd::Node*> operator
%type <Spark::FrontEnd::Node*> assign
%type <Spark::FrontEnd::Node*> vardef typedef
%type <Spark::FrontEnd::Block*> block

%%
%start program;

program:
      terms { ast.root()->block->terms->nodes = std::move($1->nodes); }
    ;

terms:
      /* empty */ { $$->nodes.clear(); }
    | terms term
        {
            if ($2 != nullptr) {
                $$->nodes.push_back($2);
            }
        }
    ;

term:
      Identifier        { $$ = ast.make<Identifier>($1.start, $1.end, std::move($1.lexeme)); }
    | Discard           { $$ = ast.make<Discard>($1.start, $1.end); }
    | Dollar Identifier { $$ = ast.make<UpvalueIdentifier>($1.start, $1.end, std::move($2.lexeme)); }
    | Dollar Discard    { $$ = ast.make<Discard>($1.start, $2.end); }
    | literal
    | paren_terms
    | collection
    | operator
    | assign
    | Dot               { $$ = ast.make<Operation>($1.start, $1.end, OperationKind::Dot); }
    | vardef
    | typedef
    | Global            { $$ = nullptr; }
    | Self              { $$ = nullptr; }
    | Super             { $$ = nullptr; }
    | block             { $$ = $1; }
    | If                { $$ = ast.make<IfKeyword>($1.start, $1.end); }
    | Then              { $$ = ast.make<ThenKeyword>($1.start, $1.end); }
    | Else              { $$ = ast.make<ElseKeyword>($1.start, $1.end); }
    | Match             { $$ = ast.make<MatchKeyword>($1.start, $1.end); }
    | Case              { $$ = ast.make<CaseKeyword>($1.start, $1.end); }
    | While             { $$ = ast.make<While>($1.start, $1.end, nullptr, nullptr); }
    | Do                { $$ = ast.make<DoWhile>($1.start, $1.end, nullptr, nullptr); }
    | For               { $$ = ast.make<For>($1.start, $1.end, nullptr, nullptr, nullptr); }
    | In                { $$ = ast.make<InKeyword>($1.start, $1.end); }
    | Break             { $$ = ast.make<Break>($1.start, $1.end); }
    | Continue          { $$ = ast.make<Continue>($1.start, $1.end); }
    | Return            { $$ = ast.make<Return>($1.start, $1.end, nullptr); }
    | Is                { $$ = ast.make<Operation>($1.start, $1.end, OperationKind::Is); }
    | Typeof            { $$ = ast.make<Typeof>($1.start, $1.end, nullptr); }
    | Throw             { $$ = ast.make<Throw>($1.start, $1.end, nullptr); }
    | Try               { $$ = ast.make<TryKeyword>($1.start, $1.end); }
    | Catch             { $$ = ast.make<Catch>($1.start, $1.end, nullptr, nullptr); }
    | Module block      { $$ = ast.make<Module>($1.start, $2->end, $2); }
    | Export            { $$ = ast.make<Export>($1.start, $1.end, nullptr); }
    | From              { $$ = ast.make<From>($1.start, $1.end, nullptr); }
    | Import            { $$ = ast.make<Import>($1.start, $1.end, nullptr); }
    | At term           { $$ = ast.make<Annotation>($1.start, $2->end, $2); }
    | Semicolon         { $$ = ast.make<Semicolon>($1.start, $1.end); }
    | LineComment       { $$ = nullptr; }
    | BlockComment      { $$ = nullptr; }
    ;

literal:
      Integer   { $$ = ast.make<IntLiteral>($1.start, $1.end, BigInt($1.lexeme)); }
    | Real      { $$ = ast.make<RealLiteral>($1.start, $1.end, BigReal($1.lexeme)); }
    | True      { $$ = ast.make<BoolLiteral>($1.start, $1.end, true); }
    | False     { $$ = ast.make<BoolLiteral>($1.start, $1.end, false); }
    | String    { $$ = ast.make<StringLiteral>($1.start, $1.end, std::move($1.lexeme)); }
    | Nil       { $$ = ast.make<Nil>($1.start, $1.end); }
    | Undefined { $$ = ast.make<Undefined>($1.start, $1.end); }
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

operator:
      Add       { $$ = ast.make<Operation>($1.start, $1.end, OperationKind::Add); }
    | Sub       { $$ = ast.make<Operation>($1.start, $1.end, OperationKind::Sub); }
    | Mul       { $$ = ast.make<Operation>($1.start, $1.end, OperationKind::Mul); }
    | Div       { $$ = ast.make<Operation>($1.start, $1.end, OperationKind::Div); }
    | Mod       { $$ = ast.make<Operation>($1.start, $1.end, OperationKind::Mod); }
    | Tide      { $$ = ast.make<Operation>($1.start, $1.end, OperationKind::Tide); }
    | And       { $$ = ast.make<Operation>($1.start, $1.end, OperationKind::And); }
    | VBar      { $$ = ast.make<Operation>($1.start, $1.end, OperationKind::VBar); }
    | Caret     { $$ = ast.make<Operation>($1.start, $1.end, OperationKind::Caret); }
    | Bang      { $$ = ast.make<Operation>($1.start, $1.end, OperationKind::Bang); }
    | LogAnd    { $$ = ast.make<Operation>($1.start, $1.end, OperationKind::LogAnd); }
    | LogOr     { $$ = ast.make<Operation>($1.start, $1.end, OperationKind::LogOr); }
    | Eq        { $$ = ast.make<Operation>($1.start, $1.end, OperationKind::Eq); }
    | Ne        { $$ = ast.make<Operation>($1.start, $1.end, OperationKind::Ne); }
    | Lt        { $$ = ast.make<Operation>($1.start, $1.end, OperationKind::Lt); }
    | Gt        { $$ = ast.make<Operation>($1.start, $1.end, OperationKind::Gt); }
    | Le        { $$ = ast.make<Operation>($1.start, $1.end, OperationKind::Le); }
    | Ge        { $$ = ast.make<Operation>($1.start, $1.end, OperationKind::Ge); }
    | StrictEq  { $$ = ast.make<Operation>($1.start, $1.end, OperationKind::StrictEq); }
    | StrictNe  { $$ = ast.make<Operation>($1.start, $1.end, OperationKind::StrictNe); }
    | Range     { $$ = ast.make<Operation>($1.start, $1.end, OperationKind::Range); }
    | RangeExcl { $$ = ast.make<Operation>($1.start, $1.end, OperationKind::RangeExcl); }
    | Question  { $$ = ast.make<Operation>($1.start, $1.end, OperationKind::Question); }
    | Coalesce  { $$ = ast.make<Operation>($1.start, $1.end, OperationKind::Coalesce); }
    | Pipe      { $$ = ast.make<Operation>($1.start, $1.end, OperationKind::Pipe); }
    ;

assign:
      Assign         { $$ = ast.make<Operation>($1.start, $1.end, OperationKind::Assign); }
    | AddAssign      { $$ = ast.make<Operation>($1.start, $1.end, OperationKind::AddAssign); }
    | SubAssign      { $$ = ast.make<Operation>($1.start, $1.end, OperationKind::SubAssign); }
    | MulAssign      { $$ = ast.make<Operation>($1.start, $1.end, OperationKind::MulAssign); }
    | DivAssign      { $$ = ast.make<Operation>($1.start, $1.end, OperationKind::DivAssign); }
    | ModAssign      { $$ = ast.make<Operation>($1.start, $1.end, OperationKind::ModAssign); }
    | BitAndAssign   { $$ = ast.make<Operation>($1.start, $1.end, OperationKind::BitAndAssign); }
    | BitOrAssign    { $$ = ast.make<Operation>($1.start, $1.end, OperationKind::BitOrAssign); }
    | BitXorAssign   { $$ = ast.make<Operation>($1.start, $1.end, OperationKind::BitXorAssign); }
    | CoalesceAssign { $$ = ast.make<Operation>($1.start, $1.end, OperationKind::CoalesceAssign); }
    ;

vardef:
      Let   { $$ = ast.make<VarDef>($1.start, $1.end, VarDefKind::Let, false, false, nullptr, nullptr); }
    | Const { $$ = ast.make<VarDef>($1.start, $1.end, VarDefKind::Const, false, false, nullptr, nullptr); }
    | Ref   { $$ = ast.make<VarDef>($1.start, $1.end, VarDefKind::Ref, false, false, nullptr, nullptr); }
    | Cref  { $$ = ast.make<VarDef>($1.start, $1.end, VarDefKind::Cref, false, false, nullptr, nullptr); }
    ;

typedef:
      Struct    { $$ = ast.make<TypeDefKeyword>($1.start, $1.end, TypeDefKind::Struct); }
    | Class     { $$ = ast.make<TypeDefKeyword>($1.start, $1.end, TypeDefKind::Class); }
    | Enum      { $$ = ast.make<TypeDefKeyword>($1.start, $1.end, TypeDefKind::Enum); }
    | Trait     { $$ = ast.make<TypeDefKeyword>($1.start, $1.end, TypeDefKind::Trait); }
    | Alias     { $$ = ast.make<TypeDefKeyword>($1.start, $1.end, TypeDefKind::Alias); }
    | Extension { $$ = ast.make<TypeDefKeyword>($1.start, $1.end, TypeDefKind::Extension); }
    ;

block:
      LBrace terms RBrace
        {
            $$ = ast.make<Block>($1.start, $3.end, $2);
        }
    ;
%%

void yy::parser::error(const yy::location& loc, const std::string& msg) {
    Location start(static_cast<size_t>(loc.begin.line), static_cast<size_t>(loc.begin.column));
    Location end(static_cast<size_t>(loc.end.line), static_cast<size_t>(loc.end.column));
    ctx.addError(msg, start, end);
}
