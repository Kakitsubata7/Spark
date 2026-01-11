#include "token_type.hpp"

namespace Spark::FrontEnd {

std::string_view toString(TokenType type) noexcept {
    switch (type) {
        case TokenType::EndOfFile: return "EndOfFile";
        case TokenType::Identifier: return "Identifier";
        case TokenType::Discard: return "Discard";
        case TokenType::Integer: return "Integer";
        case TokenType::Real: return "Real";
        case TokenType::String: return "String";
        case TokenType::Alias: return "Alias";
        case TokenType::As: return "As";
        case TokenType::Break: return "Break";
        case TokenType::Case: return "Case";
        case TokenType::Catch: return "Catch";
        case TokenType::Class: return "Class";
        case TokenType::Const: return "Const";
        case TokenType::Constructor: return "Constructor";
        case TokenType::Continue: return "Continue";
        case TokenType::Cref: return "Cref";
        case TokenType::Destructor: return "Destructor";
        case TokenType::Do: return "Do";
        case TokenType::Else: return "Else";
        case TokenType::End: return "End";
        case TokenType::Enum: return "Enum";
        case TokenType::Export: return "Export";
        case TokenType::Extension: return "Extension";
        case TokenType::False: return "False";
        case TokenType::Fn: return "Fn";
        case TokenType::For: return "For";
        case TokenType::From: return "From";
        case TokenType::Global: return "Global";
        case TokenType::If: return "If";
        case TokenType::Import: return "Import";
        case TokenType::In: return "In";
        case TokenType::Is: return "Is";
        case TokenType::Let: return "Let";
        case TokenType::Match: return "Match";
        case TokenType::Module: return "Module";
        case TokenType::Nil: return "Nil";
        case TokenType::Operator: return "Operator";
        case TokenType::Ref: return "Ref";
        case TokenType::Return: return "Return";
        case TokenType::Self: return "Self";
        case TokenType::Struct: return "Struct";
        case TokenType::Super: return "Super";
        case TokenType::Then: return "Then";
        case TokenType::Throw: return "Throw";
        case TokenType::Trait: return "Trait";
        case TokenType::True: return "True";
        case TokenType::Try: return "Try";
        case TokenType::Typeof: return "Typeof";
        case TokenType::Undefine: return "Undefine";
        case TokenType::While: return "While";
        case TokenType::Add: return "Add";
        case TokenType::Sub: return "Sub";
        case TokenType::Mul: return "Mul";
        case TokenType::Div: return "Div";
        case TokenType::Mod: return "Mod";
        case TokenType::Tide: return "Tide";
        case TokenType::And: return "And";
        case TokenType::VBar: return "VBar";
        case TokenType::Caret: return "Caret";
        case TokenType::Bang: return "Bang";
        case TokenType::LogAnd: return "LogAnd";
        case TokenType::LogOr: return "LogOr";
        case TokenType::Eq: return "Eq";
        case TokenType::Ne: return "Ne";
        case TokenType::Lt: return "Lt";
        case TokenType::Gt: return "Gt";
        case TokenType::Le: return "Le";
        case TokenType::Ge: return "Ge";
        case TokenType::Assign: return "Assign";
        case TokenType::AddAssign: return "AddAssign";
        case TokenType::SubAssign: return "SubAssign";
        case TokenType::MulAssign: return "MulAssign";
        case TokenType::DivAssign: return "DivAssign";
        case TokenType::ModAssign: return "ModAssign";
        case TokenType::BitAndAssign: return "BitAndAssign";
        case TokenType::BitOrAssign: return "BitOrAssign";
        case TokenType::BitXorAssign: return "BitXorAssign";
        case TokenType::CoalesceAssign: return "CoalesceAssign";
        case TokenType::StrictEq: return "StrictEq";
        case TokenType::StrictNe: return "StrictNe";
        case TokenType::Range: return "Range";
        case TokenType::RangeExcl: return "RangeExcl";
        case TokenType::Question: return "Question";
        case TokenType::NonNull: return "NonNull";
        case TokenType::Coalesce: return "Coalesce";
        case TokenType::Pipe: return "Pipe";
        case TokenType::Dot: return "Dot";
        case TokenType::Comma: return "Comma";
        case TokenType::Colon: return "Colon";
        case TokenType::Arrow: return "Arrow";
        case TokenType::FatArrow: return "FatArrow";
        case TokenType::Semicolon: return "Semicolon";
        case TokenType::LParen: return "LParen";
        case TokenType::RParen: return "RParen";
        case TokenType::LBracket: return "LBracket";
        case TokenType::RBracket: return "RBracket";
        case TokenType::LBrace: return "LBrace";
        case TokenType::RBrace: return "RBrace";
        case TokenType::LineComment: return "LineComment";
        case TokenType::BlockComment: return "BlockComment";
        case TokenType::At: return "At";
        case TokenType::Dollar: return "Dollar";
        default: return "Unknown";
    }
}

std::ostream& operator<<(std::ostream& os, TokenType type) {
    os << toString(type);
    return os;
}

} // Spark::FrontEnd
