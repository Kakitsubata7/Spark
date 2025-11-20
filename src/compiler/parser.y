%require "3.5"

%skeleton "lalr1.cc"

%language "c++"

%define api.token.raw
%define api.value.type variant
%define parse.error verbose

%lex-param {yyscan_t scanner}
%parse-param {yyscan_t scanner} {Spark::Compiler::LexerState& lstate} {Spark::Compiler::ParserContext& ctx}

%code requires {
#include <vector>

#include "compiler/ast/expr.hpp"
#include "compiler/ast/node.hpp"
#include "compiler/ast/stmt.hpp"
#include "compiler/lexer_state.hpp"
#include "compiler/parser_context.hpp"
#include "compiler/parser_error.hpp"
#include "compiler/token_value.hpp"

typedef void* yyscan_t;
}

%code {
using namespace Spark;
using namespace Spark::Compiler;
using namespace Spark::Compiler::AST;

int yylex(yy::parser::semantic_type* yylval, yyscan_t scanner);
}

%token <Spark::Compiler::TokenValue> Identifier Discard
%token <Spark::Compiler::TokenValue> Integer Real String
%token <Spark::Compiler::TokenValue> Alias As Break Case Catch Class Const Constructor Continue Cref Destructor Do Else End Enum Export Extension False Fn For From Global If Import In Is Let Match Module Nil Operator Ref Return Self Struct Super Then Throw Trait True Try Typeof Undefined While Yield
%token <Spark::Compiler::TokenValue> Add Sub Mul Div Mod
%token <Spark::Compiler::TokenValue> BitNot BitAnd BitOr BitXor BitShl BitShr
%token <Spark::Compiler::TokenValue> LogNot LogAnd LogOr
%token <Spark::Compiler::TokenValue> Eq Ne Lt Gt Le Ge
%token <Spark::Compiler::TokenValue> StrictEq StrictNe
%token <Spark::Compiler::TokenValue> Range RangeExcl
%token <Spark::Compiler::TokenValue> Question Coalesce
%token <Spark::Compiler::TokenValue> Assign AddAssign SubAssign MulAssign DivAssign ModAssign BitAndAssign BitOrAssign BitXorAssign BitShlAssign BitShrAssign CoalesceAssign
%token <Spark::Compiler::TokenValue> Dot
%token <Spark::Compiler::TokenValue> Comma Colon Arrow FatArrow
%token <Spark::Compiler::TokenValue> Semicolon LParen RParen LBracket RBracket LBrace RBrace
%token <Spark::Compiler::TokenValue> At
%token <Spark::Compiler::TokenValue> Dollar
%token <Spark::Compiler::TokenValue> LineComment BlockComment

%token Error
%token EndOfFile 0

%type <Spark::Compiler::AST::Node*> start

%type <Spark::Compiler::AST::Stmt*> stmt
%type <Spark::Compiler::AST::VarDeclStmt*> let
%type <Spark::Compiler::AST::WhileStmt*> while

%type <Spark::Compiler::AST::Expr*> expr
%type <Spark::Compiler::AST::Expr*> primary
%type <Spark::Compiler::AST::Expr*> literal

%type <Spark::Compiler::AST::BlockStmt*> block
%type <std::vector<Spark::Compiler::AST::Stmt*>> block_stmts

%type <Spark::Compiler::AST::TypeNode*> type
%type <Spark::Compiler::AST::TypeSegment*> type_segment
%type <Spark::Compiler::AST::TypePath*> type_path
%type <Spark::Compiler::AST::TypeModifiers> type_modifiers

%type <std::vector<Spark::Compiler::AST::Node*>> lambda_body

%%
start:
      /* empty */
        {
            auto* root = ctx.ast.root();
            root->stmts.clear();
            $$ = root;
        }
    | start stmt
        {
            auto* root = ctx.ast.root();
            root->stmts.push_back(static_cast<Stmt*>($2));
            $$ = root;
        }
    ;

stmt:
      block
      while
    ;

block:
      Do block_stmts End
        {
            auto* block = ctx.makeNode<BlockStmt>($1.line, $1.column);
            block->stmts.insert(block->stmts.end(), $2.begin(), $2.end());
            $$ = block;
        }
    ;

block_stmts:
      /* empty */
        {
            $$.clear();
        }
    | block_stmts stmt
        {
            $$ = std::move($1);
            $$.push_back($2);
        }
    ;

let:
    ;

while:
      While expr block
        {
            $$ = ctx.makeNode<WhileStmt>($1.line, $1.column, $2, $3);
        }
    ;

expr:
      primary    { $$ = $1; }
    ;

primary:
      literal            { $$ = $1; }
    | Identifier         { $$ = ctx.makeNode<VarExpr>($1.line, $1.column, $1.lexeme); }
    | LParen expr RParen { $$ = $2; }
    ;

literal:
      Integer    { $$ = ctx.makeNode<IntLiteralExpr>($1.line, $1.column, BigInt($1.lexeme)); }
    | Real       { $$ = ctx.makeNode<RealLiteralExpr>($1.line, $1.column, BigReal($1.lexeme)); }
    | String     { $$ = ctx.makeNode<StringLiteralExpr>($1.line, $1.column, $1.lexeme); }
    | True       { $$ = ctx.makeNode<BoolLiteralExpr>($1.line, $1.column, true); }
    | False      { $$ = ctx.makeNode<BoolLiteralExpr>($1.line, $1.column, false); }
    | Nil        { $$ = ctx.makeNode<NilLiteralExpr>($1.line, $1.column); }
    ;

type:
      type_path    { $$ = $1; }
    ;

type_segment:
      Identifier type_modifiers
        {
            $$ = ctx.makeNode<TypeSegment>($1.line, $1.column, $1.lexeme,
                $2.isImmutable, $2.isNullable);
        }
    ;

type_path:
      type_segment
        {
            auto* path = ctx.makeNode<TypePath>($1->line, $1->column);
            path->segments.push_back($1);
            $$ = path;
        }
    | type_path Dot type_segment
        {
            $1->segments.push_back($3);
            $$ = $1;
        }
    ;

type_modifiers:
      type_modifiers BitXor
        {
            $$ = $1;
            $$.isImmutable = true;
        }
    | type_modifiers Question
        {
            $$ = $1;
            $$.isNullable = true;
        }
    | type_modifiers Coalesce
        {
            $$ = $1;
            $$.isNullable = true;
        }
    | /* empty */
        {
            $$.isImmutable = false;
            $$.isNullable = false;
        }
    ;
%%

void yy::parser::error(const std::string& msg) {
	ctx.error = ParserError(msg, lstate.line, lstate.column);
}
