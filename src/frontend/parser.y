%require "3.5"

%skeleton "lalr1.cc"

%language "c++"

%define api.token.raw
%define api.value.type variant
%define parse.error detailed

%locations

%lex-param {yyscan_t scanner}
%parse-param {yyscan_t scanner} {yy::parser::location_type* yylloc} {Spark::FrontEnd::AST& ast} {std::vector<Spark::Error>& errors}

%code requires {
#include <cstdint>
#include <vector>

#include "frontend/ast.hpp"
#include "frontend/lexer/token_value.hpp"
#include "utils/error.hpp"

typedef void* yyscan_t;

namespace yy {
    template <typename T>
    struct Symbol {
        Spark::Location start;
        Spark::Location end;
        T value;

        Symbol() = default;
        Symbol(Spark::Location start, Spark::Location end, T value) noexcept
            : start(start), end(end), value(std::move(value)) { }
    };
} // yy
}

%code {
#include <typeinfo>

using namespace Spark;
using namespace Spark::FrontEnd;

int yylex(yy::parser::semantic_type*, yy::parser::location_type*, yyscan_t);

#define RAISE_ERROR(start, end, msg) errors.emplace_back(msg, start, end)
#define REMOVE_LAST_ERROR() errors.pop_back()
}

%token <Spark::FrontEnd::TokenValue> Identifier Discard
%token <Spark::FrontEnd::TokenValue> Integer Real String
%token <Spark::FrontEnd::TokenValue> Alias As Break Case Catch Class Const Constructor Continue Cref Destructor Do Else End Enum Export Extension False Fn For From Global If Import In Is Let Match Module Nil Operator Ref Return Self Struct Super Then Throw Trait True Try Typeof Undefine While
%token <Spark::FrontEnd::TokenValue> Add Sub Mul Div Mod
%token <Spark::FrontEnd::TokenValue> Tide And VBar Caret
%token <Spark::FrontEnd::TokenValue> Bang LogAnd LogOr
%token <Spark::FrontEnd::TokenValue> Eq Ne StrictEq StrictNe Lt Le Gt Ge
%token <Spark::FrontEnd::TokenValue> Shl Shr
%token <Spark::FrontEnd::TokenValue> Range RangeExcl
%token <Spark::FrontEnd::TokenValue> Question NonNull Coalesce
%token <Spark::FrontEnd::TokenValue> Pipe
%token <Spark::FrontEnd::TokenValue> Assign AddAssign SubAssign MulAssign DivAssign ModAssign BitAndAssign BitOrAssign BitXorAssign ShlAssign ShrAssign CoalesceAssign
%token <Spark::FrontEnd::TokenValue> Dot
%token <Spark::FrontEnd::TokenValue> Comma Colon Arrow FatArrow
%token <Spark::FrontEnd::TokenValue> Semicolon LParen RParen LBracket RBracket LBrace RBrace
%token <Spark::FrontEnd::TokenValue> At
%token <Spark::FrontEnd::TokenValue> Dollar
%token <Spark::FrontEnd::TokenValue> LineComment BlockComment

%token EndOfFile 0
%token Error 1

%type <std::vector<Spark::FrontEnd::Node*>> nodes
%type <std::vector<Spark::FrontEnd::Node*>> node_list
%type <Spark::FrontEnd::Node*> node
%type <Spark::FrontEnd::Stmt*> stmt
%type <Spark::FrontEnd::VarModifier*> varmod
%type <Symbol<Spark::FrontEnd::VarModifier::VarKind>> varkind
%type <Spark::FrontEnd::VarDefStmt*> vardef_stmt
%type <Spark::FrontEnd::VarModifier*> opt_varmod
%type <bool> opt_immut
%type <std::vector<Spark::FrontEnd::Name>> opt_template
%type <Spark::FrontEnd::FnDefStmt*> fndef_stmt
%type <std::vector<Spark::FrontEnd::FnParam*>> param_clause
%type <std::vector<Spark::FrontEnd::FnParam*>> params
%type <Spark::FrontEnd::FnParam*> param
%type <Spark::FrontEnd::FnCaptureClause*> capture_clause
%type <Spark::FrontEnd::FnCaptureClause*> captures
%type <Spark::FrontEnd::FnCapture*> capture
%type <std::vector<Spark::FrontEnd::FnReturn*>> ret_clause
%type <std::vector<Spark::FrontEnd::FnReturn*>> rets
%type <Spark::FrontEnd::FnReturn*> ret
%type <std::pair<bool, Spark::FrontEnd::Expr*>> throw_clause
%type <Spark::FrontEnd::TypeDefStmt*> typedef_stmt
%type <Spark::FrontEnd::CaseDefStmt*> casedef_stmt
%type <Symbol<Spark::FrontEnd::TypeDefStmt::TypeKind>> typedef_kind
%type <Spark::FrontEnd::AssignStmt*> assign_stmt
%type <Spark::FrontEnd::Node*> assign_rhs
%type <Spark::FrontEnd::AssignStmt*> regular_assign
%type <Spark::FrontEnd::Node*> regular_rhs
%type <Spark::FrontEnd::IfStmt*> if_stmt
%type <Spark::FrontEnd::BlockExpr*> else_clause
%type <Spark::FrontEnd::ModuleStmt*> module_stmt
%type <Spark::FrontEnd::ImportStmt*> import_stmt
%type <Spark::FrontEnd::ImportItem*> import

%type <Spark::FrontEnd::Expr*> expr
%type <std::vector<Spark::FrontEnd::Expr*>> exprs
%type <Spark::FrontEnd::LambdaExpr*> lambda
%type <Spark::FrontEnd::MatchExpr*> match
%type <std::vector<Spark::FrontEnd::MatchCase*>> cases
%type <Spark::FrontEnd::MatchCase*> case
%type <Spark::FrontEnd::TryCatchExpr*> trycatch
%type <std::vector<Spark::FrontEnd::CatchClause*>> catches
%type <Spark::FrontEnd::CatchClause*> catch
%type <Spark::FrontEnd::BlockExpr*> block
%type <Spark::FrontEnd::Expr*> binary
%type <Spark::FrontEnd::Expr*> binary_pipe
%type <Spark::FrontEnd::Expr*> binary_type
%type <Spark::FrontEnd::Expr*> binary_range
%type <Spark::FrontEnd::Expr*> binary_coalesce
%type <Spark::FrontEnd::Expr*> binary_logor
%type <Spark::FrontEnd::Expr*> binary_logand
%type <Spark::FrontEnd::Expr*> binary_bitor
%type <Spark::FrontEnd::Expr*> binary_bitxor
%type <Spark::FrontEnd::Expr*> binary_bitand
%type <Spark::FrontEnd::Expr*> binary_eq
%type <Spark::FrontEnd::Expr*> binary_compare
%type <Spark::FrontEnd::Expr*> binary_bitshift
%type <Spark::FrontEnd::Expr*> binary_additive
%type <Spark::FrontEnd::Expr*> binary_multiplicative
%type <Spark::FrontEnd::Expr*> prefix
%type <Spark::FrontEnd::Expr*> postfix
%type <std::vector<Spark::FrontEnd::CallArg*>> call_args
%type <Spark::FrontEnd::CallArg*> call_arg
%type <Spark::FrontEnd::Expr*> primary
%type <Spark::FrontEnd::UpvalueExpr*> upvalue_expr
%type <Symbol<uint64_t>> dollars
%type <Spark::FrontEnd::TupleExpr*> tuple_expr
%type <Spark::FrontEnd::CollectionExpr*> collection_expr

%type <Spark::FrontEnd::Pattern*> pattern
%type <std::vector<Spark::FrontEnd::Pattern*>> patterns
%type <Spark::FrontEnd::CollectionPattern*> collection_pattern

%type <Symbol<Spark::FrontEnd::Literal>> literal
%type <Symbol<Spark::FrontEnd::Name>> name
%type <std::vector<Spark::FrontEnd::Name>> names
%type <Spark::FrontEnd::Path*> path
%type <Spark::FrontEnd::PathSeg*> path_seg

%%
%start program;

program:
      nodes  { ast.root = ast.make<BlockExpr>($1.front()->start, $1.back()->end, std::move($1)); }
    ;

nodes:
      /* empty */            { $$ = {}; }
    | node_list
    | node_list terminators
    ;

node_list:
      node                        { $$ = {}; $$.push_back($1); }
    | node_list terminators node  { $$ = std::move($1); $$.push_back($3); }
    | node_list error node
        {
            if (false) { // Replace condition with newline check
                yyerrok;
                REMOVE_LAST_ERROR();
                $$ = std::move($1);
                $$.push_back($3);
            } else {
                YYERROR;
            }
        }
    ;

terminator:
      Semicolon
    ;

terminators:
      terminator
    | terminators terminator
    ;

node:
      stmt  { $$ = $1; }
    | expr  { $$ = $1; }
    ;

stmt:
      vardef_stmt                  { $$ = $1; }
    | fndef_stmt                   { $$ = $1; }
    | typedef_stmt                 { $$ = $1; }
    | casedef_stmt                 { $$ = $1; }
    | assign_stmt                  { $$ = $1; }
    | if_stmt                      { $$ = $1; }
    | While expr block             { $$ = ast.make<WhileStmt>($1.start, $3->end, $2, $3); }
    | Do block While expr          { $$ = ast.make<DoWhileStmt>($1.start, $4->end, $2, $4); }
    | For pattern In expr block    { $$ = ast.make<ForStmt>($1.start, $5->end, $2, $4, $5); }
    | Break                        { $$ = ast.make<BreakStmt>($1.start, $1.end); }
    | Continue                     { $$ = ast.make<ContinueStmt>($1.start, $1.end); }
    | Return                       { $$ = ast.make<ReturnStmt>($1.start, $1.end, nullptr); }
    | Return expr                  { $$ = ast.make<ReturnStmt>($1.start, $2->end, $2); }
    | module_stmt                  { $$ = $1; }
    | Export stmt                  { $$ = ast.make<ExportStmt>($1.start, $2->end, $2); }
    | import_stmt                  { $$ = $1; }
    | From path Import Mul         { $$ = ast.make<ImportAllStmt>($1.start, $4.end, $2); }
    | Undefine path                { $$ = ast.make<UndefineStmt>($1.start, $2->end, $2); }
    ;

varmod:
      varkind
        {
           $$ = ast.make<VarModifier>($1.start, $1.end, $1.value, false, VarModifier::Optionality::None);
        }
    | varkind Caret
        {
            $$ = ast.make<VarModifier>($1.start, $2.end, $1.value, true, VarModifier::Optionality::None);
        }
    | varkind Question
        {
            $$ = ast.make<VarModifier>($1.start, $2.end, $1.value, false, VarModifier::Optionality::Optional);
        }
    | varkind NonNull
        {
            $$ = ast.make<VarModifier>($1.start, $2.end, $1.value, false, VarModifier::Optionality::OptionalNonNull);
        }
    ;

varkind:
      Let    { $$ = Symbol($1.start, $1.end, VarModifier::VarKind::Let); }
    | Const  { $$ = Symbol($1.start, $1.end, VarModifier::VarKind::Const); }
    | Ref    { $$ = Symbol($1.start, $1.end, VarModifier::VarKind::Ref); }
    | Cref   { $$ = Symbol($1.start, $1.end, VarModifier::VarKind::Cref); }
    ;

vardef_stmt:
      varmod pattern
        {
            $$ = ast.make<VarDefStmt>($1->start, $2->end, $1, $2, nullptr, nullptr);
        }
    | varmod pattern Colon expr
        {
            $$ = ast.make<VarDefStmt>($1->start, $4->end, $1, $2, $4, nullptr);
        }
    | varmod pattern Assign assign_rhs
        {
            $$ = ast.make<VarDefStmt>($1->start, $4->end, $1, $2, nullptr, $4);
        }
    | varmod pattern Colon expr Assign assign_rhs
        {
            $$ = ast.make<VarDefStmt>($1->start, $6->end, $1, $2, $4, $6);
        }
    ;

opt_varmod:
      /* empty */  { $$ = nullptr; }
    | varmod       { $$ = $1; }
    ;

opt_immut:
      /* empty */  { $$ = false; }
    | Caret        { $$ = true; }
    ;

opt_template:
      /* empty */              { $$ = {}; }
    | LBracket names RBracket  { $$ = std::move($2); }
    ;

fndef_stmt:
      Fn opt_immut name opt_template param_clause capture_clause ret_clause throw_clause block
        {
            auto [isThrowing, throwExpr] = $8;
            $$ = ast.make<FnDefStmt>($1.start, $9->end, $2, std::move($3.value), std::move($4),
                                     std::move($5), $6, std::move($7), isThrowing, throwExpr,
                                     $9);
        }
    | Fn opt_immut name opt_template param_clause capture_clause ret_clause throw_clause FatArrow expr
        {
            auto [isThrowing, throwExpr] = $8;
            $$ = ast.make<FnDefStmt>($1.start, $10->end, $2, std::move($3.value), std::move($4),
                                     std::move($5), $6,std::move($7), isThrowing, throwExpr,
                                     $10);
        }
    ;

param_clause:
      LParen RParen         { $$ = {}; }
    | LParen params RParen  { $$ = std::move($2); }
    ;

params:
      param               { $$ = {}; $$.push_back($1); }
    | params Comma param  { $$ = std::move($1); $$.push_back($3); }
    ;

param:
      pattern                     { $$ = ast.make<FnParam>($1->start, $1->end, nullptr, $1, nullptr, nullptr); }
    | pattern Colon expr          { $$ = ast.make<FnParam>($1->start, $3->end, nullptr, $1, $3, nullptr); }
    | varmod pattern              { $$ = ast.make<FnParam>($1->start, $2->end, $1, $2, nullptr, nullptr); }
    | varmod pattern Colon expr   { $$ = ast.make<FnParam>($1->start, $4->end, $1, $2, $4, nullptr); }
    | pattern Assign expr         { $$ = ast.make<FnParam>($1->start, $3->end, nullptr, $1, nullptr, $3); }
    | pattern Colon expr Assign expr
        {
            $$ = ast.make<FnParam>($1->start, $5->end, nullptr, $1, $3, $5);
        }
    | varmod pattern Assign expr  { $$ = ast.make<FnParam>($1->start, $4->end, $1, $2, nullptr, $4); }
    | varmod pattern Colon expr Assign expr
        {
            $$ = ast.make<FnParam>($1->start, $6->end, $1, $2, $4, $6);
        }
    ;

capture_clause:
      /* empty */                 { $$ = nullptr; }
    | LBracket RBracket
        {
            $$ = ast.make<FnCaptureClause>($1.start, $2.end, std::vector<FnCapture*>(), false, nullptr);
        }
    | LBracket captures RBracket  { $2->start = $1.start; $2->end = $3.end; $$ = $2; }
    ;

captures:
      capture
        {
            std::vector<FnCapture*> captures = { $1 };
            $$ = ast.make<FnCaptureClause>($1->start, $1->end, std::move(captures), false, nullptr);
        }
    | Range
        {
            $$ = ast.make<FnCaptureClause>($1.start, $1.end, std::vector<FnCapture*>(), true, nullptr);
        }
    | varmod Range
        {
            $$ = ast.make<FnCaptureClause>($1->start, $2.end, std::vector<FnCapture*>(), true, $1);
        }
    | captures Comma capture
        {
            $1->end = $3->end;
            $1->captures.push_back($3);
            $$ = std::move($1);
        }
    | captures Comma opt_varmod Range
        {
            $1->end = $4.end;
            $1->hasRest = true;
            $1->restMod = $3;
            $$ = std::move($1);
        }
    ;

capture:
      pattern         { $$ = ast.make<FnCapture>($1->start, $1->end, nullptr, $1); }
    | varmod pattern  { $$ = ast.make<FnCapture>($1->start, $2->end, $1, $2); }
    ;

ret_clause:
      /* empty */  { $$ = {}; }
    | Arrow rets   { $$ = std::move($2); }
    ;

rets:
      ret             { $$ = {}; $$.push_back($1); }
    | rets Comma ret  { $$ = std::move($1); $$.push_back($3); }
    ;

ret:
      expr       { $$ = ast.make<FnReturn>($1->start, $1->end, FnReturn::RetKind::ByValue, $1); }
    | Ref expr   { $$ = ast.make<FnReturn>($1.start, $2->end, FnReturn::RetKind::ByRef, $2); }
    | Cref expr  { $$ = ast.make<FnReturn>($1.start, $2->end, FnReturn::RetKind::ByCref, $2); }
    ;

throw_clause:
      /* empty */   { $$ = std::make_pair(false, nullptr); }
    | Throw         { $$ = std::make_pair(true, nullptr); }
    | Throw binary  { $$ = std::make_pair(true, $2); }
    ;

typedef_stmt:
      typedef_kind opt_immut name opt_template block
        {
            $$ = ast.make<TypeDefStmt>($1.start, $5->end, $1.value, $2, std::move($3.value),
                                       std::move($4), std::vector<Expr*>(), $5);
        }
    | typedef_kind opt_immut name opt_template Colon exprs block
        {
            $$ = ast.make<TypeDefStmt>($1.start, $7->end, $1.value, $2, std::move($3.value),
                                       std::move($4), std::move($6), $7);
        }
    ;

typedef_kind:
      Struct       { $$ = Symbol($1.start, $1.end, TypeDefStmt::TypeKind::Struct); }
    | Class        { $$ = Symbol($1.start, $1.end, TypeDefStmt::TypeKind::Class); }
    | Enum         { $$ = Symbol($1.start, $1.end, TypeDefStmt::TypeKind::Enum); }
    | Enum Struct  { $$ = Symbol($1.start, $2.end, TypeDefStmt::TypeKind::EnumStruct); }
    | Enum Class   { $$ = Symbol($1.start, $2.end, TypeDefStmt::TypeKind::EnumClass); }
    | Trait        { $$ = Symbol($1.start, $1.end, TypeDefStmt::TypeKind::Trait); }
    | Alias        { $$ = Symbol($1.start, $1.end, TypeDefStmt::TypeKind::Alias); }
    | Extension    { $$ = Symbol($1.start, $1.end, TypeDefStmt::TypeKind::Extension); }
    ;

casedef_stmt:
      Case name              { $$ = ast.make<CaseDefStmt>($1.start, $2.end, std::move($2.value), nullptr); }
    | Case name Assign expr  { $$ = ast.make<CaseDefStmt>($1.start, $4->end, std::move($2.value), $4); }
    ;

assign_stmt:
      regular_assign CoalesceAssign assign_rhs
        {
            $$ = ast.make<AssignStmt>($1->start, $3->end, AssignStmt::OpKind::CoalesceAssign, $1, $3);
        }
    | regular_assign
    ;

assign_rhs:
      expr         { $$ = $1; }
    | assign_stmt  { $$ = $1; }
    ;

regular_assign:
      expr Assign regular_rhs
        {
            $$ = ast.make<AssignStmt>($1->start, $3->end, AssignStmt::OpKind::Assign, $1, $3);
        }
    | expr AddAssign regular_rhs
        {
            $$ = ast.make<AssignStmt>($1->start, $3->end, AssignStmt::OpKind::AddAssign, $1, $3);
        }
    | expr SubAssign regular_rhs
        {
            $$ = ast.make<AssignStmt>($1->start, $3->end, AssignStmt::OpKind::SubAssign, $1, $3);
        }
    | expr MulAssign regular_rhs
        {
            $$ = ast.make<AssignStmt>($1->start, $3->end, AssignStmt::OpKind::MulAssign, $1, $3);
        }
    | expr DivAssign regular_rhs
        {
            $$ = ast.make<AssignStmt>($1->start, $3->end, AssignStmt::OpKind::DivAssign, $1, $3);
        }
    | expr ModAssign regular_rhs
        {
            $$ = ast.make<AssignStmt>($1->start, $3->end, AssignStmt::OpKind::ModAssign, $1, $3);
        }
    | expr BitAndAssign regular_rhs
        {
            $$ = ast.make<AssignStmt>($1->start, $3->end, AssignStmt::OpKind::BitAndAssign, $1, $3);
        }
    | expr BitOrAssign regular_rhs
        {
            $$ = ast.make<AssignStmt>($1->start, $3->end, AssignStmt::OpKind::BitOrAssign, $1, $3);
        }
    | expr ShlAssign regular_rhs
        {
            $$ = ast.make<AssignStmt>($1->start, $3->end, AssignStmt::OpKind::BitShlAssign, $1, $3);
        }
    | expr ShrAssign regular_rhs
        {
            $$ = ast.make<AssignStmt>($1->start, $3->end, AssignStmt::OpKind::BitShrAssign, $1, $3);
        }
    ;

regular_rhs:
      expr            { $$ = $1; }
    | regular_assign  { $$ = $1; }
    ;

if_stmt:
      If expr block              { $$ = ast.make<IfStmt>($1.start, $3->end, $2, $3, nullptr); }
    | If expr block else_clause  { $$ = ast.make<IfStmt>($1.start, $4->end, $2, $3, $4); }
    ;

else_clause:
      Else block    { $$ = std::move($2); }
    | Else if_stmt
        {
            std::vector<Node*> nodes;
            nodes.push_back($2);
            $$ = ast.make<BlockExpr>($2->start, $2->end, std::move(nodes));
        }
    ;

module_stmt:
      Module path block  { $$ = ast.make<ModuleStmt>($1.start, $3->end, $2, $3); }
    ;

import_stmt:
      From path Import import
        {
            std::vector<ImportItem*> imports = { $4 };
            $$ = ast.make<ImportStmt>($1.start, $4->end, $2, std::move(imports));
        }
    | import_stmt Comma import
        {
            $$->imports.push_back($3);
            $$->end = $3->end;
        }
    ;

import:
      path          { $$ = ast.make<ImportItem>($1->start, $1->end, $1, std::nullopt); }
    | path As name  { $$ = ast.make<ImportItem>($1->start, $3.end, $1, std::move($3.value)); }
    ;



expr:
      lambda                        { $$ = $1; }
    | If expr Then expr Else expr   { $$ = ast.make<IfThenExpr>($1.start, $6->end, $2, $4, $6); }
    | Try expr Else expr            { $$ = ast.make<TryElseExpr>($1.start, $4->end, $2, $4); }
    | match                         { $$ = $1; }
    | trycatch                      { $$ = $1; }
    | Throw expr                    { $$ = ast.make<ThrowExpr>($1.start, $2->end, $2); }
    | block                         { $$ = $1; }
    | binary Is expr                { $$ = ast.make<IsExpr>($1->start, $3->end, $1, $3); }
    | binary As pattern Colon expr  { $$ = ast.make<AsExpr>($1->start, $5->end, $1, $3, $5); }
    | binary
    ;

exprs:
      expr              { $$ = {}; $$.push_back($1); }
    | exprs Comma expr  { $$ = std::move($1); $$.push_back($3); }
    ;

lambda:
      Fn opt_immut param_clause capture_clause ret_clause throw_clause FatArrow expr
        {
            auto [isThrowing, throwExpr] = $6;
            $$ = ast.make<LambdaExpr>($1.start, $8->end, $2, std::move($3), $4, std::move($5), isThrowing, throwExpr,
                                      $8);
        }
    ;

match:
      Match expr LBrace cases RBrace  { $$ = ast.make<MatchExpr>($1.start, $5.end, $2, std::move($4)); }
    ;

cases:
      case        { $$ = {}; $$.push_back($1); }
    | cases case  { $$ = std::move($1); $$.push_back($2); }
    ;

case:
      Case pattern FatArrow expr          { $$ = ast.make<MatchCase>($1.start, $4->end, $2, nullptr, $4); }
    | Case If expr FatArrow expr          { $$ = ast.make<MatchCase>($1.start, $5->end, nullptr, $3, $5); }
    | Case pattern If expr FatArrow expr  { $$ = ast.make<MatchCase>($1.start, $6->end, $2, $4, $6); }
    ;

trycatch:
      Try expr LBrace catches RBrace  { $$ = ast.make<TryCatchExpr>($1.start, $5.end, $2, std::move($4)); }
    ;

catches:
      catch          { $$ = {}; $$.push_back($1); }
    | catches catch  { $$ = std::move($1); $$.push_back($2); }
    ;

catch:
      Catch pattern FatArrow expr          { $$ = ast.make<CatchClause>($1.start, $4->end, $2, nullptr, $4); }
    | Catch If expr FatArrow expr          { $$ = ast.make<CatchClause>($1.start, $5->end, nullptr, $3, $5); }
    | Catch pattern If expr FatArrow expr  { $$ = ast.make<CatchClause>($1.start, $6->end, $2, $4, $6); }
    ;

block:
      LBrace nodes RBrace  { $$ = ast.make<BlockExpr>($1.start, $3.end, std::move($2)); }
    ;

binary:
      binary_pipe
    ;

binary_pipe:
      binary_type Pipe binary_pipe
        {
            $$ = ast.make<BinaryExpr>($1->start, $3->end, BinaryExpr::OpKind::Pipe, $1, $3);
        }
    | binary_type
    ;

binary_type:
      binary_range Arrow binary_type
        {
            $$ = ast.make<BinaryExpr>($1->start, $3->end, BinaryExpr::OpKind::FuncType, $1, $3);
        }
    | binary_range
    ;

binary_range:
      binary_coalesce Range binary_coalesce
        {
            $$ = ast.make<BinaryExpr>($1->start, $3->end, BinaryExpr::OpKind::Range, $1, $3);
        }
    | binary_coalesce RangeExcl binary_coalesce
        {
            $$ = ast.make<BinaryExpr>($1->start, $3->end, BinaryExpr::OpKind::RangeExcl, $1, $3);
        }
    | binary_coalesce
    ;

binary_coalesce:
      binary_logor Coalesce binary_coalesce
        {
            $$ = ast.make<BinaryExpr>($1->start, $3->end, BinaryExpr::OpKind::Coalesce, $1, $3);
        }
    | binary_logor
    ;

binary_logor:
      binary_logor LogOr binary_logand
        {
            $$ = ast.make<BinaryExpr>($1->start, $3->end, BinaryExpr::OpKind::LogOr, $1, $3);
        }
    | binary_logand
    ;

binary_logand:
      binary_logand LogAnd binary_bitor
        {
            $$ = ast.make<BinaryExpr>($1->start, $3->end, BinaryExpr::OpKind::LogAnd, $1, $3);
        }
    | binary_bitor
    ;

binary_bitor:
      binary_bitor VBar binary_bitxor
        {
            $$ = ast.make<BinaryExpr>($1->start, $3->end, BinaryExpr::OpKind::BitOr, $1, $3);
        }
    | binary_bitxor
    ;

binary_bitxor:
      binary_bitxor Caret binary_bitand
        {
            $$ = ast.make<BinaryExpr>($1->start, $3->end, BinaryExpr::OpKind::BitXor, $1, $3);
        }
    | binary_bitand
    ;

binary_bitand:
      binary_bitand And binary_eq
        {
            $$ = ast.make<BinaryExpr>($1->start, $3->end, BinaryExpr::OpKind::BitAnd, $1, $3);
        }
    | binary_eq
    ;

binary_eq:
      binary_compare Eq binary_compare
        {
            $$ = ast.make<BinaryExpr>($1->start, $3->end, BinaryExpr::OpKind::Eq, $1, $3);
        }
    | binary_compare Ne binary_compare
        {
            $$ = ast.make<BinaryExpr>($1->start, $3->end, BinaryExpr::OpKind::Ne, $1, $3);
        }
    | binary_compare StrictEq binary_compare
        {
            $$ = ast.make<BinaryExpr>($1->start, $3->end, BinaryExpr::OpKind::StrictEq, $1, $3);
        }
    | binary_compare StrictNe binary_compare
        {
            $$ = ast.make<BinaryExpr>($1->start, $3->end, BinaryExpr::OpKind::StrictNe, $1, $3);
        }
    | binary_compare
    ;

binary_compare:
      binary_bitshift Lt binary_bitshift
        {
            $$ = ast.make<BinaryExpr>($1->start, $3->end, BinaryExpr::OpKind::Lt, $1, $3);
        }
    | binary_bitshift Le binary_bitshift
        {
            $$ = ast.make<BinaryExpr>($1->start, $3->end, BinaryExpr::OpKind::Le, $1, $3);
        }
    | binary_bitshift Gt binary_bitshift
        {
            $$ = ast.make<BinaryExpr>($1->start, $3->end, BinaryExpr::OpKind::Gt, $1, $3);
        }
    | binary_bitshift Ge binary_bitshift
        {
            $$ = ast.make<BinaryExpr>($1->start, $3->end, BinaryExpr::OpKind::Ge, $1, $3);
        }
    | binary_bitshift
    ;

binary_bitshift:
      binary_bitshift Shl binary_additive
        {
            $$ = ast.make<BinaryExpr>($1->start, $3->end, BinaryExpr::OpKind::BitShl, $1, $3);
        }
    | binary_bitshift Shr binary_additive
        {
            $$ = ast.make<BinaryExpr>($1->start, $3->end, BinaryExpr::OpKind::BitShr, $1, $3);
        }
    | binary_additive
    ;

binary_additive:
      binary_additive Add binary_multiplicative
        {
            $$ = ast.make<BinaryExpr>($1->start, $3->end, BinaryExpr::OpKind::Add, $1, $3);
        }
    | binary_additive Sub binary_multiplicative
        {
            $$ = ast.make<BinaryExpr>($1->start, $3->end, BinaryExpr::OpKind::Sub, $1, $3);
        }
    | binary_multiplicative
    ;

binary_multiplicative:
      binary_multiplicative Mul prefix
        {
            $$ = ast.make<BinaryExpr>($1->start, $3->end, BinaryExpr::OpKind::Mul, $1, $3);
        }
    | binary_multiplicative Div prefix
        {
            $$ = ast.make<BinaryExpr>($1->start, $3->end, BinaryExpr::OpKind::Div, $1, $3);
        }
    | binary_multiplicative Mod prefix
        {
            $$ = ast.make<BinaryExpr>($1->start, $3->end, BinaryExpr::OpKind::Mod, $1, $3);
        }
    | prefix
    ;

prefix:
      Add prefix    { $$ = ast.make<PrefixExpr>($1.start, $2->end, PrefixExpr::OpKind::Pos, $2); }
    | Sub prefix    { $$ = ast.make<PrefixExpr>($1.start, $2->end, PrefixExpr::OpKind::Neg, $2); }
    | Tide prefix   { $$ = ast.make<PrefixExpr>($1.start, $2->end, PrefixExpr::OpKind::BitNot, $2); }
    | Bang prefix   { $$ = ast.make<PrefixExpr>($1.start, $2->end, PrefixExpr::OpKind::LogNot, $2); }
    | Caret prefix  { $$ = ast.make<PrefixExpr>($1.start, $2->end, PrefixExpr::OpKind::Immut, $2); }
    | postfix
    ;

postfix:
      postfix Question  { $$ = ast.make<PostfixExpr>($1->start, $2.end, PostfixExpr::OpKind::Optional, $1); }
    | postfix NonNull   { $$ = ast.make<PostfixExpr>($1->start, $2.end, PostfixExpr::OpKind::NonNull, $1); }
    | postfix Bang      { $$ = ast.make<PostfixExpr>($1->start, $2.end, PostfixExpr::OpKind::ForceUnwrap, $1); }
    | postfix Dot name  { $$ = ast.make<MemberAccessExpr>($1->start, $3.end, $1, std::move($3.value)); }
    | postfix LParen call_args RParen
        {
            // TODO: Make sure named args are after all positional args
            $$ = ast.make<CallExpr>($1->start, $4.end, $1, std::move($3));
        }
    | postfix LBracket exprs RBracket
        {
            $$ = ast.make<SubscriptExpr>($1->start, $4.end, $1, std::move($3));
        }
    | primary
    ;

call_args:
      /* empty */               { $$ = {}; }
    | call_arg                  { $$ = {}; $$.push_back($1); }
    | call_args Comma call_arg  { $$ = $1; $$.push_back($3); }
    ;

call_arg:
      expr             { $$ = ast.make<CallArg>($1->start, $1->end, std::nullopt, $1); }
    | name Colon expr  { $$ = ast.make<CallArg>($1.start, $3->end, std::move($1.value), $3); }
    ;

primary:
      literal             { $$ = ast.make<LiteralExpr>($1.start, $1.end, std::move($1.value)); }
    | name                { $$ = ast.make<NameExpr>($1.start, $1.end, std::move($1.value)); }
    | Global Dot name     { $$ = ast.make<GlobalAccessExpr>($1.start, $3.end, std::move($3.value)); }
    | upvalue_expr        { $$ = $1; }
    | LParen expr RParen  { $$ = $2; }
    | tuple_expr          { $$ = $1; }
    | collection_expr     { $$ = $1; }
    | Typeof LParen expr RParen
        {
            $$ = ast.make<TypeofExpr>($1.start, $4.end, $3);
        }
    ;

upvalue_expr:
      dollars name
        {
            if ($1.end.line != $2.start.line || $1.end.column + 1 != $2.start.column) {
                RAISE_ERROR($1.start, $2.end, "unexpected whitespace after `$`");
                YYERROR;
            }
            $$ = ast.make<UpvalueExpr>($1.start, $2.end, $1.value, std::move($2.value));
        }
    ;

dollars:
      Dollar          { $$ = Symbol<uint64_t>($1.start, $1.end, 1); }
    | dollars Dollar
        {
            if ($1.end.line != $2.start.line || $1.end.column + 1 != $2.start.column) {
                RAISE_ERROR($1.start, $2.end, "unexpected whitespace after `$`");
                YYERROR;
            }
            ++($$.value);
            $$.end = $2.end;
        }
    ;

tuple_expr:
      LParen expr Comma exprs RParen
        {
            $4.insert($4.begin(), $2);
            $$ = ast.make<TupleExpr>($1.start, $5.end, std::move($4));
        }
    ;

collection_expr:
      LBracket exprs RBracket
        {
            $$ = ast.make<CollectionExpr>($1.start, $3.end, std::move($2));
        }
    ;



pattern:
      literal             { $$ = ast.make<LiteralPattern>($1.start, $1.end, std::move($1.value)); }
    | name                { $$ = ast.make<BindingPattern>($1.start, $1.end, std::move($1.value)); }
    | LParen patterns RParen
        {
            $$ = ast.make<TuplePattern>($1.start, $3.end, std::move($2));
        }
    | collection_pattern  { $$ = $1; }
    ;

patterns:
      pattern                 { $$ = {}; $$.push_back($1); }
    | patterns Comma pattern  { $$ = std::move($1); $$.push_back($3); }
    ;

collection_pattern:
      LBracket RBracket
        {
            $$ = ast.make<CollectionPattern>($1.start, $2.end, std::vector<Pattern*>(), std::vector<Pattern*>(), true);
        }
    | LBracket Range RBracket
        {
            $$ = ast.make<CollectionPattern>($1.start, $3.end, std::vector<Pattern*>(), std::vector<Pattern*>(), false);
        }
    | LBracket patterns RBracket
        {
            $$ = ast.make<CollectionPattern>($1.start, $3.end, std::move($2), std::vector<Pattern*>(), true);
        }
    | LBracket Range Comma patterns RBracket
        {
            $$ = ast.make<CollectionPattern>($1.start, $5.end, std::vector<Pattern*>(), std::move($4), false);
        }
    | LBracket patterns Comma Range RBracket
        {
            $$ = ast.make<CollectionPattern>($1.start, $5.end, std::move($2), std::vector<Pattern*>(), false);
        }
    | LBracket patterns Comma Range Comma patterns RBracket
        {
            $$ = ast.make<CollectionPattern>($1.start, $7.end, std::move($2), std::move($6), false);
        }
    ;



literal:
      Integer        { $$ = Symbol<Literal>($1.start, $1.end, IntLiteral(BigInt($1.lexeme))); }
    | Real           { $$ = Symbol<Literal>($1.start, $1.end, RealLiteral(BigReal($1.lexeme))); }
    | True           { $$ = Symbol<Literal>($1.start, $1.end, BoolLiteral(true)); }
    | False          { $$ = Symbol<Literal>($1.start, $1.end, BoolLiteral(false)); }
    | String         { $$ = Symbol<Literal>($1.start, $1.end, StringLiteral(std::move($1.lexeme))); }
    | Nil            { $$ = Symbol<Literal>($1.start, $1.end, NilLiteral()); }
    | LParen RParen  { $$ = Symbol<Literal>($1.start, $2.end, VoidLiteral()); }
    ;

name:
      Identifier       { $$ = Symbol<Name>($1.start, $1.end, IdentifierName(std::move($1.lexeme))); }
    | Discard          { $$ = Symbol<Name>($1.start, $1.end, DiscardName()); }
    | Constructor      { $$ = Symbol<Name>($1.start, $1.end, ConstructorName()); }
    | Destructor       { $$ = Symbol<Name>($1.start, $1.end, DestructorName()); }
    | Operator Add
        {
            $$ = Symbol<Name>($1.start, $2.end, OperatorName(OperatorName::OpKind::Add));
        }
    | Operator Sub
        {
            $$ = Symbol<Name>($1.start, $2.end, OperatorName(OperatorName::OpKind::Sub));
        }
    | Operator Mul
        {
            $$ = Symbol<Name>($1.start, $2.end, OperatorName(OperatorName::OpKind::Mul));
        }
    | Operator Div
        {
            $$ = Symbol<Name>($1.start, $2.end, OperatorName(OperatorName::OpKind::Div));
        }
    | Operator Mod
        {
            $$ = Symbol<Name>($1.start, $2.end, OperatorName(OperatorName::OpKind::Mod));
        }
    | Operator Tide
        {
            $$ = Symbol<Name>($1.start, $2.end, OperatorName(OperatorName::OpKind::BitNot));
        }
    | Operator And
        {
            $$ = Symbol<Name>($1.start, $2.end, OperatorName(OperatorName::OpKind::BitAnd));
        }
    | Operator VBar
        {
            $$ = Symbol<Name>($1.start, $2.end, OperatorName(OperatorName::OpKind::BitOr));
        }
    | Operator Caret
        {
            $$ = Symbol<Name>($1.start, $2.end, OperatorName(OperatorName::OpKind::BitXor));
        }
    | Operator Shl
        {
            $$ = Symbol<Name>($1.start, $2.end, OperatorName(OperatorName::OpKind::BitShl));
        }
    | Operator Shr
        {
            $$ = Symbol<Name>($1.start, $2.end, OperatorName(OperatorName::OpKind::BitShr));
        }
    | Operator Bang
        {
            $$ = Symbol<Name>($1.start, $2.end, OperatorName(OperatorName::OpKind::LogNot));
        }
    | Operator LogAnd
        {
            $$ = Symbol<Name>($1.start, $2.end, OperatorName(OperatorName::OpKind::LogAnd));
        }
    | Operator LogOr
        {
            $$ = Symbol<Name>($1.start, $2.end, OperatorName(OperatorName::OpKind::LogOr));
        }
    | Operator Eq
        {
            $$ = Symbol<Name>($1.start, $2.end, OperatorName(OperatorName::OpKind::Eq));
        }
    | Operator Ne
        {
            $$ = Symbol<Name>($1.start, $2.end, OperatorName(OperatorName::OpKind::Ne));
        }
    | Operator Lt
        {
            $$ = Symbol<Name>($1.start, $2.end, OperatorName(OperatorName::OpKind::Lt));
        }
    | Operator Le
        {
            $$ = Symbol<Name>($1.start, $2.end, OperatorName(OperatorName::OpKind::Le));
        }
    | Operator Gt
        {
            $$ = Symbol<Name>($1.start, $2.end, OperatorName(OperatorName::OpKind::Gt));
        }
    | Operator Ge
        {
            $$ = Symbol<Name>($1.start, $2.end, OperatorName(OperatorName::OpKind::Ge));
        }
    | Operator Range
        {
            $$ = Symbol<Name>($1.start, $2.end, OperatorName(OperatorName::OpKind::Range));
        }
    | Operator RangeExcl
        {
            $$ = Symbol<Name>($1.start, $2.end, OperatorName(OperatorName::OpKind::RangeExcl));
        }
    | Operator LParen Add RParen
        {
            $$ = Symbol<Name>($1.start, $4.end, OperatorName(OperatorName::OpKind::Pos));
        }
    | Operator LParen Sub RParen
        {
            $$ = Symbol<Name>($1.start, $4.end, OperatorName(OperatorName::OpKind::Neg));
        }
    | Operator LParen RParen
        {
            $$ = Symbol<Name>($1.start, $3.end, OperatorName(OperatorName::OpKind::Call));
        }
    | Operator LBracket RBracket
        {
            $$ = Symbol<Name>($1.start, $3.end, OperatorName(OperatorName::OpKind::Subscript));
        }
    | Operator AddAssign
        {
            $$ = Symbol<Name>($1.start, $2.end, OperatorName(OperatorName::OpKind::AddAssign));
        }
    | Operator SubAssign
        {
            $$ = Symbol<Name>($1.start, $2.end, OperatorName(OperatorName::OpKind::SubAssign));
        }
    | Operator MulAssign
        {
            $$ = Symbol<Name>($1.start, $2.end, OperatorName(OperatorName::OpKind::MulAssign));
        }
    | Operator DivAssign
        {
            $$ = Symbol<Name>($1.start, $2.end, OperatorName(OperatorName::OpKind::DivAssign));
        }
    | Operator ModAssign
        {
            $$ = Symbol<Name>($1.start, $2.end, OperatorName(OperatorName::OpKind::ModAssign));
        }
    | Operator BitAndAssign
        {
            $$ = Symbol<Name>($1.start, $2.end, OperatorName(OperatorName::OpKind::BitAndAssign));
        }
    | Operator BitOrAssign
        {
            $$ = Symbol<Name>($1.start, $2.end, OperatorName(OperatorName::OpKind::BitOrAssign));
        }
    | Operator BitXorAssign
        {
            $$ = Symbol<Name>($1.start, $2.end, OperatorName(OperatorName::OpKind::BitXorAssign));
        }
    | Operator ShlAssign
        {
            $$ = Symbol<Name>($1.start, $2.end, OperatorName(OperatorName::OpKind::BitShlAssign));
        }
    | Operator ShrAssign
        {
            $$ = Symbol<Name>($1.start, $2.end, OperatorName(OperatorName::OpKind::BitShrAssign));
        }
    | Self             { $$ = Symbol<Name>($1.start, $1.end, SelfName()); }
    ;

names:
      name              { $$ = {}; $$.push_back(std::move($1.value)); }
    | names Comma name  { $$ = std::move($1); $$.push_back(std::move($3.value)); }
    ;

path:
      path_seg
        {
            std::vector<PathSeg*> segs = { $1 };
            $$ = ast.make<Path>($1->start, $1->end, std::move(segs));
        }
    | path Dot path_seg
        {
            $$ = std::move($1);
            $$->segs.push_back($3);
            $$->end = $3->end;
        }
    ;

path_seg:
      name
        {
            $$ = ast.make<PathSeg>($1.start, $1.end, std::move($1.value), std::vector<Expr*>());
        }
    | name LBracket exprs RBracket
        {
            $$ = ast.make<PathSeg>($1.start, $4.end, std::move($1.value), std::move($3));
        }
    ;
%%

void yy::parser::error(const yy::location& loc, const std::string& msg) {
    Location start(static_cast<size_t>(loc.begin.line), static_cast<size_t>(loc.begin.column));
    Location end(static_cast<size_t>(loc.end.line), static_cast<size_t>(loc.end.column));
    RAISE_ERROR(start, end, msg);
}
