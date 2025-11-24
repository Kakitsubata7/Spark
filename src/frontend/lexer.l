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
#define LBUFFER (LSTATE.tokbuf())

#define YYLINENO (LSTATE.lineno())
#define YYCOLUMNNO (LSTATE.columnno())
#define YYLOCATION (Location(YYLINENO, YYCOLUMNNO))

/**
  * Updates yylval to "emit" the token with given lexeme, start and end locations.
  */
#define EMIT_TOK(text, start, end) (YYLVAL->emplace<TokenValue>(std::string(text), (start), (end)))

/**
  * Helper macro to emit and return a single-line token.
  */
#define RETURN_TOK(type)                                                                       \
{                                                                                              \
    EMIT_TOK(YYTEXT, YYLOCATION, Location(YYLINENO, YYCOLUMNNO + YYLENG)); \
    LSTATE.advance(YYLENG);                                                                    \
    return (type);                                                                             \
}

/**
  * Reports an error to the lexer state.
  */
#define RAISE_ERROR(message, start, end) (LSTATE.addError((message), (start), (end)))

#define YY_INPUT(buf, result, max_size)                         \
{                                                               \
    LexerState& ls = LSTATE;                                    \
    ls.stream().read(reinterpret_cast<char*>(buf), (max_size)); \
    (result) = ls.stream().gcount();                            \
    if ((result) == 0) {                                        \
        (result) = YY_NULL;                                     \
    } else {                                                    \
        ls.srcbuf().append(std::string_view(                    \
            reinterpret_cast<const char*>(buf),                 \
            static_cast<size_t>(result)                         \
        ));                                                     \
    }                                                           \
}
%}

%%

(\r\n|\r|\n) { LSTATE.whenNewline(); }

[ \t\r]+ {
    LSTATE.advance(YYLENG);
}

[0-9_]*\.[0-9_]+ {
    RETURN_TOK(TokenType::Real);
}

[0-9_]+|0[bB][0-9_]+|0[oO][0-9_]+|0[xX][0-9A-Za-z_]+ {
    RETURN_TOK(TokenType::Integer);
}

[A-Za-z_][A-Za-z0-9_]* {
    RETURN_TOK(classifyWord(YYTEXT));
}

"//"[^\n]* {
    EMIT_TOK(YYTEXT.substr(2), YYLOCATION, Location(YYLINENO, YYCOLUMNNO + YYLENG));
    LSTATE.advance(YYLENG);
    return TokenType::LineComment;
}

["'] {
    LBUFFER.reset(YYLINENO, YYCOLUMNNO);
    LSTATE.advance(1);
    LSTATE.setStrDelim(YYTEXT[0]);
    BEGIN(IN_STRING);
}

<IN_STRING>{
    "\\n" { LBUFFER.append('\n'); LSTATE.advance(2); }
    "\\t" { LBUFFER.append('\t'); LSTATE.advance(2); }
    "\\r" { LBUFFER.append('\r'); LSTATE.advance(2); }
    "\\\\" { LBUFFER.append('\\'); LSTATE.advance(2); }
    "\\\"" { LBUFFER.append('"'); LSTATE.advance(2); }
    "\\\'" { LBUFFER.append('\''); LSTATE.advance(2); }

    \\. {
        LBUFFER.append(YYTEXT[1]);
        std::ostringstream oss;
        oss << "unrecognized escape sequence: " << "'" << YYTEXT << "'";
        RAISE_ERROR(oss.str(), Location(YYLINENO, YYCOLUMNNO - 1), Location(YYLINENO, YYCOLUMNNO));
        LSTATE.advance(2);
    }

    (\r\n|\r|\n) {
        BEGIN(INITIAL);
        Location end(YYLINENO, YYCOLUMNNO - 1);
        RAISE_ERROR("unterminated string literal", LBUFFER.start(), end);
        LSTATE.whenNewline();
        EMIT_TOK(LBUFFER.view(), LBUFFER.start(), end);
        return TokenType::String;
    }

    <<EOF>> {
        BEGIN(INITIAL);
        Location end(YYLINENO, YYCOLUMNNO - 1);
        RAISE_ERROR("unterminated string literal", LBUFFER.start(), end);
        EMIT_TOK(LBUFFER.view(), LBUFFER.start(), end);
        return TokenType::String;
    }

    . {
        LSTATE.advance(1);
        if (YYTEXT[0] == LSTATE.strDelim()) {
            BEGIN(INITIAL);
            EMIT_TOK(LBUFFER.view(), LBUFFER.start(), YYLOCATION);
            return TokenType::String;
        }
        LBUFFER.append(YYTEXT[0]);
    }
}

"/*" {
    BEGIN(IN_BLOCK_COMMENT);
    LBUFFER.reset(YYLINENO, YYCOLUMNNO);
    LSTATE.advance(2);
}

<IN_BLOCK_COMMENT>{
    "*/" {
        BEGIN(INITIAL);
        EMIT_TOK(LBUFFER.view(), LBUFFER.start(), YYLOCATION);
        LSTATE.advance(2);
        return TokenType::BlockComment;
    }

    [^*\r\n]+ {
        LBUFFER.append(YYTEXT);
        LSTATE.advance(YYLENG);
    }

    "*" {
        LBUFFER.append('*');
        LSTATE.advance(1);
    }

    (\r\n|\r|\n) {
        LBUFFER.append(YYTEXT);
        LSTATE.whenNewline();
    }

    <<EOF>> {
        BEGIN(INITIAL);
        Location end(YYLINENO, YYCOLUMNNO - 1);
        RAISE_ERROR("unterminated block comment", end, end);
        EMIT_TOK(LBUFFER.view(), LBUFFER.start(), end);
        return TokenType::BlockComment;
    }
}

"+=" { RETURN_TOK(TokenType::AddAssign); }
"+" { RETURN_TOK(TokenType::Add); }

"->" { RETURN_TOK(TokenType::Arrow); }
"-=" { RETURN_TOK(TokenType::SubAssign); }
"-" { RETURN_TOK(TokenType::Sub); }

"*=" { RETURN_TOK(TokenType::MulAssign); }
"*" { RETURN_TOK(TokenType::Mul); }

"/=" { RETURN_TOK(TokenType::DivAssign); }
"/" { RETURN_TOK(TokenType::Div); }

"%=" { RETURN_TOK(TokenType::ModAssign); }
"%" { RETURN_TOK(TokenType::Mod); }

"~" { RETURN_TOK(TokenType::BitNot); }

"&&" { RETURN_TOK(TokenType::LogAnd); }
"&=" { RETURN_TOK(TokenType::BitAndAssign); }
"&" { RETURN_TOK(TokenType::BitAnd); }

"||" { RETURN_TOK(TokenType::LogOr); }
"|=" { RETURN_TOK(TokenType::BitOrAssign); }
"|" { RETURN_TOK(TokenType::BitOr); }

"^=" { RETURN_TOK(TokenType::BitXorAssign); }
"^" { RETURN_TOK(TokenType::BitXor); }

"<<=" { RETURN_TOK(TokenType::BitShlAssign); }
"<<" { RETURN_TOK(TokenType::BitShl); }
"<=" { RETURN_TOK(TokenType::Le); }
"<" { RETURN_TOK(TokenType::Lt); }

">>=" { RETURN_TOK(TokenType::BitShrAssign); }
">>" { RETURN_TOK(TokenType::BitShr); }
">=" { RETURN_TOK(TokenType::Ge); }
">" { RETURN_TOK(TokenType::Gt); }

"!==" { RETURN_TOK(TokenType::StrictNe); }
"!=" { RETURN_TOK(TokenType::Ne); }
"!" { RETURN_TOK(TokenType::LogNot); }

"===" { RETURN_TOK(TokenType::StrictEq); }
"=>" { RETURN_TOK(TokenType::FatArrow); }
"==" { RETURN_TOK(TokenType::Eq); }
"=" { RETURN_TOK(TokenType::Assign); }

"..<" { RETURN_TOK(TokenType::RangeExcl); }
"..." { RETURN_TOK(TokenType::Range); }
"." { RETURN_TOK(TokenType::Dot); }

"??=" { RETURN_TOK(TokenType::CoalesceAssign); }
"??" { RETURN_TOK(TokenType::Coalesce); }
"?" { RETURN_TOK(TokenType::Question); }

"," { RETURN_TOK(TokenType::Comma); }
":" { RETURN_TOK(TokenType::Colon); }
";" { RETURN_TOK(TokenType::Semicolon); }
"(" { RETURN_TOK(TokenType::LParen); }
")" { RETURN_TOK(TokenType::RParen); }
"[" { RETURN_TOK(TokenType::LBracket); }
"]" { RETURN_TOK(TokenType::RBracket); }
"{" { RETURN_TOK(TokenType::LBrace); }
"}" { RETURN_TOK(TokenType::RBrace); }
"@" { RETURN_TOK(TokenType::At); }
"$" { RETURN_TOK(TokenType::Dollar); }

. {
    std::ostringstream oss;
    oss << "unrecognized character: " << "'" << YYTEXT << "'";
    RAISE_ERROR(oss.str(), YYLOCATION, YYLOCATION);
    EMIT_TOK(YYTEXT, YYLOCATION, YYLOCATION);
    LSTATE.advance(1);
    return TokenType::Error;
}

%%
