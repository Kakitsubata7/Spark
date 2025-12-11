#pragma once

namespace Spark::FrontEnd {

struct Node;

struct Terms;

struct Operation;

struct Identifier;
struct UpvalueIdentifier;
struct Discard;

struct IntLiteral;
struct RealLiteral;
struct BoolLiteral;
struct StringLiteral;
struct Nil;
struct Undefined;

struct VarDef;
struct TypeDef;
struct Block;
struct IfKeyword;
struct ThenKeyword;
struct ElseKeyword;
struct MatchKeyword;
struct CaseKeyword;
struct While;
struct DoWhile;
struct For;
struct InKeyword;
struct Break;
struct Continue;
struct Return;
struct Is;
struct Typeof;
struct Throw;
struct TryKeyword;
struct Catch;
struct Module;
struct Export;
struct From;
struct Import;

struct Annotation;

struct Semicolon;

class NodeVisitor {
public:
    virtual ~NodeVisitor() = default;

    virtual void visit(Node& node) { }

    virtual void visit(Terms& terms) { }

    virtual void visit(Operation& op) { }

    virtual void visit(Identifier& ident) { }
    virtual void visit(UpvalueIdentifier& ident) { }
    virtual void visit(Discard& discard) { }

    virtual void visit(IntLiteral& intl) { }
    virtual void visit(RealLiteral& real) { }
    virtual void visit(BoolLiteral& booll) { }
    virtual void visit(StringLiteral& strl) { }
    virtual void visit(Nil& nil) { }
    virtual void visit(Undefined& undef) { }

    virtual void visit(VarDef& vdef) { }
    virtual void visit(TypeDef& tdef) { }
    virtual void visit(Block& block) { }
    virtual void visit(IfKeyword& ifk) { }
    virtual void visit(ThenKeyword& thenk) { }
    virtual void visit(ElseKeyword& elsek) { }
    virtual void visit(MatchKeyword& matchk) { }
    virtual void visit(CaseKeyword& casek) { }
    virtual void visit(While& whilen) { }
    virtual void visit(DoWhile& dowhile) { }
    virtual void visit(For& fors) { }
    virtual void visit(InKeyword& ink) { }
    virtual void visit(Break& breaks) { }
    virtual void visit(Continue& cont) { }
    virtual void visit(Return& ret) { }
    virtual void visit(Is& is) { }
    virtual void visit(Typeof& typeofn) { }
    virtual void visit(Throw& thrown) { }
    virtual void visit(TryKeyword& tryk) { }
    virtual void visit(Catch& catchn) { }
    virtual void visit(Module& mod) { }
    virtual void visit(Export& ex) { }
    virtual void visit(From& from) { }
    virtual void visit(Import& imp) { }

    virtual void visit(Annotation& node) { }

    virtual void visit(Semicolon& node) { }
};

} // Spark::FrontEnd
