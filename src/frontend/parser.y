%require "3.5"

%skeleton "lalr1.cc"

%language "c++"

%define api.token.raw
%define api.value.type variant
%define parse.error verbose

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
%token <Spark::FrontEnd::TokenValue> Semicolon Newline LParen RParen LBracket RBracket LBrace RBrace
%token <Spark::FrontEnd::TokenValue> At
%token <Spark::FrontEnd::TokenValue> Dollar
%token <Spark::FrontEnd::TokenValue> LineComment BlockComment

%token EndOfFile 0
%token Error 1

%type <std::vector<Spark::FrontEnd::Stmt*>> stmts stmt_list
%type <Spark::FrontEnd::Stmt*> stmt
%type <Spark::FrontEnd::VarModifier*> varmod
%type <Symbol<Spark::FrontEnd::VarModifier::VarKind>> varkind
%type <Spark::FrontEnd::VarDefStmt*> vardef_stmt
%type <Spark::FrontEnd::VarModifier*> opt_varmod
%type <bool> opt_immut
%type <std::vector<Spark::FrontEnd::Expr*>> opt_template
%type <Spark::FrontEnd::FnDefStmt*> fndef_stmt
%type <Spark::FrontEnd::Name*> fn_name
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
%type <Symbol<Spark::FrontEnd::TypeDefStmt::TypeKind>> typedef_kind
%type <Spark::FrontEnd::Name*> typedef_name
%type <Spark::FrontEnd::AssignStmt*> assign_stmt
%type <Spark::FrontEnd::Node*> assign_rhs
%type <Spark::FrontEnd::AssignStmt*> regular_assign
%type <Spark::FrontEnd::Node*> regular_rhs
%type <Spark::FrontEnd::IfStmt*> if_stmt
%type <Spark::FrontEnd::BlockExpr*> else_clause
%type <Spark::FrontEnd::ModuleStmt*> module_stmt
%type <std::vector<Spark::FrontEnd::Name*>> module_path
%type <Spark::FrontEnd::Name*> module_name

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
%type <Spark::FrontEnd::IdentifierName*> arg_label
%type <Spark::FrontEnd::Expr*> primary
%type <Spark::FrontEnd::Expr*> literal_expr
%type <Spark::FrontEnd::UpvalueExpr*> upvalue_expr
%type <Symbol<uint64_t>> dollars
%type <Spark::FrontEnd::TupleExpr*> tuple_expr
%type <Spark::FrontEnd::CollectionExpr*> collection_expr

%type <Spark::FrontEnd::Pattern*> pattern
%type <std::vector<Spark::FrontEnd::Pattern*>> patterns
%type <Spark::FrontEnd::CollectionPattern*> collection_pattern

%type <Spark::FrontEnd::Name*> name
%type <Symbol<Spark::FrontEnd::OverloadableOpKind>> overloadable_op

%%
%start program;

program:
      stmts  { ast.root = ast.make<BlockExpr>($1.front()->start, $1.back()->end, std::move($1)); }
    ;

stmts:
      /* empty */            { $$ = {}; }
    | stmt_list
    | stmt_list terminators
    ;

stmt_list:
      stmt                        { $$ = {}; $$.push_back($1); }
    | stmt_list terminators stmt  { $$ = std::move($1); $$.push_back($3); }
    ;

terminator:
      Newline
    | Semicolon
    ;

terminators:
      terminator
    | terminators terminator
    ;

stmt:
      vardef_stmt                  { $$ = $1; }
    | fndef_stmt                   { $$ = $1; }
    | typedef_stmt
        {
            if ($1->kind == TypeDefStmt::TypeKind::Alias) {
                if ($1->bases.size() != 1) {
                    RAISE_ERROR($1->start, $1->bases.back()->end, "alias requires exactly one base type");
                }
            } else if ($1->kind == TypeDefStmt::TypeKind::Extension) {
                if ($1->bases.size() != 1) {
                    RAISE_ERROR($1->start, $1->bases.back()->end, "extension requires exactly one base type");
                }
            }
            $$ = $1;
        }
    //| enumcase_stmt
    | assign_stmt                  { $$ = $1; }
    | if_stmt                      { $$ = $1; }
    | While expr block             { $$ = ast.make<WhileStmt>($1.start, $3->end, $2, $3); }
    | For pattern In expr block    { $$ = ast.make<ForStmt>($1.start, $5->end, $2, $4, $5); }
    | Break                        { $$ = ast.make<BreakStmt>($1.start, $1.end); }
    | Continue                     { $$ = ast.make<ContinueStmt>($1.start, $1.end); }
    | Return                       { $$ = ast.make<ReturnStmt>($1.start, $1.end, nullptr); }
    | Return expr                  { $$ = ast.make<ReturnStmt>($1.start, $2->end, $2); }
    | module_stmt                  { $$ = $1; }
    //| Export stmt                { $$ = ast.make<Export>($1.start, $2.end, $2); }
    //| import_stmt
    //| Undefine postfix           { $$ = ast.make<Undefine>($1.start, $2.end, $2); }
    | expr                         { $$ = ast.make<ExprStmt>($1->start, $1->end, $1); }
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
    | LBracket exprs RBracket  { $$ = std::move($2); }
    ;

fndef_stmt:
      Fn opt_immut fn_name opt_template param_clause capture_clause ret_clause throw_clause block
        {
            auto [isThrowing, throwExpr] = $8;
            $$ = ast.make<FnDefStmt>($1.start, $9->end, $2, $3, std::move($4), std::move($5), $6, std::move($7),
                                     isThrowing, throwExpr, $9);
        }
    | Fn opt_immut fn_name opt_template param_clause capture_clause ret_clause throw_clause FatArrow expr
        {
            auto [isThrowing, throwExpr] = $8;
            $$ = ast.make<FnDefStmt>($1.start, $10->end, $2, $3, std::move($4), std::move($5), $6, std::move($7),
                                     isThrowing, throwExpr, $10);
        }
    ;

fn_name:
      Identifier       { $$ = ast.make<IdentifierName>($1.start, $1.end, std::move($1.lexeme)); }
    | Discard          { $$ = ast.make<DiscardName>($1.start, $1.end); }
    | Constructor      { $$ = ast.make<ConstructorName>($1.start, $1.end); }
    | Destructor       { $$ = ast.make<DestructorName>($1.start, $1.end); }
    | overloadable_op  { $$ = ast.make<OverloadableOpName>($1.start, $1.end, $1.value); }
    | Self             { RAISE_ERROR($1.start, $1.end, "`self` cannot be used as a function definition name"); YYERROR; }
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
    | pattern Colon expr          { $$ = ast.make<FnParam>($1->start, $3->end, nullptr, $1, nullptr, nullptr); }
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
      typedef_kind opt_immut typedef_name opt_template block
        {
            $$ = ast.make<TypeDefStmt>($1.start, $5->end, $1.value, $2, $3, std::move($4), std::vector<Expr*>(), $5);
        }
    | typedef_kind opt_immut typedef_name opt_template Colon exprs block
        {
            $$ = ast.make<TypeDefStmt>($1.start, $7->end, $1.value, $2, $3, std::move($4), std::move($6), $7);
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

typedef_name:
      Identifier       { $$ = ast.make<IdentifierName>($1.start, $1.end, std::move($1.lexeme)); }
    | Discard          { $$ = ast.make<DiscardName>($1.start, $1.end); }
    | Constructor      { RAISE_ERROR($1.start, $1.end, "`constructor` cannot be used as a type definition name"); YYERROR; }
    | Destructor       { RAISE_ERROR($1.start, $1.end, "`destructor` cannot be used as a type definition name"); YYERROR; }
    | overloadable_op  { RAISE_ERROR($1.start, $1.end, "`operator` cannot be used as atype definition name"); YYERROR; }
    | Self             { RAISE_ERROR($1.start, $1.end, "`self` cannot be used as a type definition name"); YYERROR; }
    ;

enumcase_stmt:
      Case case_name
    | Case case_name Assign expr
    | Case adt_constructor
    ;

case_name:
      Identifier
    | Discard
    ;

adt_constructor:
      Identifier LParen RParen
    | Identifier LParen exprs RParen
    | Identifier LParen named_adt_members RParen
    ;

named_adt_members:
      named_adt_member
    | named_adt_members Comma named_adt_member
    ;

named_adt_member:
      Identifier Colon expr
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
            std::vector<Stmt*> stmts;
            stmts.push_back($2);
            $$ = ast.make<BlockExpr>($2->start, $2->end, std::move(stmts));
        }
    ;

module_stmt:
      Module module_path block  { $$ = ast.make<ModuleStmt>($1.start, $3->end, std::move($2), $3); }
    ;

module_path:
      module_name                  { $$ = {}; $$.push_back($1); }
    | module_path Dot module_name  { $$ = std::move($1); $$.push_back($3); }
    ;

module_name:
      Identifier       { $$ = ast.make<IdentifierName>($1.start, $1.end, std::move($1.lexeme)); }
    | Discard          { $$ = ast.make<DiscardName>($1.start, $1.end); }
    | Constructor      { RAISE_ERROR($1.start, $1.end, "`constructor` cannot be used as a module definition name"); YYERROR; }
    | Destructor       { RAISE_ERROR($1.start, $1.end, "`destructor` cannot be used as a module definition name"); YYERROR; }
    | overloadable_op  { RAISE_ERROR($1.start, $1.end, "`operator` cannot be used as a module definition name"); YYERROR; }
    | Self             { RAISE_ERROR($1.start, $1.end, "`self` cannot be used as a module definition name"); YYERROR; }
    ;

import_stmt:
      From postfix Import imports
    | From postfix Import Mul
    ;

imports:
      import
    | imports Comma import
    ;

import:
      postfix
    | postfix As Identifier
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
      LBrace stmts RBrace  { $$ = ast.make<BlockExpr>($1.start, $3.end, std::move($2)); }
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
    | postfix Dot name
        {
            if (dynamic_cast<DiscardName*>($3) != nullptr) {
                RAISE_ERROR($3->start, $3->end, "`_` cannot be used as an expression");
                YYERROR;
            }
            $$ = ast.make<MemberAccessExpr>($1->start, $3->end, $1, $3);
        }
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
      expr                  { $$ = ast.make<CallArg>($1->start, $1->end, nullptr, $1); }
    | arg_label Colon expr  { $$ = ast.make<CallArg>($1->start, $3->end, $1, $3); }
    ;

arg_label:
      Identifier       { $$ = ast.make<IdentifierName>($1.start, $1.end, std::move($1.lexeme)); }
    | Discard          { RAISE_ERROR($1.start, $1.end, "`_` cannot be used for module definition"); YYERROR; }
    | Constructor      { RAISE_ERROR($1.start, $1.end, "`constructor` cannot be used for module definition"); YYERROR; }
    | Destructor       { RAISE_ERROR($1.start, $1.end, "`destructor` cannot be used for module definition"); YYERROR; }
    | overloadable_op  { RAISE_ERROR($1.start, $1.end, "`operator` cannot be used for module definition"); YYERROR; }
    | Self             { RAISE_ERROR($1.start, $1.end, "`self` cannot be used for module definition"); YYERROR; }
    ;

primary:
      literal_expr
    | Identifier                 { $$ = ast.make<IdentifierExpr>($1.start, $1.end, std::move($1.lexeme)); }
    | Discard                    { RAISE_ERROR($1.start, $1.end, "`_` cannot be used as an expression"); YYERROR; }
    | Constructor                { $$ = ast.make<ConstructorExpr>($1.start, $1.end); }
    | Destructor                 { $$ = ast.make<DestructorExpr>($1.start, $1.end); }
    | overloadable_op            { $$ = ast.make<OverloadableOpExpr>($1.start, $1.end, $1.value); }
    | Self                       { $$ = ast.make<SelfExpr>($1.start, $1.end); }
    | Global Dot name            { $$ = ast.make<GlobalAccessExpr>($1.start, $3->end, $3); }
    | upvalue_expr               { $$ = $1; }
    | LParen expr RParen         { $$ = $2; }
    | tuple_expr                 { $$ = $1; }
    | collection_expr            { $$ = $1; }
    | Typeof LParen expr RParen  { $$ = ast.make<TypeofExpr>($1.start, $4.end, $3); }
    ;

literal_expr:
      Integer        { $$ = ast.make<IntLiteralExpr>($1.start, $1.end, BigInt($1.lexeme)); }
    | Real           { $$ = ast.make<RealLiteralExpr>($1.start, $1.end, BigReal($1.lexeme)); }
    | True           { $$ = ast.make<BoolLiteralExpr>($1.start, $1.end, true); }
    | False          { $$ = ast.make<BoolLiteralExpr>($1.start, $1.end, false); }
    | String         { $$ = ast.make<StringLiteralExpr>($1.start, $1.end, std::move($1.lexeme)); }
    | Nil            { $$ = ast.make<NilLiteralExpr>($1.start, $1.end); }
    | LParen RParen  { $$ = ast.make<VoidLiteralExpr>($1.start, $1.end); }
    ;

upvalue_expr:
      dollars name
        {
            if (dynamic_cast<DiscardName*>($2) != nullptr) {
                RAISE_ERROR($2->start, $2->end, "`_` cannot be used as an expression");
                YYERROR;
            }
            $$ = ast.make<UpvalueExpr>($1.start, $2->end, $1.value, $2);
        }
    ;

dollars:
      Dollar          { $$ = Symbol<uint64_t>($1.start, $1.end, 1); }
    | dollars Dollar  { ++($$.value); $$.end = $2.end; }
    ;

tuple_expr:
      LParen expr Comma exprs RParen
        {
            $4.insert($4.begin(), $2);
            $$ = ast.make<TupleExpr>($1.start, $5.end, std::move($4));
        }
    ;

collection_expr:
      LBracket exprs RBracket  { $$ = ast.make<CollectionExpr>($1.start, $3.end, std::move($2)); }
    ;



pattern:
      Integer             { $$ = ast.make<IntLiteralPattern>($1.start, $1.end, BigInt($1.lexeme)); }
    | Real                { $$ = ast.make<RealLiteralPattern>($1.start, $1.end, BigReal($1.lexeme)); }
    | True                { $$ = ast.make<BoolLiteralPattern>($1.start, $1.end, true); }
    | False               { $$ = ast.make<BoolLiteralPattern>($1.start, $1.end, false); }
    | String              { $$ = ast.make<StringLiteralPattern>($1.start, $1.end, std::move($1.lexeme)); }
    | Nil                 { $$ = ast.make<NilLiteralPattern>($1.start, $1.end); }
    | LParen RParen       { $$ = ast.make<VoidLiteralPattern>($1.start, $1.end); }
    | Identifier          { $$ = ast.make<BindingPattern>($1.start, $1.end, std::move($1.lexeme)); }
    | Discard             { $$ = ast.make<WildcardPattern>($1.start, $1.end); }
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



name:
      Identifier       { $$ = ast.make<IdentifierName>($1.start, $1.end, std::move($1.lexeme)); }
    | Discard          { $$ = ast.make<DiscardName>($1.start, $1.end); }
    | Constructor      { $$ = ast.make<ConstructorName>($1.start, $1.end); }
    | Destructor       { $$ = ast.make<DestructorName>($1.start, $1.end); }
    | overloadable_op  { $$ = ast.make<OverloadableOpName>($1.start, $1.end, $1.value); }
    | Self             { $$ = ast.make<SelfName>($1.start, $1.end); }
    ;

overloadable_op:
      Operator Add                { $$ = Symbol($1.start, $2.end, OverloadableOpKind::Add); }
    | Operator Sub                { $$ = Symbol($1.start, $2.end, OverloadableOpKind::Sub); }
    | Operator Mul                { $$ = Symbol($1.start, $2.end, OverloadableOpKind::Mul); }
    | Operator Div                { $$ = Symbol($1.start, $2.end, OverloadableOpKind::Div); }
    | Operator Mod                { $$ = Symbol($1.start, $2.end, OverloadableOpKind::Mod); }
    | Operator Tide               { $$ = Symbol($1.start, $2.end, OverloadableOpKind::BitNot); }
    | Operator And                { $$ = Symbol($1.start, $2.end, OverloadableOpKind::BitAnd); }
    | Operator VBar               { $$ = Symbol($1.start, $2.end, OverloadableOpKind::BitOr); }
    | Operator Caret              { $$ = Symbol($1.start, $2.end, OverloadableOpKind::BitXor); }
    | Operator Shl                { $$ = Symbol($1.start, $2.end, OverloadableOpKind::BitShl); }
    | Operator Shr                { $$ = Symbol($1.start, $2.end, OverloadableOpKind::BitShr); }
    | Operator Bang               { $$ = Symbol($1.start, $2.end, OverloadableOpKind::LogNot); }
    | Operator LogAnd             { $$ = Symbol($1.start, $2.end, OverloadableOpKind::LogAnd); }
    | Operator LogOr              { $$ = Symbol($1.start, $2.end, OverloadableOpKind::LogOr); }
    | Operator Eq                 { $$ = Symbol($1.start, $2.end, OverloadableOpKind::Eq); }
    | Operator Ne                 { $$ = Symbol($1.start, $2.end, OverloadableOpKind::Ne); }
    | Operator Lt                 { $$ = Symbol($1.start, $2.end, OverloadableOpKind::Lt); }
    | Operator Le                 { $$ = Symbol($1.start, $2.end, OverloadableOpKind::Le); }
    | Operator Gt                 { $$ = Symbol($1.start, $2.end, OverloadableOpKind::Gt); }
    | Operator Ge                 { $$ = Symbol($1.start, $2.end, OverloadableOpKind::Ge); }
    | Operator Range              { $$ = Symbol($1.start, $2.end, OverloadableOpKind::Range); }
    | Operator RangeExcl          { $$ = Symbol($1.start, $2.end, OverloadableOpKind::RangeExcl); }
    | Operator LParen RParen      { $$ = Symbol($1.start, $3.end, OverloadableOpKind::Call); }
    | Operator LBracket RBracket  { $$ = Symbol($1.start, $3.end, OverloadableOpKind::Subscript); }
    | Operator AddAssign          { $$ = Symbol($1.start, $2.end, OverloadableOpKind::AddAssign); }
    | Operator SubAssign          { $$ = Symbol($1.start, $2.end, OverloadableOpKind::SubAssign); }
    | Operator MulAssign          { $$ = Symbol($1.start, $2.end, OverloadableOpKind::MulAssign); }
    | Operator DivAssign          { $$ = Symbol($1.start, $2.end, OverloadableOpKind::DivAssign); }
    | Operator ModAssign          { $$ = Symbol($1.start, $2.end, OverloadableOpKind::ModAssign); }
    | Operator BitAndAssign       { $$ = Symbol($1.start, $2.end, OverloadableOpKind::BitAndAssign); }
    | Operator BitOrAssign        { $$ = Symbol($1.start, $2.end, OverloadableOpKind::BitOrAssign); }
    | Operator BitXorAssign       { $$ = Symbol($1.start, $2.end, OverloadableOpKind::BitXorAssign); }
    | Operator ShlAssign          { $$ = Symbol($1.start, $2.end, OverloadableOpKind::BitShlAssign); }
    | Operator ShrAssign          { $$ = Symbol($1.start, $2.end, OverloadableOpKind::BitShrAssign); }
    ;
%%

void yy::parser::error(const yy::location& loc, const std::string& msg) {
    Location start(static_cast<size_t>(loc.begin.line), static_cast<size_t>(loc.begin.column));
    Location end(static_cast<size_t>(loc.end.line), static_cast<size_t>(loc.end.column));
    errors.emplace_back(msg, start, end);
}
