%option bison-bridge reentrant noyywrap noyylineno nounput noinput
%x IN_STRING IN_BLOCK_COMMENT

%top {
#include <parser.tab.hpp>

#include "frontend/token_type.hpp"
#include "frontend/token_value.hpp"

#define YYSTYPE Spark::FrontEnd::TokenValue
}

%{
#include <sstream>
#include <string_view>

#include "frontend/lexer_state.hpp"
#include "frontend/lexer_utils.hpp"

using namespace Spark::FrontEnd;

#define YYLENG (static_cast<size_t>(yyget_leng(yyscanner)))
#define YYTEXT (std::string_view(yyget_text(yyscanner), YYLENG))
#define YYLVAL (yyget_lval(yyscanner))

#define LSTATE (*static_cast<LexerState*>(yyget_extra(yyscanner)))
#define LBUFFER (LSTATE.tokenBuffer)

#define YYLINENO (LSTATE.line)
#define YYCOLUMNNO (LSTATE.column)

#define EMIT_TOKEN(text, line, column) (*YYLVAL = TokenValue((std::string(text)), (line), (column)))
#define RAISE_ERROR(message, line, column) (LSTATE.addError((message), (line), (column)))

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

"\r\n" { LSTATE.whenNewline(); }
"\n" { LSTATE.whenNewline(); }
"\r" { LSTATE.whenNewline(); }

[ \t\r]+ {
    LSTATE.consume(YYLENG);
}

[0-9_]*\.[0-9_]+ {
    EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO);
    LSTATE.consume(YYLENG);
    return TokenType::Real;
}

[0-9_]+|0[bB][0-9_]+|0[oO][0-9_]+|0[xX][0-9A-Za-z_]+ {
    EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO);
    LSTATE.consume(YYLENG);
    return TokenType::Integer;
}

[A-Za-z_][A-Za-z0-9_]* {
    EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO);
    LSTATE.consume(YYLENG);
    return classifyWord(YYTEXT);
}

"//"[^\n]* {
    EMIT_TOKEN(YYTEXT.substr(2), YYLINENO, YYCOLUMNNO);
    LSTATE.consume(YYLENG);
    return TokenType::LineComment;
}

["'] {
    LBUFFER.reset(YYLINENO, YYCOLUMNNO);
    LSTATE.consume(1);
    LSTATE.stringDelimiter = YYTEXT[0];
    BEGIN(IN_STRING);
}

<IN_STRING>{
    "\\n" { LBUFFER.append('\n'); LSTATE.consume(2); }
    "\\t" { LBUFFER.append('\t'); LSTATE.consume(2); }
    "\\r" { LBUFFER.append('\r'); LSTATE.consume(2); }
    "\\\\" { LBUFFER.append('\\'); LSTATE.consume(2); }
    "\\\"" { LBUFFER.append('"'); LSTATE.consume(2); }
    "\\\'" { LBUFFER.append('\''); LSTATE.consume(2); }

    "\\." {
        LBUFFER.append(YYTEXT[1]);
        std::ostringstream oss;
        oss << "unrecognized escape sequence: " << "'" << YYTEXT << "'";
        RAISE_ERROR(oss.str(), YYLINENO, YYCOLUMNNO);
        LSTATE.consume(2);
    }

    "\n" {
        BEGIN(INITIAL);
        RAISE_ERROR("unterminated string literal", YYLINENO, YYCOLUMNNO);
        LSTATE.whenNewline();
        return TokenType::Error;
    }

    <<EOF>> {
        RAISE_ERROR("unterminated string literal at EOF", YYLINENO, YYCOLUMNNO);
        return TokenType::Error;
    }

    . {
        LSTATE.consume(1);
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
    LSTATE.consume(2);
}

<IN_BLOCK_COMMENT>{
    "*/" {
        BEGIN(INITIAL);
        EMIT_TOKEN(LBUFFER.view(), LBUFFER.line(), LBUFFER.column());
        LSTATE.consume(2);
        return TokenType::BlockComment;
    }

    [^*\n]+ {
        LBUFFER.append(YYTEXT);
        LSTATE.consume(YYLENG);
    }

    "*" {
        LBUFFER.append('*');
        LSTATE.consume(1);
    }

    "\n" {
        LBUFFER.append('\n');
        LSTATE.whenNewline();
    }

    <<EOF>> {
        RAISE_ERROR("unterminated block comment", YYLINENO, YYCOLUMNNO);
        return TokenType::BlockComment;
    }
}

"+=" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.consume(2); return TokenType::AddAssign; }
"+" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.consume(1); return TokenType::Add; }

"->" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.consume(2); return TokenType::Arrow; }
"-=" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.consume(2); return TokenType::SubAssign; }
"-" {  EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.consume(1); return TokenType::Sub; }

"*=" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.consume(2); return TokenType::MulAssign; }
"*" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.consume(1); return TokenType::Mul; }

"/=" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.consume(2); return TokenType::DivAssign; }
"/" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.consume(1); return TokenType::Div; }

"%=" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.consume(2); return TokenType::ModAssign; }
"%" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.consume(1); return TokenType::Mod; }

"~" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.consume(1); return TokenType::BitNot; }

"&&" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.consume(2); return TokenType::LogAnd; }
"&=" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.consume(2); return TokenType::BitAndAssign; }
"&" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.consume(1); return TokenType::BitAnd; }

"||" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.consume(2); return TokenType::LogOr; }
"|=" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.consume(2); return TokenType::BitOrAssign; }
"|" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.consume(1); return TokenType::BitOr; }

"^=" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.consume(2); return TokenType::BitXorAssign; }
"^" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.consume(1); return TokenType::BitXor; }

"<<=" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.consume(3); return TokenType::BitShlAssign; }
"<<" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.consume(2); return TokenType::BitShl; }
"<=" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.consume(2); return TokenType::Le; }
"<" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.consume(1); return TokenType::Lt; }

">>=" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.consume(3); return TokenType::BitShrAssign; }
">>" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.consume(2); return TokenType::BitShr; }
">=" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.consume(2); return TokenType::Ge; }
">" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.consume(1); return TokenType::Gt; }

"!=" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.consume(2); return TokenType::Ne; }
"!" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.consume(1); return TokenType::LogNot; }

"=>" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.consume(2); return TokenType::FatArrow; }
"==" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.consume(2); return TokenType::Eq; }
"=" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.consume(1); return TokenType::Assign; }

"..<" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.consume(3); return TokenType::RangeExcl; }
"..." { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.consume(3); return TokenType::Range; }
"." { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.consume(1); return TokenType::Dot; }

"," { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.consume(1); return TokenType::Comma; }
":" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.consume(1); return TokenType::Colon; }
";" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.consume(1); return TokenType::Semicolon; }
"(" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.consume(1); return TokenType::LParen; }
")" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.consume(1); return TokenType::RParen; }
"[" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.consume(1); return TokenType::LBracket; }
"]" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.consume(1); return TokenType::RBracket; }
"{" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.consume(1); return TokenType::LBrace; }
"}" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.consume(1); return TokenType::RBrace; }
"@" { EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO); LSTATE.consume(1); return TokenType::At; }

. {
    std::ostringstream oss;
    oss << "unrecognized character: " << "'" << YYTEXT << "'";
    RAISE_ERROR(oss.str(), YYLINENO, YYCOLUMNNO);
    EMIT_TOKEN(YYTEXT, YYLINENO, YYCOLUMNNO);
    LSTATE.consume(1);
    return TokenType::Error;
}

%%
