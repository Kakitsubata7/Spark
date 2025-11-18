%require "3.5"

%skeleton "lalr1.cc"

%language "c++"

%define api.token.raw
%define api.value.type variant

%lex-param {yyscan_t scanner}
%parse-param {yyscan_t scanner} {Spark::FrontEnd::ParserContext& ctx}

%code requires {
#include <vector>

#include "frontend/ast/node.hpp"
#include "frontend/ast/stmt.hpp"
#include "frontend/parser_context.hpp"
#include "frontend/token_value.hpp"

typedef void* yyscan_t;
}

%code {
using namespace Spark::FrontEnd;
using namespace Spark::FrontEnd::AST;

int yylex(yy::parser::semantic_type* yylval, yyscan_t scanner);
void yyerror(yyscan_t scanner, ParserContext& ctx, const char* msg);
}

%token <Spark::FrontEnd::TokenValue> Identifier Discard
%token <Spark::FrontEnd::TokenValue> Integer Real String
%token <Spark::FrontEnd::TokenValue> Alias As Break Case Catch Class Const Constructor Continue Cref Destructor Do Else End Enum Export Extension False Fn For From Global If Import In Is Let Match Module Nil Operator Ref Return Self Struct Super Then Throw Trait True Try Typeof Undefined While Yield
%token <Spark::FrontEnd::TokenValue> Add Sub Mul Div Mod
%token <Spark::FrontEnd::TokenValue> BitNot BitAnd BitOr BitXor BitShl BitShr
%token <Spark::FrontEnd::TokenValue> LogNot LogAnd LogOr
%token <Spark::FrontEnd::TokenValue> Eq Ne Lt Gt Le Ge
%token <Spark::FrontEnd::TokenValue> StrictEq StrictNe
%token <Spark::FrontEnd::TokenValue> Range RangeExcl
%token <Spark::FrontEnd::TokenValue> Question Coalesce
%token <Spark::FrontEnd::TokenValue> Assign AddAssign SubAssign MulAssign DivAssign ModAssign BitAndAssign BitOrAssign BitXorAssign BitShlAssign BitShrAssign CoalesceAssign
%token <Spark::FrontEnd::TokenValue> Dot
%token <Spark::FrontEnd::TokenValue> Comma Colon Arrow FatArrow
%token <Spark::FrontEnd::TokenValue> Semicolon LParen RParen LBracket RBracket LBrace RBrace
%token <Spark::FrontEnd::TokenValue> At
%token <Spark::FrontEnd::TokenValue> Dollar
%token <Spark::FrontEnd::TokenValue> LineComment BlockComment

%token Error
%token EndOfFile 0

%type <Spark::FrontEnd::AST::Node*> start

%type <Spark::FrontEnd::AST::Stmt*> stmt
%type <Spark::FrontEnd::AST::VarDeclStmt*> let_stmt

%type <Spark::FrontEnd::AST::Expr*> expr
%type <Spark::FrontEnd::AST::Expr*> primary
%type <Spark::FrontEnd::AST::Expr*> literal

%type <Spark::FrontEnd::AST::BlockStmt*> block
%type <std::vector<Spark::FrontEnd::AST::Stmt*>> block_stmts

%type <Spark::FrontEnd::AST::TypeNode*> type
%type <Spark::FrontEnd::AST::TypeSegment*> type_segment
%type <Spark::FrontEnd::AST::TypePath*> type_path

%type <std::vector<Spark::FrontEnd::AST::Node*>> lambda_body

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

let_stmt:


expr:
      primary    { $$ = $1; }
    ;

primary:
      literal            { $$ = $1; }
    | Identifier         { $$ = ctx.makeNode<VarExpr>($1.line, $1.column, $1.lexeme); }
    | LParen expr RParen { $$ = $2; }
    ;

literal:
      Integer    { $$->node = ctx.makeNode<IntLiteralExpr>($1.line, $1.column, BigInt($1.lexeme)); }
    | Real       { $$->node = ctx.makeNode<RealLiteralExpr>($1.line, $1.column, BigReal($1.lexeme)); }
    | String     { $$->node = ctx.makeNode<StringLiteralExpr>($1.line, $1.column, $1.lexeme); }
    | True       { $$->node = ctx.makeNode<BoolLiteralExpr>($1.line, $1.column, true); }
    | False      { $$->node = ctx.makeNode<BoolLiteralExpr>($1.line, $1.column, false); }
    | Nil        { $$->node = ctx.makeNode<NilLiteralExpr>($1.line, $1.column); }
    ;

/*type:
      type_path    { $$ = $1; }
    ;

type_segment:
      Identifier segment
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
    ;*/
%%
