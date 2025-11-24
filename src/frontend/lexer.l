%option bison-bridge reentrant noyywrap noyylineno nounput noinput
%x IN_STRING IN_BLOCK_COMMENT

%top {
#include <parser.tab.hpp>

#include "frontend/lexer/token_type.hpp"

#define YYSTYPE yy::parser::semantic_type
}

%{
#include <sstream>
#include <string_view>

#include "frontend/lexer.hpp"

using namespace Spark;
using namespace Spark::FrontEnd;

#define YYLENG (static_cast<size_t>(yyget_leng(yyscanner)))
#define YYTEXT (std::string_view(yyget_text(yyscanner), YYLENG))
#define YYLVAL (yyget_lval(yyscanner))

#define LSTATE (*static_cast<LexerState*>(yyget_extra(yyscanner)))
#define LBUFFER (LSTATE.tokenBuffer)

#define YYLINENO (LSTATE.lineno)
#define YYCOLUMNNO (LSTATE.columnno)

#define EMIT_TOKEN(text, line, col) (YYLVAL->emplace<TokenValue>(std::string(text), (line), (col)))

#define RAISE_ERROR(message, start, end) (LSTATE.addError((message), (start), (end)))

#define YY_INPUT(buf, result, max_size)                         \
{                                                               \
    LexerState& ls = LSTATE;                                    \
    ls.streamp->read(reinterpret_cast<char*>(buf), (max_size)); \
    (result) = ls.streamp->gcount();                            \
    if ((result) == 0) {                                        \
        (result) = YY_NULL;                                     \
    }                                                           \
}
%}

%%

(\r\n|\r|\n) { LSTATE.whenNewline(); }

[ \t\r]+ {
    LSTATE.step(YYLENG);
}

[0-9_]*\.[0-9_]+ {
    EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO);
    LSTATE.step(YYLENG);
    return TokenType::Real;
}

[0-9_]+|0[bB][0-9_]+|0[oO][0-9_]+|0[xX][0-9A-Za-z_]+ {
    EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO);
    LSTATE.step(YYLENG);
    return TokenType::Integer;
}

[A-Za-z_][A-Za-z0-9_]* {
    EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO);
    LSTATE.step(YYLENG);
    return classifyWord(YYTEXT);
}

"//"[^\n]* {
    EMIT_TOKEN(YYTEXT.substr(2), YYLINENO, YYCOLUMNNO);
    LSTATE.step(YYLENG);
    return TokenType::LineComment;
}

["'] {
    LBUFFER.reset(YYLINENO, YYCOLUMNNO);
    LSTATE.step(1);
    LSTATE.stringDelimiter = YYTEXT[0];
    BEGIN(IN_STRING);
}

<IN_STRING>{
    "\\n" { LBUFFER.append('\n'); LSTATE.step(2); }
    "\\t" { LBUFFER.append('\t'); LSTATE.step(2); }
    "\\r" { LBUFFER.append('\r'); LSTATE.step(2); }
    "\\\\" { LBUFFER.append('\\'); LSTATE.step(2); }
    "\\\"" { LBUFFER.append('"'); LSTATE.step(2); }
    "\\\'" { LBUFFER.append('\''); LSTATE.step(2); }

    \\. {
        LBUFFER.append(YYTEXT[1]);
        std::ostringstream oss;
        oss << "unrecognized escape sequence: " << "'" << YYTEXT << "'";
        RAISE_ERROR(oss.str(), Location(YYLINENO, YYCOLUMNNO - 1), Location(YYLINENO, YYCOLUMNNO));
        LSTATE.step(2);
    }

    (\r\n|\r|\n) {
        BEGIN(INITIAL);
        RAISE_ERROR("unterminated string literal", Location(LBUFFER.line(), LBUFFER.column()),
            Location(YYLINENO, YYCOLUMNNO - 1));
        LSTATE.whenNewline();
        EMIT_TOKEN(LBUFFER.view(), LBUFFER.line(), LBUFFER.column());
        return TokenType::String;
    }

    <<EOF>> {
        BEGIN(INITIAL);
        RAISE_ERROR("unterminated string literal", Location(LBUFFER.line(), LBUFFER.column()),
            Location(YYLINENO, YYCOLUMNNO - 1));
        EMIT_TOKEN(LBUFFER.view(), LBUFFER.line(), LBUFFER.column());
        return TokenType::String;
    }

    . {
        LSTATE.step(1);
        if (YYTEXT[0] == LSTATE.stringDelimiter) {
            BEGIN(INITIAL);
            EMIT_TOKEN(LBUFFER.view(), LBUFFER.line(), LBUFFER.column());
            return TokenType::String;
        }
        LBUFFER.append(YYTEXT[0]);
    }
}

"/*" {
    BEGIN(IN_BLOCK_COMMENT);
    LBUFFER.reset(YYLINENO, YYCOLUMNNO);
    LSTATE.step(2);
}

<IN_BLOCK_COMMENT>{
    "*/" {
        BEGIN(INITIAL);
        EMIT_TOKEN(LBUFFER.view(), LBUFFER.line(), LBUFFER.column());
        LSTATE.step(2);
        return TokenType::BlockComment;
    }

    [^*\r\n]+ {
        LBUFFER.append(YYTEXT);
        LSTATE.step(YYLENG);
    }

    "*" {
        LBUFFER.append('*');
        LSTATE.step(1);
    }

    (\r\n|\r|\n) {
        LBUFFER.append(YYTEXT);
        LSTATE.whenNewline();
    }

    <<EOF>> {
        BEGIN(INITIAL);
        RAISE_ERROR("unterminated block comment", Location(YYLINENO, YYCOLUMNNO - 1),
            Location(YYLINENO, YYCOLUMNNO - 1));
        EMIT_TOKEN(LBUFFER.view(), LBUFFER.line(), LBUFFER.column());
        return TokenType::BlockComment;
    }
}

"+=" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.step(2); return TokenType::AddAssign; }
"+" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.step(1); return TokenType::Add; }

"->" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.step(2); return TokenType::Arrow; }
"-=" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.step(2); return TokenType::SubAssign; }
"-" {  EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.step(1); return TokenType::Sub; }

"*=" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.step(2); return TokenType::MulAssign; }
"*" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.step(1); return TokenType::Mul; }

"/=" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.step(2); return TokenType::DivAssign; }
"/" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.step(1); return TokenType::Div; }

"%=" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.step(2); return TokenType::ModAssign; }
"%" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.step(1); return TokenType::Mod; }

"~" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.step(1); return TokenType::BitNot; }

"&&" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.step(2); return TokenType::LogAnd; }
"&=" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.step(2); return TokenType::BitAndAssign; }
"&" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.step(1); return TokenType::BitAnd; }

"||" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.step(2); return TokenType::LogOr; }
"|=" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.step(2); return TokenType::BitOrAssign; }
"|" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.step(1); return TokenType::BitOr; }

"^=" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.step(2); return TokenType::BitXorAssign; }
"^" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.step(1); return TokenType::BitXor; }

"<<=" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.step(3); return TokenType::BitShlAssign; }
"<<" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.step(2); return TokenType::BitShl; }
"<=" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.step(2); return TokenType::Le; }
"<" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.step(1); return TokenType::Lt; }

">>=" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.step(3); return TokenType::BitShrAssign; }
">>" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.step(2); return TokenType::BitShr; }
">=" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.step(2); return TokenType::Ge; }
">" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.step(1); return TokenType::Gt; }

"!==" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.step(3); return TokenType::StrictNe; }
"!=" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.step(2); return TokenType::Ne; }
"!" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.step(1); return TokenType::LogNot; }

"===" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.step(3); return TokenType::StrictEq; }
"=>" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.step(2); return TokenType::FatArrow; }
"==" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.step(2); return TokenType::Eq; }
"=" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.step(1); return TokenType::Assign; }

"..<" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.step(3); return TokenType::RangeExcl; }
"..." { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.step(3); return TokenType::Range; }
"." { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.step(1); return TokenType::Dot; }

"??=" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.step(3); return TokenType::CoalesceAssign; }
"??" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.step(2); return TokenType::Coalesce; }
"?" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.step(1); return TokenType::Question; }

"," { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.step(1); return TokenType::Comma; }
":" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.step(1); return TokenType::Colon; }
";" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.step(1); return TokenType::Semicolon; }
"(" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.step(1); return TokenType::LParen; }
")" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.step(1); return TokenType::RParen; }
"[" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.step(1); return TokenType::LBracket; }
"]" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.step(1); return TokenType::RBracket; }
"{" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.step(1); return TokenType::LBrace; }
"}" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.step(1); return TokenType::RBrace; }
"@" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.step(1); return TokenType::At; }
"$" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.step(1); return TokenType::Dollar; }

. {
    std::ostringstream oss;
    oss << "unrecognized character: " << "'" << YYTEXT << "'";
    RAISE_ERROR(oss.str(), Location(YYLINENO, YYCOLUMNNO), Location(YYLINENO, YYCOLUMNNO));
    EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO);
    LSTATE.step(1);
    return TokenType::Error;
}

%%
