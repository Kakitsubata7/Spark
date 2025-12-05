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
%token <Spark::FrontEnd::TokenValue> Tide And BitOr BitXor
%token <Spark::FrontEnd::TokenValue> Bang LogAnd LogOr
%token <Spark::FrontEnd::TokenValue> Eq Ne Lt Gt Le Ge
%token <Spark::FrontEnd::TokenValue> StrictEq StrictNe
%token <Spark::FrontEnd::TokenValue> Range RangeExcl
%token <Spark::FrontEnd::TokenValue> Question Coalesce
%token <Spark::FrontEnd::TokenValue> Assign AddAssign SubAssign MulAssign DivAssign ModAssign BitAndAssign BitOrAssign BitXorAssign CoalesceAssign
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
%type <Spark::FrontEnd::Stmt*> assign
%type <Spark::FrontEnd::Stmt*> call_stmt
%type <Spark::FrontEnd::CallExpr*> postfix_call
%type <Spark::FrontEnd::Stmt*> while

%type <Spark::FrontEnd::Expr*> lhs
%type <Spark::FrontEnd::Expr*> lhs_atom

%type <Spark::FrontEnd::Expr*> expr
%type <Spark::FrontEnd::Expr*> coalesce_expr
%type <Spark::FrontEnd::Expr*> logical_or_expr
%type <Spark::FrontEnd::Expr*> logical_and_expr
%type <Spark::FrontEnd::Expr*> bit_or_expr
%type <Spark::FrontEnd::Expr*> bit_xor_expr
%type <Spark::FrontEnd::Expr*> bit_and_expr
%type <Spark::FrontEnd::Expr*> equality_expr
%type <Spark::FrontEnd::Expr*> relational_expr
%type <Spark::FrontEnd::Expr*> range_expr
%type <Spark::FrontEnd::Expr*> shift_expr
%type <Spark::FrontEnd::Expr*> additive_expr
%type <Spark::FrontEnd::Expr*> multiplicative_expr
%type <Spark::FrontEnd::Expr*> unary_expr
%type <Spark::FrontEnd::Expr*> postfix_expr
%type <std::vector<Spark::FrontEnd::Expr*>> call_args
%type <std::vector<Spark::FrontEnd::Expr*>> subscript_args
%type <Spark::FrontEnd::Expr*> primary
%type <Spark::FrontEnd::Expr*> literal

%nonassoc EXPR_BASE

%left Lt Gt Le Ge
%left SHIFTOP

%%
/*
 * Root
 */
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
            if ($2) {
                root->stmts.push_back(static_cast<Stmt*>($2));
            }
            $$ = root;
        }
    ;

/*
 * Statements
 */
stmt:
      Semicolon    { $$ = nullptr; }
    | block        { $$ = $1; }
    | call_stmt
    | assign
    | while
    ;

/**
  * Block Statement
  */
block:
      Do block_stmts End
        {
            auto* block = ctx.makeNode<BlockStmt>($1.start, $1.end);
            block->stmts.insert(block->stmts.end(), $2.begin(), $2.end());
            $$ = block;
        }
    ;

block_stmts:
      /* empty */    { $$.clear(); }
    | block_stmts stmt
        {
            $$ = std::move($1);
            $$.push_back($2);
        }
    ;

/**
  * Assignment Statement
  */
assign:
      lhs Assign expr            { $$ = ctx.makeNode<AssignStmt>($1->start, $3->end, AssignStmt::OpType::Assign, $1, $3); }
    | lhs AddAssign expr         { $$ = ctx.makeNode<AssignStmt>($1->start, $3->end, AssignStmt::OpType::AddAssign, $1, $3); }
    | lhs SubAssign expr         { $$ = ctx.makeNode<AssignStmt>($1->start, $3->end, AssignStmt::OpType::SubAssign, $1, $3); }
    | lhs MulAssign expr         { $$ = ctx.makeNode<AssignStmt>($1->start, $3->end, AssignStmt::OpType::MulAssign, $1, $3); }
    | lhs DivAssign expr         { $$ = ctx.makeNode<AssignStmt>($1->start, $3->end, AssignStmt::OpType::DivAssign, $1, $3); }
    | lhs ModAssign expr         { $$ = ctx.makeNode<AssignStmt>($1->start, $3->end, AssignStmt::OpType::ModAssign, $1, $3); }
    | lhs BitAndAssign expr      { $$ = ctx.makeNode<AssignStmt>($1->start, $3->end, AssignStmt::OpType::BitAndAssign, $1, $3); }
    | lhs BitOrAssign expr       { $$ = ctx.makeNode<AssignStmt>($1->start, $3->end, AssignStmt::OpType::BitOrAssign, $1, $3); }
    | lhs BitXorAssign expr      { $$ = ctx.makeNode<AssignStmt>($1->start, $3->end, AssignStmt::OpType::BitXorAssign, $1, $3); }
    | lhs CoalesceAssign expr    { $$ = ctx.makeNode<AssignStmt>($1->start, $3->end, AssignStmt::OpType::CoalesceAssign, $1, $3); }
    ;

/**
  * Call Statement
  */
call_stmt:
      postfix_call
        {
            $$ = ctx.makeNode<CallStmt>($1->start, $1->end, $1->callable, std::move($1->args));
            // TODO: Deallocate $1
        }
    ;

postfix_call:
      postfix_expr LParen call_args RParen
        {
            $$ = ctx.makeNode<CallExpr>($1->start, $4.end, $1, std::move($3), false);
        }
    | postfix_expr Question LParen call_args RParen
        {
            $$ = ctx.makeNode<CallExpr>($1->start, $5.end, $1, std::move($4), true);
        }
    ;

while:
      While expr block    { $$ = ctx.makeNode<WhileStmt>($1.start, $1.end, $2, $3); }
    ;

/**
  * L-value Expressions
  */
lhs:
      lhs_atom
    | lhs Dot Identifier
    | lhs LBracket subscript_args RBracket
    | lhs LParen call_args RParen
    ;

lhs_atom:
      Identifier            { $$ = ctx.makeNode<VarExpr>($1.start, $1.end, $1.lexeme); }
    | Dollar Identifier
        {
            if ($1.end.lineno == $2.start.lineno && $1.end.columnno + 1 == $2.start.columnno) {
                $$ = ctx.makeNode<UpvalueVarExpr>($1.start, $1.end, $1.lexeme);
            } else {
                RAISE_ERROR($1.start, $2.end, "space between '$' and identifier is not allowed");
                $$ = ctx.makeNode<VarExpr>($2.start, $2.end, $2.lexeme);
            }
        }
    | LParen lhs RParen    { $$ = $2; }
    ;

/**
  * Expressions
  */
expr:
      coalesce_expr    %prec EXPR_BASE
    | If expr Then expr Else expr      { $$ = ctx.makeNode<IfThenExpr>($1.start, $6->end, $2, $4, $6); }
    | Try expr Else expr               { $$ = ctx.makeNode<TryElseExpr>($1.start, $4->end, $2, $4); }
    ;

/**
  * Null-Coalescing Expression
  */
coalesce_expr:
      logical_or_expr
    | coalesce_expr Coalesce logical_or_expr
        {
            $$ = ctx.makeNode<BinaryExpr>($1->start, $3->end, BinaryExpr::OpType::Coalesce, $1, $3);
        }
    ;

/**
  * Logical Or Expression (||)
  */
logical_or_expr:
      logical_and_expr
    | logical_or_expr LogOr logical_and_expr
        {
            $$ = ctx.makeNode<BinaryExpr>($1->start, $3->end, BinaryExpr::OpType::LogOr, $1, $3);
        }
    ;

/**
  * Logical And Expression (&&)
  */
logical_and_expr:
      bit_or_expr
    | logical_and_expr LogAnd bit_or_expr
        {
            $$ = ctx.makeNode<BinaryExpr>($1->start, $3->end, BinaryExpr::OpType::LogAnd, $1, $3);
        }
    ;

/**
  * Bitwise-Or Expression (|)
  */
bit_or_expr:
      bit_xor_expr
    | bit_or_expr BitOr bit_xor_expr
        {
            $$ = ctx.makeNode<BinaryExpr>($1->start, $3->end, BinaryExpr::OpType::BitOr, $1, $3);
        }
    ;

/**
  * Bitwise-Xor Expression (^)
  */
bit_xor_expr:
      bit_and_expr
    | bit_xor_expr BitXor bit_and_expr
        {
            $$ = ctx.makeNode<BinaryExpr>($1->start, $3->end, BinaryExpr::OpType::BitXor, $1, $3);
        }
    ;

/**
  * Bitwise-And Expression (&)
  */
bit_and_expr:
      equality_expr
    | bit_and_expr And equality_expr
        {
            $$ = ctx.makeNode<BinaryExpr>($1->start, $3->end, BinaryExpr::OpType::BitAnd, $1, $3);
        }
    ;

/**
  * Equality Expressions (==, !=, ===, !==)
  */
equality_expr:
      relational_expr
    | equality_expr Eq relational_expr          { $$ = ctx.makeNode<BinaryExpr>($1->start, $3->end, BinaryExpr::OpType::Eq, $1, $3); }
    | equality_expr Ne relational_expr          { $$ = ctx.makeNode<BinaryExpr>($1->start, $3->end, BinaryExpr::OpType::Ne, $1, $3); }
    | equality_expr StrictEq relational_expr    { $$ = ctx.makeNode<BinaryExpr>($1->start, $3->end, BinaryExpr::OpType::StrictEq, $1, $3); }
    | equality_expr StrictNe relational_expr    { $$ = ctx.makeNode<BinaryExpr>($1->start, $3->end, BinaryExpr::OpType::StrictNe, $1, $3); }
    ;

/**
  * Relational Expressions (<, >, <=, >=)
  */
relational_expr:
      range_expr
    | relational_expr Lt shift_expr    { $$ = ctx.makeNode<BinaryExpr>($1->start, $3->end, BinaryExpr::OpType::Lt, $1, $3); }
    | relational_expr Gt shift_expr    { $$ = ctx.makeNode<BinaryExpr>($1->start, $3->end, BinaryExpr::OpType::Gt, $1, $3); }
    | relational_expr Le shift_expr    { $$ = ctx.makeNode<BinaryExpr>($1->start, $3->end, BinaryExpr::OpType::Le, $1, $3); }
    | relational_expr Ge shift_expr    { $$ = ctx.makeNode<BinaryExpr>($1->start, $3->end, BinaryExpr::OpType::Ge, $1, $3); }
    ;

/**
  * Range Expressions (..., ..<)
  */
range_expr:
      shift_expr
    | shift_expr Range shift_expr                         { $$ = ctx.makeNode<RangeExpr>($1->start, $3->end, $1, $3, nullptr, false); }
    | shift_expr Range shift_expr Colon shift_expr        { $$ = ctx.makeNode<RangeExpr>($1->start, $5->end, $1, $3, $5, false); }
    | shift_expr RangeExcl shift_expr                     { $$ = ctx.makeNode<RangeExpr>($1->start, $3->end, $1, $3, nullptr, true); }
    | shift_expr RangeExcl shift_expr Colon shift_expr    { $$ = ctx.makeNode<RangeExpr>($1->start, $5->end, $1, $3, $5, true); }
    ;

/**
  * Shift Expressions (<<, >>)
  */
shift_expr:
      additive_expr
    | shift_expr Lt Lt additive_expr    %prec SHIFTOP
        {
            if ($2.end.lineno != $3.start.lineno || $2.end.columnno + 1 != $3.start.columnno) {
                RAISE_ERROR($3.start, $3.end, "unexpected <");
            }
            $$ = ctx.makeNode<BinaryExpr>($1->start, $4->end, BinaryExpr::OpType::BitShl, $1, $4);
        }
    | shift_expr Gt Gt additive_expr    %prec SHIFTOP
        {
           if ($2.end.lineno != $3.start.lineno || $2.end.columnno + 1 != $3.start.columnno) {
               RAISE_ERROR($3.start, $3.end, "unexpected >");
           }
           $$ = ctx.makeNode<BinaryExpr>($1->start, $4->end, BinaryExpr::OpType::BitShr, $1, $4);
        }
    ;

/**
  * Additive Expressions (+, -)
  */
additive_expr:
      multiplicative_expr
    | additive_expr Add multiplicative_expr    { $$ = ctx.makeNode<BinaryExpr>($1->start, $3->end, BinaryExpr::OpType::Add, $1, $3); }
    | additive_expr Sub multiplicative_expr    { $$ = ctx.makeNode<BinaryExpr>($1->start, $3->end, BinaryExpr::OpType::Sub, $1, $3); }
    ;

/**
  * Multiplicative Expressions (*, /, %)
  */
multiplicative_expr:
      unary_expr
    | multiplicative_expr Mul unary_expr    { $$ = ctx.makeNode<BinaryExpr>($1->start, $3->end, BinaryExpr::OpType::Mul, $1, $3); }
    | multiplicative_expr Div unary_expr    { $$ = ctx.makeNode<BinaryExpr>($1->start, $3->end, BinaryExpr::OpType::Div, $1, $3); }
    | multiplicative_expr Mod unary_expr    { $$ = ctx.makeNode<BinaryExpr>($1->start, $3->end, BinaryExpr::OpType::Mod, $1, $3); }
    ;

/**
  * Unary Expressions (Prefix)
  */
unary_expr:
      postfix_expr       { $$ = $1; }
    | Add unary_expr     { $$ = ctx.makeNode<UnaryExpr>($1.start, $2->end, UnaryExpr::OpType::Pos, $2); }
    | Sub unary_expr     { $$ = ctx.makeNode<UnaryExpr>($1.start, $2->end, UnaryExpr::OpType::Neg, $2); }
    | Tide unary_expr    { $$ = ctx.makeNode<UnaryExpr>($1.start, $2->end, UnaryExpr::OpType::BitNot, $2); }
    | Bang unary_expr    { $$ = ctx.makeNode<UnaryExpr>($1.start, $2->end, UnaryExpr::OpType::LogNot, $2); }
    | And unary_expr     { $$ = ctx.makeNode<UnaryExpr>($1.start, $2->end, UnaryExpr::OpType::Ref, $2); }
    ;

/**
  * Postfix Expressions
  */
postfix_expr:
      primary
    | postfix_expr Dot Identifier                               { $$ = ctx.makeNode<MemberAccessExpr>($1->start, $3.end, $1, $3.lexeme); }
    | postfix_expr Question Dot Identifier                      { $$ = ctx.makeNode<MemberAccessExpr>($1->start, $4.end, $1, $4.lexeme, true); }
    | postfix_expr LParen call_args RParen                      { $$ = ctx.makeNode<CallExpr>($1->start, $4.end, $1, std::move($3)); }
    | postfix_expr Question LParen call_args RParen             { $$ = ctx.makeNode<CallExpr>($1->start, $5.end, $1, std::move($4)); }
    | postfix_expr LBracket subscript_args RBracket             { $$ = ctx.makeNode<SubscriptExpr>($1->start, $4.end, $1, std::move($3)); }
    | postfix_expr Question LBracket subscript_args RBracket    { $$ = ctx.makeNode<SubscriptExpr>($1->start, $5.end, $1, std::move($4)); }
    | postfix_expr Bang                                         { $$ = ctx.makeNode<ForceUnwrapExpr>($1->start, $2.end, $1); }
    | postfix_expr Question                                     { $$ = ctx.makeNode<OptionalExpr>($1->start, $2.end, $1); }
    ;

call_args:
      /* empty */    { $$.clear(); }
    | expr
        {
            $$.clear();
            $$.push_back($1);
        }
    | call_args Comma expr
        {
            $$ = std::move($1);
            $$.push_back($3);
        }
    ;

subscript_args:
      expr
        {
            $$.clear();
            $$.push_back($1);
        }
    | subscript_args Comma expr
        {
            $$ = std::move($1);
            $$.push_back($3);
        }
    ;

/**
  * Primary Expressions
  * Atomic expressions that cannot be reduced further by the parser.
  */
primary:
      literal
    | Identifier            { $$ = ctx.makeNode<VarExpr>($1.start, $1.end, $1.lexeme); }
    | Dollar Identifier
        {
            if ($1.end.lineno == $2.start.lineno && $1.end.columnno + 1 == $2.start.columnno) {
                $$ = ctx.makeNode<UpvalueVarExpr>($1.start, $1.end, $1.lexeme);
            } else {
                RAISE_ERROR($1.start, $2.end, "space between '$' and identifier is not allowed");
                $$ = ctx.makeNode<VarExpr>($2.start, $2.end, $2.lexeme);
            }
        }
    | LParen expr RParen    { $$ = $2; }
    ;

/**
  * Literals
  */
literal:
      Integer    { $$ = ctx.makeNode<IntLiteralExpr>($1.start, $1.end, BigInt($1.lexeme)); }
    | Real       { $$ = ctx.makeNode<RealLiteralExpr>($1.start, $1.end, BigReal($1.lexeme)); }
    | String     { $$ = ctx.makeNode<StringLiteralExpr>($1.start, $1.end, $1.lexeme); }
    | True       { $$ = ctx.makeNode<BoolLiteralExpr>($1.start, $1.end, true); }
    | False      { $$ = ctx.makeNode<BoolLiteralExpr>($1.start, $1.end, false); }
    | Nil        { $$ = ctx.makeNode<NilLiteralExpr>($1.start, $1.end); }
    ;
%%

void yy::parser::error(const yy::location& loc, const std::string& msg) {
    Location start(static_cast<size_t>(loc.begin.line), static_cast<size_t>(loc.begin.column));
    Location end(static_cast<size_t>(loc.end.line), static_cast<size_t>(loc.end.column));
    ctx.addError(msg, start, end);
}
