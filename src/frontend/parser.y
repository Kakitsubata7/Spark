%require "3.5"

%skeleton "lalr1.cc"

%language "c++"

%define api.token.raw
%define api.value.type variant
%define parse.error verbose

%locations

%lex-param {yyscan_t scanner}
%parse-param {yyscan_t scanner} {yy::parser::location_type* yylloc}

%code requires {
#include <cstdint>
#include <vector>

#include "frontend/ast/node.hpp"
#include "frontend/lexer/token_value.hpp"

typedef void* yyscan_t;

namespace yy {
    template <typename T>
    struct Symbol {
        Spark::FrontEnd::Location start;
        Spark::FrontEnd::Location end;
        T value;

        Symbol() = default;
        Symbol(Spark::FrontEnd::Location start, Spark::FrontEnd::Location end, T value) noexcept
            : start(start), end(end), value(std::move(value)) { }
    };
} // yy
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

%token <Spark::FrontEnd::TokenValue> Identifier Wildcard
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

%right Assign AddAssign SubAssign MulAssign DivAssign ModAssign BitAndAssign BitOrAssign BitXorAssign
%right CoalesceAssign

%type <std::vector<Spark::FrontEnd::Node*>> stmts stmt_list
%type <Spark::FrontEnd::Node*> stmt expr lambda block primary identifier literal
%type <uint64_t> dollars

%%
%start program;

program:
      stmts  { ast.root = ast.make<Block>($1.front()->start, $1.back()->end, std::move($1)); }
    ;

stmts:
      /* empty */
    | stmt_list
    | stmt_list terminators
    ;

stmt_list:
      stmt                        { $$.push_back($1); }
    | stmt_list terminators stmt  { $1.push_back($3); }
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
      binding
    | assign_stmt
    | fndef_stmt
    | annotations fndef_stmt
    | typedef_stmt
    | annotations typedef_stmt
    | enumcase_stmt
    | annotations enumcase_stmt
    | ifelse
    | While expr block           { $$ = ast.make<While>($1.start, $3->end, $2, $3); }
    | For expr In expr block     { $$ = ast.make<For>($1.start, $5->end, $2, $4, $5); }
    | Break                      { $$ = ast.make<Break>($1.start, $1.end); }
    | Continue                   { $$ = ast.make<Continue>($1.start, $1.end); }
    | Return                     { $$ = ast.make<Return>($1.start, $1.end, nullptr); }
    | Return expr                { $$ = ast.make<Return>($1.start, $2->end, $2); }
    | module_stmt
    | annotations module_stmt
    | Export stmt                { $$ = ast.make<Export>($1.start, $2->end, $2); }
    | import_stmt
    | Undefine postfix           { $$ = ast.make<Undefine>($1.start, $2->end, $2); }
    | expr
    ;

assign_stmt:
      pattern assign_op assign_rhs  { $$ = ast.make<Assign>($1->start, $3->end, $1, $3, $2); }
    ;

assign_op:
      Assign          { $$ = Assign::OpKind::Assign; }
    | AddAssign       { $$ = Assign::OpKind::AddAssign; }
    | SubAssign       { $$ = Assign::OpKind::SubAssign; }
    | MulAssign       { $$ = Assign::OpKind::MulAssign; }
    | DivAssign       { $$ = Assign::OpKind::DivAssign; }
    | ModAssign       { $$ = Assign::OpKind::ModAssign; }
    | BitAndAssign    { $$ = Assign::OpKind::BitAndAssign; }
    | BitOrAssign     { $$ = Assign::OpKind::BitOrAssign; }
    | BitXorAssign    { $$ = Assign::OpKind::BitXorAssign; }
    | ShlAssign       { $$ = Assign::OpKind::BitShrAssign; }
    | ShrAssign       { $$ = Assign::OpKind::BitShlAssign; }
    | CoalesceAssign  { $$ = Assign::OpKind::CoalesceAssign; }
    ;

assign_rhs:
      expr
    | assign_stmt
    ;

fndef_stmt:
      fn fn_name params captures fn_ret fn_throw block
    | fn fn_name params captures fn_ret fn_throw FatArrow expr
    ;

fn:
      Fn
    | fn Caret
    ;

fn_name:
      identifier
    | Constructor
    | Destructor
    | overloadable_op
    ;

params:
      LParen RParen
    | LParen param_list RParen
    ;

param_list:
      param
    | param_list Comma param
    ;

param:
      param_element
    | annotations param_element
    ;

param_element:
      identifier
    | identifier Colon type
    | varmod identifier
    | varmod identifier Colon type
    ;

captures:
      LBracket RBracket
    | LBracket capture_list RBracket
    ;

capture_list:
      capture
    | capture_list Comma capture
    ;

capture:
      postfix
    | varmod postfix
    | Range
    | varmod Range
    ;

fn_ret:
      /* empty */
    | Arrow rets
    ;

rets:
      ret
    | rets Comma ret
    ;

ret:
      type
    | Ref type
    | Cref type
    ;

fn_throw:
      /* empty */
    | Throw
    | Throw type
    ;

typedef_stmt:
      typedef_kind typedef_name block
        {
            $$ = ast.make<TypeDef>($1.start, $3->end, $1.value, false, $2, {}, $3);
        }
    | typedef_kind Caret typedef_name block
        {
            $$ = ast.make<TypeDef>($1.start, $4->end, $1.value, true, $3, {}, $4);
        }
    | typedef_kind typedef_name Colon exprs block // TODO: Decide whether type should be expr or binary
        {
            $$ = ast.make<TypeDef>($1.start, $5->end, $1.value, false, $2, std::move($4), $5);
        }
    | typedef_kind Caret typedef_name Colon exprs block
        {
            $$ = ast.make<TypeDef>($1.start, $6->end, $1.value, true, $3, std::move($5), $6);
        }
    ;

typedef_kind:
      Struct       { $$ = Symbol($1.start, $1.end, TypeDef::TypeKind::Struct); }
    | Class        { $$ = Symbol($1.start, $1.end, TypeDef::TypeKind::Class); }
    | Enum         { $$ = Symbol($1.start, $1.end, TypeDef::TypeKind::Enum); }
    | Enum Struct  { $$ = Symbol($1.start, $2.end, TypeDef::TypeKind::EnumStruct); }
    | Enum Class   { $$ = Symbol($1.start, $2.end, TypeDef::TypeKind::EnumClass); }
    | Trait        { $$ = Symbol($1.start, $1.end, TypeDef::TypeKind::Trait); }
    | Alias        { $$ = Symbol($1.start, $1.end, TypeDef::TypeKind::Alias); }
    | Extension    { $$ = Symbol($1.start, $1.end, TypeDef::TypeKind::Extension); }
    ;

typedef_name:
      identifier
    | wildcard
    | constructor
    | destructor
    | overloadable_op
    ;

enumcase_stmt:
      Case case_name
    | Case case_name Assign expr
    | Case adt_constructor
    ;

case_name:
      identifier
    | wildcard
    ;

adt_constructor:
      identifier LParen RParen
    | identifier LParen positional_adt_members RParen
    | identifier LParen named_adt_members RParen
    ;

positional_adt_members:
      type
    | positional_adt_members Comma type
    ;

named_adt_members:
      named_adt_member
    | named_adt_members Comma named_adt_member
    ;

named_adt_member:
      identifier Colon type
    ;

module_stmt:
      Module module_path block  { $$ = ast.make<Module>($1.start, $3->end, std::move($2), $3); }
    ;

module_path:
      module_name                  { $$.clear(); $$.push_back($1); }
    | module_path Dot module_name  { $$.push_back($3); }
    ;

module_name:
      identifier
    | wildcard
    ;

ifelse:
      If expr block else  { $$ = ast.make<IfElse>($1.start, $4->end, $2, $3, $4); }
    ;

else:
      /* empty */  { $$ = nullptr; }
    | Else block   { $$ = $2; }
    | Else ifelse  { $$ = $2; }
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
      lambda
    | annotations lambda
    | If expr Then expr Else expr  { $$ = ast.make<IfThen>($1.start, $6->end, $2, $4, $6); }
    | Try expr Else expr           { $$ = ast.make<TryElse>($1.start, $4->end, $2, $4); }
    | match
    | trycatch
    | Throw expr                   { $$ = ast.make<Throw>($1.start, $2->end, $2); }
    | block
    | binary
    ;

exprs:
      expr
    | exprs Comma expr
    ;

type:
      binary
    ;

lambda:
      fn params captures fn_ret fn_throw FatArrow expr
    ;

match:
      Match expr LBrace cases RBrace { $$ = ast.make<Match>($1.start, $5.end, nullptr, $2, std::move($4)); }
    | Match pattern Assign expr LBrace cases RBraces { $$ = ast.make<Match>($1.start, $7.end, $2, $4, std::move($6);); }
    ;

cases:
      case        { $$.clear(); $$.push_back($1); }
    | cases case  { $$.push_back($2); }
    ;

case:
      Case pattern FatArrow expr          { $$ = Match::Case($2, nullptr, $4); }
    | Case If expr FatArrow expr          { $$ = Match::Case(nullptr, $3, $5); }
    | Case pattern If expr FatArrow expr  { $$ = Match::Case($2, $4, $6); }
    ;

trycatch:
      Try expr LBrace catches RBrace  { $$ = ast.make<TryCatch>($1.start, $5.end, $2, std::move($4)); }
    ;

catches:
      catch          { $$.clear(); $$.push_back($1); }
    | catches catch  { $$.push_back($2); }
    ;

catch:
      Catch pattern FatArrow expr          { $$ = TryCatch::Catch($2, nullptr, $4); }
    | Catch If expr FatArrow expr          { $$ = TryCatch::Catch(nullptr, $3, $5); }
    | Catch pattern If expr FatArrow expr  { $$ = TryCatch::Catch($2, $4, $6); }
    ;

block:
      LBrace stmts RBrace  { $$ = make<Block>($1.start, $3.end, std::move($2)); }
    ;

binary:
      binary_is
    ;

binary_is:
      binary_pipe Is pattern
    | binary_pipe Is binary_pipe
    | binary_pipe
    ;

binary_pipe:
      binary_type Pipe binary_pipe
        {
            $$ = ast.make<BinaryExpr>($1->start, $3->end, $1, $3, BinaryExpr::OpKind::Pipe);
        }
    | binary_type
    ;

binary_type:
      binary_range Arrow binary_type
        {
            $$ = ast.make<BinaryExpr>($1->start, $3->end, $1, $3, BinaryExpr::OpKind::FuncType);
        }
    | binary_range
    ;

binary_range:
      binary_coalesce Range binary_coalesce
        {
            $$ = ast.make<BinaryExpr>($1->start, $3->end, $1, $3, BinaryExpr::OpKind::Range);
        }
    | binary_coalesce RangeExcl binary_coalesce
        {
            $$ = ast.make<BinaryExpr>($1->start, $3->end, $1, $3, BinaryExpr::OpKind::RangeExcl);
        }
    | binary_coalesce
    ;

binary_coalesce:
      binary_logor Coalesce binary_coalesce
        {
            $$ = ast.make<BinaryExpr>($1->start, $3->end, $1, $3, BinaryExpr::OpKind::Coalesce);
        }
    | binary_logor
    ;

binary_logor:
      binary_logor LogOr binary_logand
        {
            $$ = ast.make<BinaryExpr>($1->start, $3->end, $1, $3, BinaryExpr::OpKind::LogOr);
        }
    | binary_logand
    ;

binary_logand:
      binary_logand LogAnd binary_bitor
        {
            $$ = ast.make<BinaryExpr>($1->start, $3->end, $1, $3, BinaryExpr::OpKind::LogAnd);
        }
    | binary_bitor
    ;

binary_bitor:
      binary_bitor VBar binary_bitxor
        {
            $$ = ast.make<BinaryExpr>($1->start, $3->end, $1, $3, BinaryExpr::OpKind::BitOr);
        }
    | binary_bitxor
    ;

binary_bitxor:
      binary_bitxor Caret binary_bitand
        {
            $$ = ast.make<BinaryExpr>($1->start, $3->end, $1, $3, BinaryExpr::OpKind::BitXor);
        }
    | binary_bitand
    ;

binary_bitand:
      binary_bitand And binary_eq
        {
            $$ = ast.make<BinaryExpr>($1->start, $3->end, $1, $3, BinaryExpr::OpKind::BitAnd);
        }
    | binary_eq
    ;

binary_eq:
      binary_compare Eq binary_compare
        {
            $$ = ast.make<BinaryExpr>($1->start, $3->end, $1, $3, BinaryExpr::OpKind::Eq);
        }
    | binary_compare Ne binary_compare
        {
            $$ = ast.make<BinaryExpr>($1->start, $3->end, $1, $3, BinaryExpr::OpKind::Ne);
        }
    | binary_compare StrictEq binary_compare
        {
            $$ = ast.make<BinaryExpr>($1->start, $3->end, $1, $3, BinaryExpr::OpKind::StrictEq);
        }
    | binary_compare StrictNe binary_compare
        {
            $$ = ast.make<BinaryExpr>($1->start, $3->end, $1, $3, BinaryExpr::OpKind::StrictNe);
        }
    | binary_compare
    ;

binary_compare:
      binary_bitshift Lt binary_bitshift
        {
            $$ = ast.make<BinaryExpr>($1->start, $3->end, $1, $3, BinaryExpr::OpKind::Lt);
        }
    | binary_bitshift Le binary_bitshift
        {
            $$ = ast.make<BinaryExpr>($1->start, $3->end, $1, $3, BinaryExpr::OpKind::Le);
        }
    | binary_bitshift Gt binary_bitshift
        {
            $$ = ast.make<BinaryExpr>($1->start, $3->end, $1, $3, BinaryExpr::OpKind::Gt);
        }
    | binary_bitshift Ge binary_bitshift
        {
            $$ = ast.make<BinaryExpr>($1->start, $3->end, $1, $3, BinaryExpr::OpKind::Ge);
        }
    | binary_bitshift
    ;

binary_bitshift:
      binary_bitshift Shl binary_additive
        {
            $$ = ast.make<BinaryExpr>($1->start, $3->end, $1, $3, BinaryExpr::OpKind::BitShl);
        }
    | binary_bitshift Shr binary_additive
        {
            $$ = ast.make<BinaryExpr>($1->start, $3->end, $1, $3, BinaryExpr::OpKind::BitShr);
        }
    | binary_additive
    ;

binary_additive:
      binary_additive Add binary_multiplicative
        {
            $$ = ast.make<BinaryExpr>($1->start, $3->end, $1, $3, BinaryExpr::OpKind::Add);
        }
    | binary_additive Sub binary_multiplicative
        {
            $$ = ast.make<BinaryExpr>($1->start, $3->end, $1, $3, BinaryExpr::OpKind::Sub);
        }
    | binary_multiplicative
    ;

binary_multiplicative:
      binary_multiplicative Mul prefix
        {
            $$ = ast.make<BinaryExpr>($1->start, $3->end, $1, $3, BinaryExpr::OpKind::Mul);
        }
    | binary_multiplicative Div prefix
        {
            $$ = ast.make<BinaryExpr>($1->start, $3->end, $1, $3, BinaryExpr::OpKind::Div);
        }
    | binary_multiplicative Mod prefix
        {
            $$ = ast.make<BinaryExpr>($1->start, $3->end, $1, $3, BinaryExpr::OpKind::Mod);
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
      primary
    | postfix Question         { $$ = ast.make<PostfixExpr>($1->start, $2.end, PostfixExpr::OpKind::Optional, $1); }
    | postfix NonNull          { $$ = ast.make<PostfixExpr>($1->start, $2.end, PostfixExpr::OpKind::NonNull, $1); }
    | postfix Bang             { $$ = ast.make<PostfixExpr>($1->start, $2.end, PostfixExpr::OpKind::ForceUnwrap, $1); }
    | postfix Dot member_name  { $$ = ast.make<MemberAccess>($1->start, $3->end, $1, $3); }
    | postfix LParen call_args RParen
    | postfix LBracket subscript_args RBracket
        {
            $$ = ast.make<Subscript>($1->start, $4->end, $1, std::move($3));
        }
    ;

member_name:
      identifier
    | constructor
    | destructor
    | overloadable_op
    ;

call_args:
      /* empty */
    | positional_args
    | positional_args Comma named_args
    | named_args
    ;

positional_args:
      pattern
    | positional_args Comma pattern
    ;

named_args:
      named_arg
    | named_args Comma named_arg
    ;

named_arg:
      identifier Colon pattern
    ;

subscript_args:
      expr                       { $$.clear(); $$.push_back($1); }
    | subscript_args Comma expr  { $$.push_back($3); }
    ;

primary:
      literal
    | identifier
    | wildcard
    | global
    | self
    | destructor
    | overloadable_op
    | upvalue
    | LParen expr RParen         { $$ = $2; }
    | tuple
    | collection
    | Typeof LParen expr RParen  { $$ = ast.make<Typeof>($1.start, $4.end, $3); }
    ;

literal:
      Integer        { $$ = ast.make<IntLiteral>($1.start, $1.end, BigInt($1.lexeme)); }
    | Real           { $$ = ast.make<RealLiteral>($1.start, $1.end, BigReal($1.lexeme)); }
    | True           { $$ = ast.make<BoolLiteral>($1.start, $1.end, true); }
    | False          { $$ = ast.make<BoolLiteral>($1.start, $1.end, false); }
    | String         { $$ = ast.make<StringLiteral>($1.start, $1.end, std::move($1.lexeme)); }
    | Nil            { $$ = ast.make<NilLiteral>($1.start, $1.end); }
    | LParen RParen  { $$ = ast.make<VoidLiteral>($1.start, $1.end); }
    ;

identifier:
      Identifier  { $$ = ast.make<Identifier>($1.start, $1.end, std::move($1.lexeme)); }
    ;

wildcard:
      Wildcard  { $$ = ast.make<Wildcard>($1.start, $1.end); }
    ;

global:
      Global  { $$ = ast.make<Global>($1.start, $1.end); }
    ;

self:
      Self  { $$ = ast.make<Self>($1.start, $1.end); }
    ;

constructor:
      Constructor  { $$ = ast.make<Constructor>($1.start, $1.end); }
    ;

destructor:
      Destructor  { $$ = ast.make<Destructor>($1.start, $1.end); }
    ;

overloadable_op:
      Operator Add            { $$ = ast.make<OverloadableOp>($1.start, $2.end, OverloadableOp::OpKind::Add); }
    | Operator Sub            { $$ = ast.make<OverloadableOp>($1.start, $2.end, OverloadableOp::OpKind::Sub); }
    | Operator Mul            { $$ = ast.make<OverloadableOp>($1.start, $2.end, OverloadableOp::OpKind::Mul); }
    | Operator Div            { $$ = ast.make<OverloadableOp>($1.start, $2.end, OverloadableOp::OpKind::Div); }
    | Operator Mod            { $$ = ast.make<OverloadableOp>($1.start, $2.end, OverloadableOp::OpKind::Mod); }
    | Operator Tide           { $$ = ast.make<OverloadableOp>($1.start, $2.end, OverloadableOp::OpKind::BitNot); }
    | Operator And            { $$ = ast.make<OverloadableOp>($1.start, $2.end, OverloadableOp::OpKind::BitAnd); }
    | Operator VBar           { $$ = ast.make<OverloadableOp>($1.start, $2.end, OverloadableOp::OpKind::BitOr); }
    | Operator Caret          { $$ = ast.make<OverloadableOp>($1.start, $2.end, OverloadableOp::OpKind::BitXor); }
    | Operator Shl            { $$ = ast.make<OverloadableOp>($1.start, $2.end, OverloadableOp::OpKind::BitShl); }
    | Operator Shr            { $$ = ast.make<OverloadableOp>($1.start, $2.end, OverloadableOp::OpKind::BitShr); }
    | Operator Bang           { $$ = ast.make<OverloadableOp>($1.start, $2.end, OverloadableOp::OpKind::LogNot); }
    | Operator LogAnd         { $$ = ast.make<OverloadableOp>($1.start, $2.end, OverloadableOp::OpKind::LogAnd); }
    | Operator LogOr          { $$ = ast.make<OverloadableOp>($1.start, $2.end, OverloadableOp::OpKind::LogOr); }
    | Operator Eq             { $$ = ast.make<OverloadableOp>($1.start, $2.end, OverloadableOp::OpKind::Eq); }
    | Operator Ne             { $$ = ast.make<OverloadableOp>($1.start, $2.end, OverloadableOp::OpKind::Ne); }
    | Operator Lt             { $$ = ast.make<OverloadableOp>($1.start, $2.end, OverloadableOp::OpKind::Lt); }
    | Operator Le             { $$ = ast.make<OverloadableOp>($1.start, $2.end, OverloadableOp::OpKind::Le); }
    | Operator Gt             { $$ = ast.make<OverloadableOp>($1.start, $2.end, OverloadableOp::OpKind::Gt); }
    | Operator Ge             { $$ = ast.make<OverloadableOp>($1.start, $2.end, OverloadableOp::OpKind::Ge); }
    | Operator Range          { $$ = ast.make<OverloadableOp>($1.start, $2.end, OverloadableOp::OpKind::Range); }
    | Operator RangeExcl      { $$ = ast.make<OverloadableOp>($1.start, $2.end, OverloadableOp::OpKind::RangeExcl); }
    | Operator LParen RParen  { $$ = ast.make<OverloadableOp>($1.start, $3.end, OverloadableOp::OpKind::Call); }
    | Operator LBracket RBracket
        {
            $$ = ast.make<OverloadableOp>($1.start, $3.end, OverloadableOp::OpKind::Subscript);
        }
    | Operator AddAssign      { $$ = ast.make<OverloadableOp>($1.start, $2.end, OverloadableOp::OpKind::AddAssign); }
    | Operator SubAssign      { $$ = ast.make<OverloadableOp>($1.start, $2.end, OverloadableOp::OpKind::SubAssign); }
    | Operator MulAssign      { $$ = ast.make<OverloadableOp>($1.start, $2.end, OverloadableOp::OpKind::MulAssign); }
    | Operator DivAssign      { $$ = ast.make<OverloadableOp>($1.start, $2.end, OverloadableOp::OpKind::DivAssign); }
    | Operator ModAssign      { $$ = ast.make<OverloadableOp>($1.start, $2.end, OverloadableOp::OpKind::ModAssign); }
    | Operator BitAndAssign   { $$ = ast.make<OverloadableOp>($1.start, $2.end, OverloadableOp::OpKind::BitAndAssign); }
    | Operator BitOrAssign    { $$ = ast.make<OverloadableOp>($1.start, $2.end, OverloadableOp::OpKind::BitOrAssign); }
    | Operator BitXorAssign   { $$ = ast.make<OverloadableOp>($1.start, $2.end, OverloadableOp::OpKind::BitXorAssign); }
    | Operator ShlAssign      { $$ = ast.make<OverloadableOp>($1.start, $2.end, OverloadableOp::OpKind::BitShlAssign); }
    | Operator ShrAssign      { $$ = ast.make<OverloadableOp>($1.start, $2.end, OverloadableOp::OpKind::BitShrAssign); }
    ;

upvalue:
      dollars identifier       { $$ = ast.make<Upvalue>($1.start, $2->end, $1.value, $2); }
    | dollars wildcard         { $$ = ast.make<Upvalue>($1.start, $2->end, $1.value, $2); }
    | dollars self             { $$ = ast.make<Upvalue>($1.start, $2->end, $1.value, $2); }
    | dollars constructor      { $$ = ast.make<Upvalue>($1.start, $2->end, $1.value, $2); }
    | dollars destructor       { $$ = ast.make<Upvalue>($1.start, $2->end, $1.value, $2); }
    | dollars overloadable_op  { $$ = ast.make<Upvalue>($1.start, $2->end, $1.value, $2); }
    ;

dollars:
      Dollar          { $$ = Symbol(1); }
    | dollars Dollar  { ++($$.value); $$.end = $2.end; }
    ;

tuple:
      LParen pattern Comma patterns RParen
    ;

/**
 * [], [...], [patterns], [..., patterns], [patterns, ...], [patterns, ..., patterns]
 */
collection:
      // []
      LBracket RBracket
      // [...]
    | LBracket Range RBracket
      // [patterns]
    | LBracket patterns RBracket
      // [..., patterns]
    | LBracket Range Comma patterns RBracket
      // [patterns, ...]
    | LBracket patterns Comma Range RBracket
      // [patterns, ..., patterns]
    | LBracket patterns Comma Range Comma patterns RBracket
    ;



pattern:
      binding
    | expr
    ;

patterns:
      pattern
    | patterns Comma pattern
    ;

binding:
      bind
    | annotations bind
    ;

bind:
      binding_mod identifier
    | binding_mod identifier Colon type
    ;

binding_mod:
      binding_kind
        {
            $$ = ast.make<BindingModifier>($1.start, $1.end, $1.value, false, BindingModifier::Optionality::None);
        }
    | binding_kind Caret
        {
            $$ = ast.make<BindingModifier>($1.start, $2.end, $1.value, true, BindingModifier::Optionality::None);
        }
    | binding_kind Question
        {
            $$ = ast.make<BindingModifier>($1.start, $2.end, $1.value, false, BindingModifier::Optionality::Optional);
        }
    | binding_kind NonNull
        {
            $$ = ast.make<BindingModifier>($1.start, $2.end, $1.value, false,
                                           BindingModifier::Optionality::OptionalNonNull);
        }
    ;

binding_kind:
      Let    { $$ = Symbol($1.start, $1.end, BindingModifier::BindingKind::Let); }
    | Const  { $$ = Symbol($1.start, $1.end, BindingModifier::BindingKind::Const); }
    | Ref    { $$ = Symbol($1.start, $1.end, BindingModifier::BindingKind::Ref); }
    | Cref   { $$ = Symbol($1.start, $1.end. BindingModifier::BindingKind::Cref); }
    ;



annotation:
      At annot_name
    | At annot_name LParen call_args RParen
    ;

annotations:
      annotation
    | annotations annotation
    ;

annot_name:
      identifier
    | annot_name Dot identifier
    ;
%%

void yy::parser::error(const yy::location& loc, const std::string& msg) {
    Location start(static_cast<size_t>(loc.begin.line), static_cast<size_t>(loc.begin.column));
    Location end(static_cast<size_t>(loc.end.line), static_cast<size_t>(loc.end.column));
}
