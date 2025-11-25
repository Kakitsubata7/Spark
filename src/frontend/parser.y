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

%token EndOfFile 0
%token Error 1

%type <Spark::FrontEnd::Node*> start

%type <Spark::FrontEnd::Stmt*> stmt
%type <Spark::FrontEnd::BlockStmt*> block
%type <std::vector<Spark::FrontEnd::Stmt*>> block_stmts
%type <Spark::FrontEnd::Stmt*> assign_stmt
%type <Spark::FrontEnd::Stmt*> let
%type <Spark::FrontEnd::Stmt*> while

%type <Spark::FrontEnd::Expr*> expr
%type <Spark::FrontEnd::Expr*> assign_expr
%type <Spark::FrontEnd::Expr*> logical_or_expr
%type <Spark::FrontEnd::Expr*> logical_and_expr
%type <Spark::FrontEnd::Expr*> bit_or_expr
%type <Spark::FrontEnd::Expr*> bit_xor_expr
%type <Spark::FrontEnd::Expr*> bit_and_expr
%type <Spark::FrontEnd::Expr*> equality_expr
%type <Spark::FrontEnd::Expr*> relational_expr
%type <Spark::FrontEnd::Expr*> shift_expr
%type <Spark::FrontEnd::Expr*> additive_expr
%type <Spark::FrontEnd::Expr*> multiplicative_expr
%type <Spark::FrontEnd::Expr*> unary_expr
%type <Spark::FrontEnd::Expr*> postfix_expr
%type <Spark::FrontEnd::Expr*> lhs_expr
%type <Spark::FrontEnd::Expr*> primary
%type <Spark::FrontEnd::Expr*> literal


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
    | assign_stmt
    | while
    ;

block:
      Do block_stmts End
        {
            auto* block = ctx.makeNode<BlockStmt>($1.start, $1.end);
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

assign_stmt:
      lhs_expr Assign assign_expr
        {
            $$ = ctx.makeNode<AssignStmt>($1->start, $3->end, AssignType::Assign, $1, $3 );
        }
    ;

let:
    ;

while:
      While expr block
        {
            $$ = ctx.makeNode<WhileStmt>($1.start, $1.end, $2, $3);
        }
    ;

expr:
      logical_or_expr { $$ = $1; }
    ;

assign_expr:
      logical_or_expr { $$ = $1; }
    | lhs_expr Assign assign_expr
        {
            $$ = ctx.makeNode<AssignExpr>($1->start, $3->end, AssignType::Assign, $1, $3);
        }
    | lhs_expr AddAssign assign_expr
        {
            $$ = ctx.makeNode<AssignExpr>($1->start, $3->end, AssignType::AddAssign, $1, $3);
        }
    | lhs_expr SubAssign assign_expr
        {
            $$ = ctx.makeNode<AssignExpr>($1->start, $3->end, AssignType::SubAssign, $1, $3);
        }
    | lhs_expr MulAssign assign_expr
        {
            $$ = ctx.makeNode<AssignExpr>($1->start, $3->end, AssignType::MulAssign, $1, $3);
        }
    | lhs_expr DivAssign assign_expr
        {
            $$ = ctx.makeNode<AssignExpr>($1->start, $3->end, AssignType::DivAssign, $1, $3);
        }
    | lhs_expr ModAssign assign_expr
        {
            $$ = ctx.makeNode<AssignExpr>($1->start, $3->end, AssignType::ModAssign, $1, $3);
        }
    | lhs_expr BitAndAssign assign_expr
        {
            $$ = ctx.makeNode<AssignExpr>($1->start, $3->end, AssignType::BitAndAssign, $1, $3);
        }
    | lhs_expr BitOrAssign assign_expr
        {
            $$ = ctx.makeNode<AssignExpr>($1->start, $3->end, AssignType::BitOrAssign, $1, $3);
        }
    | lhs_expr BitXorAssign assign_expr
        {
            $$ = ctx.makeNode<AssignExpr>($1->start, $3->end, AssignType::BitXorAssign, $1, $3);
        }
    | lhs_expr BitShlAssign assign_expr
        {
            $$ = ctx.makeNode<AssignExpr>($1->start, $3->end, AssignType::BitShlAssign, $1, $3);
        }
    | lhs_expr BitShrAssign assign_expr
        {
            $$ = ctx.makeNode<AssignExpr>($1->start, $3->end, AssignType::BitShrAssign, $1, $3);
        }
    | lhs_expr CoalesceAssign assign_expr
        {
            $$ = ctx.makeNode<AssignExpr>($1->start, $3->end, AssignType::CoalesceAssign, $1, $3);
        }
    ;

lhs_expr:
      Identifier             { $$ = ctx.makeNode<VarExpr>($1.start, $1.end, $1.lexeme); }
    | LParen lhs_expr RParen { $$ = $2; }
    ;

logical_or_expr:
      logical_and_expr   { $$ = $1; }
    | logical_or_expr LogOr logical_and_expr
        {
            $$ = ctx.makeNode<BinaryExpr>($1->start, $3->end, BinaryExpr::OperatorType::LogOr, $1, $3);
        }
    ;

logical_and_expr:
      bit_or_expr        { $$ = $1; }
    | logical_and_expr LogAnd bit_or_expr
        {
            $$ = ctx.makeNode<BinaryExpr>($1->start, $3->end, BinaryExpr::OperatorType::LogAnd, $1, $3);
        }
    ;

bit_or_expr:
      bit_xor_expr       { $$ = $1; }
    | bit_or_expr BitOr bit_xor_expr
        {
            $$ = ctx.makeNode<BinaryExpr>($1->start, $3->end, BinaryExpr::OperatorType::BitOr, $1, $3);
        }
    ;

bit_xor_expr:
      bit_and_expr       { $$ = $1; }
    | bit_xor_expr BitXor bit_and_expr
        {
            $$ = ctx.makeNode<BinaryExpr>($1->start, $3->end, BinaryExpr::OperatorType::BitXor, $1, $3);
        }
    ;

bit_and_expr:
      equality_expr      { $$ = $1; }
    | bit_and_expr BitAnd equality_expr
        {
            $$ = ctx.makeNode<BinaryExpr>($1->start, $3->end, BinaryExpr::OperatorType::BitAnd, $1, $3);
        }
    ;

equality_expr:
      relational_expr    { $$ = $1; }
    | equality_expr Eq relational_expr
        {
            $$ = ctx.makeNode<BinaryExpr>($1->start, $3->end, BinaryExpr::OperatorType::Eq, $1, $3);
        }
    | equality_expr Ne relational_expr
        {
            $$ = ctx.makeNode<BinaryExpr>($1->start, $3->end, BinaryExpr::OperatorType::Ne, $1, $3);
        }
    | equality_expr StrictEq relational_expr
        {
            $$ = ctx.makeNode<BinaryExpr>($1->start, $3->end, BinaryExpr::OperatorType::StrictEq, $1, $3);
        }
    | equality_expr StrictNe relational_expr
        {
            $$ = ctx.makeNode<BinaryExpr>($1->start, $3->end, BinaryExpr::OperatorType::StrictNe, $1, $3);
        }
    ;

relational_expr:
      shift_expr         { $$ = $1; }
    | relational_expr Lt shift_expr
        {
            $$ = ctx.makeNode<BinaryExpr>($1->start, $3->end, BinaryExpr::OperatorType::Lt, $1, $3);
        }
    | relational_expr Gt shift_expr
        {
            $$ = ctx.makeNode<BinaryExpr>($1->start, $3->end, BinaryExpr::OperatorType::Gt, $1, $3);
        }
    | relational_expr Le shift_expr
        {
            $$ = ctx.makeNode<BinaryExpr>($1->start, $3->end, BinaryExpr::OperatorType::Le, $1, $3);
        }
    | relational_expr Ge shift_expr
        {
            $$ = ctx.makeNode<BinaryExpr>($1->start, $3->end, BinaryExpr::OperatorType::Ge, $1, $3);
        }
    ;

shift_expr:
      additive_expr      { $$ = $1; }
    | shift_expr BitShl additive_expr
        {
            $$ = ctx.makeNode<BinaryExpr>($1->start, $3->end, BinaryExpr::OperatorType::BitShl, $1, $3);
        }
    | shift_expr BitShr additive_expr
        {
            $$ = ctx.makeNode<BinaryExpr>($1->start, $3->end, BinaryExpr::OperatorType::BitShr, $1, $3);
        }
    ;

additive_expr:
      multiplicative_expr { $$ = $1; }
    | additive_expr Add multiplicative_expr
        {
            $$ = ctx.makeNode<BinaryExpr>($1->start, $3->end, BinaryExpr::OperatorType::Add, $1, $3);
        }
    | additive_expr Sub multiplicative_expr
        {
            $$ = ctx.makeNode<BinaryExpr>($1->start, $3->end, BinaryExpr::OperatorType::Sub, $1, $3);
        }
    ;

multiplicative_expr:
      unary_expr         { $$ = $1; }
    | multiplicative_expr Mul unary_expr
        {
            $$ = ctx.makeNode<BinaryExpr>($1->start, $3->end, BinaryExpr::OperatorType::Mul, $1, $3);
        }
    | multiplicative_expr Div unary_expr
        {
            $$ = ctx.makeNode<BinaryExpr>($1->start, $3->end, BinaryExpr::OperatorType::Div, $1, $3);
        }
    | multiplicative_expr Mod unary_expr
        {
            $$ = ctx.makeNode<BinaryExpr>($1->start, $3->end, BinaryExpr::OperatorType::Mod, $1, $3);
        }
    ;

unary_expr:
      postfix_expr       { $$ = $1; }
    | Add unary_expr
        {
            $$ = ctx.makeNode<UnaryExpr>($1.start, $2->end, UnaryExpr::OperatorType::Pos, $2);
        }
    | Sub unary_expr
        {
            $$ = ctx.makeNode<UnaryExpr>($1.start, $2->end, UnaryExpr::OperatorType::Neg, $2);
        }
    | LogNot unary_expr
        {
            $$ = ctx.makeNode<UnaryExpr>($1.start, $2->end, UnaryExpr::OperatorType::LogNot, $2);
        }
    | BitNot unary_expr
        {
            $$ = ctx.makeNode<UnaryExpr>($1.start, $2->end, UnaryExpr::OperatorType::BitNot, $2);
        }
    ;

postfix_expr:
      primary            { $$ = $1; }
    ;

primary:
      literal            { $$ = $1; }
    | Identifier         { $$ = ctx.makeNode<VarExpr>($1.start, $1.end, $1.lexeme); }
    | LParen expr RParen { $$ = $2; }
    | If expr Then expr Else expr
        {
            $$ = ctx.makeNode<IfThenExpr>($1.start, $6->end, $2, $4, $6);
        }
    ;

literal:
      Integer    { $$ = ctx.makeNode<IntLiteralExpr>($1.start, $1.end, BigInt($1.lexeme)); }
    | Real       { $$ = ctx.makeNode<RealLiteralExpr>($1.start, $1.end, BigReal($1.lexeme)); }
    | String     { $$ = ctx.makeNode<StringLiteralExpr>($1.start, $1.end, $1.lexeme); }
    | True       { $$ = ctx.makeNode<BoolLiteralExpr>($1.start, $1.end, true); }
    | False      { $$ = ctx.makeNode<BoolLiteralExpr>($1.start, $1.end, false); }
    | Nil        { $$ = ctx.makeNode<NilLiteralExpr>($1.start, $1.end); }
    ;

type:
      type_path    { $$ = $1; }
    ;

type_segment:
      Identifier type_modifiers
        {
            $$ = ctx.makeNode<TypeSegment>($1.start, $1.end, $1.lexeme,
                $2.isImmutable, $2.isNullable);
        }
    ;

type_path:
      type_segment
        {
            auto* path = ctx.makeNode<TypePath>($1->start, $1->end);
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

void yy::parser::error(const yy::location& loc, const std::string& msg) {
    Location start(static_cast<size_t>(loc.begin.line), static_cast<size_t>(loc.begin.column));
    Location end(static_cast<size_t>(loc.end.line), static_cast<size_t>(loc.end.column));
    ctx.addError(msg, start, end);
}
