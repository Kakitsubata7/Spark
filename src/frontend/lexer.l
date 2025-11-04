%option bison-bridge reentrant noyywrap noyylineno nounput noinput
%x IN_STRING IN_BLOCK_COMMENT

%top {
#include <parser.tab.hpp>

#include "frontend/token_type.hpp"
#include "frontend/token_value.hpp"

#define YYSTYPE Spark::FrontEnd::TokenValue
}

%{
#include "frontend/lexer_utils.hpp"

using namespace Spark::FrontEnd;

#define ltext (yyget_text(yyscanner))
#define lleng (static_cast<size_t>(yyget_leng(yyscanner)))
#define llval (yyget_lval(yyscanner))
#define lextra (yyget_extra(yyscanner))
#define lstate (*static_cast<LexerState*>(yyget_extra(yyscanner)))

#define EMIT_TOKEN() (*llval = makeToken(std::string_view(ltext, lleng), lstate))

#define YY_INPUT(buf, result, max_size)                                  \
{                                                                        \
    LexerState& ls = *static_cast<LexerState*>(yyget_extra(yyscanner));  \
    ls.stream->read(reinterpret_cast<char*>(buf), max_size);             \
    result = ls.stream->gcount();                                        \
    if (result == 0) {                                                   \
        result = YY_NULL;                                                \
    }                                                                    \
}
%}

%%

"\r\n" { handleNewline(lstate); }
"\n" { handleNewline(lstate); }
"\r" { handleNewline(lstate); }

[ \t\r]+ {
    consumeCharacters(lleng, lstate);
}

[0-9_]*\.[0-9_]+ {
    *llval = makeToken(std::string_view(ltext, lleng), lstate);
    return TokenType::Real;
}

[0-9_]+|0[bB][0-9_]+|0[oO][0-9_]+|0[xX][0-9A-Za-z_]+ {
    *llval = makeToken(std::string_view(ltext, lleng), lstate);
    return TokenType::Integer;
}

[A-Za-z_][A-Za-z0-9_]* {
    std::string_view word(ltext, lleng);
    *llval = makeToken(word, lstate);
    return classifyWord(word);
}

"//"[^\n]* {
    *llval = makeToken(std::string_view(ltext, lleng).substr(2), lstate);
    return TokenType::LineComment;
}

"/*" {
    consumeCharacters(2, lstate);
    clearTokenBuffer(lstate);
    BEGIN(IN_BLOCK_COMMENT);
}

<IN_BLOCK_COMMENT>{
    "*/" {
        consumeCharacters(2, lstate);
        *llval = makeToken(lstate.tokenBuffer, lstate);
        clearTokenBuffer(lstate);
        BEGIN(INITIAL);
        return TokenType::BlockComment;
    }

    [^*\n]+ {
        size_t n = lleng;
        consumeCharacters(n, lstate);
        appendTokenBuffer(lstate, std::string_view(ltext, n));
    }

    "*" {
        consumeCharacters(1, lstate);
        appendTokenBuffer(lstate, '*');
    }

    "\n" {
        handleNewline(lstate);
        appendTokenBuffer(lstate, '\n');
    }

    <<EOF>> {
        raiseError(lstate, "unterminated block comment");
        return TokenType::BlockComment;
    }
}

"+=" { EMIT_TOKEN(); return TokenType::AddAssign; }
"+" { EMIT_TOKEN(); return TokenType::Add; }

"->" { EMIT_TOKEN(); return TokenType::Arrow; }
"-=" { EMIT_TOKEN(); return TokenType::SubAssign; }
"-" { EMIT_TOKEN(); return TokenType::Sub; }

"*=" { EMIT_TOKEN(); return TokenType::MulAssign; }
"*" { EMIT_TOKEN(); return TokenType::Mul; }

"/=" { EMIT_TOKEN(); return TokenType::DivAssign; }
"/" { EMIT_TOKEN(); return TokenType::Div; }

"%=" { EMIT_TOKEN(); return TokenType::ModAssign; }
"%" { EMIT_TOKEN(); return TokenType::Mod; }

"~" { EMIT_TOKEN(); return TokenType::BitNot; }

"&&" { EMIT_TOKEN(); return TokenType::LogAnd; }
"&=" { EMIT_TOKEN(); return TokenType::BitAndAssign; }
"&" { EMIT_TOKEN(); return TokenType::BitAnd; }

"||" { EMIT_TOKEN(); return TokenType::LogOr; }
"|=" { EMIT_TOKEN(); return TokenType::BitOrAssign; }
"|" { EMIT_TOKEN(); return TokenType::BitOr; }

"^=" { EMIT_TOKEN(); return TokenType::BitXorAssign; }
"^" { EMIT_TOKEN(); return TokenType::BitXor; }

"<<=" { EMIT_TOKEN(); return TokenType::BitShlAssign; }
"<<" { EMIT_TOKEN(); return TokenType::BitShl; }
"<=" { EMIT_TOKEN(); return TokenType::Le; }
"<" { EMIT_TOKEN(); return TokenType::Lt; }

">>=" { EMIT_TOKEN(); return TokenType::BitShrAssign; }
">>" { EMIT_TOKEN(); return TokenType::BitShr; }
">=" { EMIT_TOKEN(); return TokenType::Ge; }
">" { EMIT_TOKEN(); return TokenType::Gt; }

"!=" { EMIT_TOKEN(); return TokenType::Ne; }
"!" { EMIT_TOKEN(); return TokenType::LogNot; }

"=>" { EMIT_TOKEN(); return TokenType::FatArrow; }
"==" { EMIT_TOKEN(); return TokenType::Eq; }
"=" { EMIT_TOKEN(); return TokenType::Assign; }

"..<" { EMIT_TOKEN(); return TokenType::RangeExcl; }
"..." { EMIT_TOKEN(); return TokenType::Range; }
"." { EMIT_TOKEN(); return TokenType::Dot; }

"," { EMIT_TOKEN(); return TokenType::Comma; }
":" { EMIT_TOKEN(); return TokenType::Colon; }
";" { EMIT_TOKEN(); return TokenType::Semicolon; }
"(" { EMIT_TOKEN(); return TokenType::LParen; }
")" { EMIT_TOKEN(); return TokenType::RParen; }
"[" { EMIT_TOKEN(); return TokenType::LBracket; }
"]" { EMIT_TOKEN(); return TokenType::RBracket; }
"{" { EMIT_TOKEN(); return TokenType::LBrace; }
"}" { EMIT_TOKEN(); return TokenType::RBrace; }

. {
    raiseError(lstate, "unrecognized character");
    EMIT_TOKEN();
    return TokenType::Error;
}

%%
