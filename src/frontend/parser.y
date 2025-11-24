%require "3.5"

%skeleton "lalr1.cc"

%language "c++"

%define api.token.raw
%define api.value.type variant
%define parse.error verbose

%lex-param {yyscan_t scanner}
%parse-param {yyscan_t scanner} {Spark::FrontEnd::LexerState& lstate} {Spark::FrontEnd::ParserContext& ctx}

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

int yylex(yy::parser::semantic_type* yylval, yyscan_t scanner);
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

%type <Spark::FrontEnd::Node*> start

%type <Spark::FrontEnd::Stmt*> stmt
%type <Spark::FrontEnd::VarDeclStmt*> let
%type <Spark::FrontEnd::WhileStmt*> while

%type <Spark::FrontEnd::Expr*> expr
%type <Spark::FrontEnd::Expr*> primary
%type <Spark::FrontEnd::Expr*> literal

%type <Spark::FrontEnd::BlockStmt*> block
%type <std::vector<Spark::FrontEnd::Stmt*>> block_stmts

%type <Spark::FrontEnd::TypeNode*> type
%type <Spark::FrontEnd::TypeSegment*> type_segment
%type <Spark::FrontEnd::TypePath*> type_path
%type <Spark::FrontEnd::TypeModifiers> type_modifiers

%type <std::vector<Spark::FrontEnd::Node*>> lambda_body

%%
start:
      /* empty */
        {
            auto* root = ctx.ast().root();
            root->stmts.clear();
            $$ = root;
        }
    | start stmt
        {
            auto* root = ctx.ast().root();
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
            auto* block = ctx.makeNode<BlockStmt>($1.lineno, $1.columnno);
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
            $$ = ctx.makeNode<WhileStmt>($1.lineno, $1.columnno, $2, $3);
        }
    ;

expr:
      primary    { $$ = $1; }
    ;

primary:
      literal            { $$ = $1; }
    | Identifier         { $$ = ctx.makeNode<VarExpr>($1.lineno, $1.columnno, $1.lexeme); }
    | LParen expr RParen { $$ = $2; }
    ;

literal:
      Integer    { $$ = ctx.makeNode<IntLiteralExpr>($1.lineno, $1.columnno, BigInt($1.lexeme)); }
    | Real       { $$ = ctx.makeNode<RealLiteralExpr>($1.lineno, $1.columnno, BigReal($1.lexeme)); }
    | String     { $$ = ctx.makeNode<StringLiteralExpr>($1.lineno, $1.columnno, $1.lexeme); }
    | True       { $$ = ctx.makeNode<BoolLiteralExpr>($1.lineno, $1.columnno, true); }
    | False      { $$ = ctx.makeNode<BoolLiteralExpr>($1.lineno, $1.columnno, false); }
    | Nil        { $$ = ctx.makeNode<NilLiteralExpr>($1.lineno, $1.columnno); }
    ;

type:
      type_path    { $$ = $1; }
    ;

type_segment:
      Identifier type_modifiers
        {
            $$ = ctx.makeNode<TypeSegment>($1.lineno, $1.columnno, $1.lexeme,
                $2.isImmutable, $2.isNullable);
        }
    ;

type_path:
      type_segment
        {
            auto* path = ctx.makeNode<TypePath>($1->lineno, $1->column);
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
    // TODO
}
