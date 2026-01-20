%option bison-bridge reentrant noyywrap noyylineno nounput noinput
%x IN_STRING IN_BLOCK_COMMENT

%top {
#include <parser.tab.hpp>

#include "frontend/lexer/token_type.hpp"

#define YYSTYPE yy::parser::semantic_type

#define YY_DECL \
    int yylex(yy::parser::semantic_type* yylval_param, yy::parser::location_type* yylloc, yyscan_t yyscanner)
YY_DECL;
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

#define YYLINENO (LSTATE.line)
#define YYCOLUMNNO (LSTATE.column)
#define YYLOCATION (Location(YYLINENO, YYCOLUMNNO))

/**
  * Updates yylval to "emit" the token with given lexeme, start and end locations typed `Spark::Location`.
  */
#define EMIT_TOK(text, start, end) (YYLVAL->emplace<TokenValue>(std::string(text), (start), (end)))

/**
  * Helper macro to emit and return a single-line token.
  */
#define RETURN_TOK(type)                                                   \
{                                                                          \
    EMIT_TOK(YYTEXT, YYLOCATION, Location(YYLINENO, YYCOLUMNNO + YYLENG)); \
    YYCOLUMNNO += YYLENG;                                                \
    return (type);                                                         \
}

/**
  * Sets yylloc to a specific start and end typed `Spark::Location`.
  */
#define SET_LOC(s, e)                    \
{                                        \
    yylloc->begin.line = (s).line;     \
    yylloc->begin.column = (s).column; \
    yylloc->end.line = (e).line;       \
    yylloc->end.column = (e).column;   \
}

/**
  * Helper macro to update yylloc for single-line tokens.
  */
#define UPDATE_LOC()                             \
{                                                \
    Location end(YYLINENO, YYCOLUMNNO + YYLENG); \
    SET_LOC(YYLOCATION, end)                     \
}

/**
  * Reports an error to the lexer state.
  */
#define RAISE_ERROR(message, start, end) (LSTATE.addError((message), (start), (end)))

#define YY_INPUT(buf, result, maxSize)     \
{                                          \
    size_t read =                          \
    LSTATE.readChunk(                      \
        reinterpret_cast<char*>(buf),      \
        (maxSize)                          \
    );                                     \
                                           \
    if (read == 0) {                       \
        (result) = YY_NULL;                \
    } else {                               \
        (result) = static_cast<int>(read); \
    }                                      \
}
%}

%%
(\r\n|\r|\n) {
    LSTATE.whenNewline();
}

[ \t\r]+ {
    YYCOLUMNNO += YYLENG;
}

[0-9_]*\.[0-9_]+ {
    UPDATE_LOC();
    RETURN_TOK(TokenType::Real);
}

[0-9][0-9_]* {
    UPDATE_LOC();
    RETURN_TOK(TokenType::Integer);
}

0[bB][01][01_]* {
    UPDATE_LOC();
    RETURN_TOK(TokenType::Integer);
}

0[oO][0-7][0-7_]* {
    UPDATE_LOC();
    RETURN_TOK(TokenType::Integer);
}

0[xX][0-9A-Fa-f][0-9A-Fa-f_]* {
    UPDATE_LOC();
    RETURN_TOK(TokenType::Integer);
}

[A-Za-z_][A-Za-z0-9_]* {
    UPDATE_LOC();
    RETURN_TOK(classifyWord(YYTEXT));
}

"//"[^\n]* {
    Location end(YYLINENO, YYCOLUMNNO + YYLENG);
    SET_LOC(YYLOCATION, end);
    EMIT_TOK(YYTEXT.substr(2), YYLOCATION, end);
    YYCOLUMNNO += YYLENG;
    return TokenType::LineComment;
}

["'] {
    LBUFFER.reset(YYLINENO, YYCOLUMNNO);
    ++YYCOLUMNNO;
    LSTATE.strDelim = YYTEXT[0];
    BEGIN(IN_STRING);
}

<IN_STRING>{
    "\\n" { LBUFFER.append('\n'); YYCOLUMNNO += 2; }
    "\\t" { LBUFFER.append('\t'); YYCOLUMNNO += 2; }
    "\\r" { LBUFFER.append('\r'); YYCOLUMNNO += 2; }
    "\\\\" { LBUFFER.append('\\'); YYCOLUMNNO += 2; }
    "\\\"" { LBUFFER.append('"'); YYCOLUMNNO += 2; }
    "\\\'" { LBUFFER.append('\''); YYCOLUMNNO += 2; }

    \\. {
        LBUFFER.append(YYTEXT[1]);
        std::ostringstream oss;
        oss << "unrecognized escape sequence: " << "'" << YYTEXT << "'";
        RAISE_ERROR(oss.str(), Location(YYLINENO, YYCOLUMNNO - 1), Location(YYLINENO, YYCOLUMNNO));
        YYCOLUMNNO += 2;
    }

    (\r\n|\r|\n) {
        BEGIN(INITIAL);
        Location end(YYLINENO, YYCOLUMNNO - 1);
        RAISE_ERROR("unterminated string literal", LBUFFER.start(), end);
        SET_LOC(LBUFFER.start(), end);
        LSTATE.whenNewline();
        EMIT_TOK(LBUFFER.view(), LBUFFER.start(), end);
        return TokenType::String;
    }

    <<EOF>> {
        BEGIN(INITIAL);
        Location end(YYLINENO, YYCOLUMNNO - 1);
        RAISE_ERROR("unterminated string literal", LBUFFER.start(), end);
        SET_LOC(LBUFFER.start(), end);
        EMIT_TOK(LBUFFER.view(), LBUFFER.start(), end);
        return TokenType::String;
    }

    . {
        ++YYCOLUMNNO;
        if (YYTEXT[0] == LSTATE.strDelim) {
            BEGIN(INITIAL);
            SET_LOC(LBUFFER.start(), YYLOCATION);
            EMIT_TOK(LBUFFER.view(), LBUFFER.start(), YYLOCATION);
            return TokenType::String;
        }
        LBUFFER.append(YYTEXT[0]);
    }
}

"/*" {
    BEGIN(IN_BLOCK_COMMENT);
    LBUFFER.reset(YYLINENO, YYCOLUMNNO);
    YYCOLUMNNO += 2;
}

<IN_BLOCK_COMMENT>{
    "*/" {
        BEGIN(INITIAL);
        SET_LOC(LBUFFER.start(), YYLOCATION);
        EMIT_TOK(LBUFFER.view(), LBUFFER.start(), YYLOCATION);
        YYCOLUMNNO += 2;
        return TokenType::BlockComment;
    }

    [^*\r\n]+ {
        LBUFFER.append(YYTEXT);
        YYCOLUMNNO += YYLENG;
    }

    "*" {
        LBUFFER.append('*');
        ++YYCOLUMNNO;
    }

    (\r\n|\r|\n) {
        LBUFFER.append(YYTEXT);
        LSTATE.whenNewline();
    }

    <<EOF>> {
        BEGIN(INITIAL);
        Location end(YYLINENO, YYCOLUMNNO - 1);
        RAISE_ERROR("unterminated block comment", end, end);
        SET_LOC(LBUFFER.start(), end);
        EMIT_TOK(LBUFFER.view(), LBUFFER.start(), end);
        return TokenType::BlockComment;
    }
}

"+=" { UPDATE_LOC(); RETURN_TOK(TokenType::AddAssign); }
"+" { UPDATE_LOC(); RETURN_TOK(TokenType::Add); }

"->" { UPDATE_LOC(); RETURN_TOK(TokenType::Arrow); }
"-=" { UPDATE_LOC(); RETURN_TOK(TokenType::SubAssign); }
"-" { UPDATE_LOC(); RETURN_TOK(TokenType::Sub); }

"*=" { UPDATE_LOC(); RETURN_TOK(TokenType::MulAssign); }
"*" { UPDATE_LOC(); RETURN_TOK(TokenType::Mul); }

"/=" { UPDATE_LOC(); RETURN_TOK(TokenType::DivAssign); }
"/" { UPDATE_LOC(); RETURN_TOK(TokenType::Div); }

"%=" { UPDATE_LOC(); RETURN_TOK(TokenType::ModAssign); }
"%" { UPDATE_LOC(); RETURN_TOK(TokenType::Mod); }

"~" { UPDATE_LOC(); RETURN_TOK(TokenType::Tide); }

"&&" { UPDATE_LOC(); RETURN_TOK(TokenType::LogAnd); }
"&=" { UPDATE_LOC(); RETURN_TOK(TokenType::BitAndAssign); }
"&" { UPDATE_LOC(); RETURN_TOK(TokenType::And); }

"||" { UPDATE_LOC(); RETURN_TOK(TokenType::LogOr); }
"|=" { UPDATE_LOC(); RETURN_TOK(TokenType::BitOrAssign); }
"|>" { UPDATE_LOC(); RETURN_TOK(TokenType::Pipe); }
"|" { UPDATE_LOC(); RETURN_TOK(TokenType::VBar); }

"^=" { UPDATE_LOC(); RETURN_TOK(TokenType::BitXorAssign); }
"^" { UPDATE_LOC(); RETURN_TOK(TokenType::Caret); }

"<=" { UPDATE_LOC(); RETURN_TOK(TokenType::Le); }
"<" { UPDATE_LOC(); RETURN_TOK(TokenType::Lt); }

">=" { UPDATE_LOC(); RETURN_TOK(TokenType::Ge); }
">" { UPDATE_LOC(); RETURN_TOK(TokenType::Gt); }

"!==" { UPDATE_LOC(); RETURN_TOK(TokenType::StrictNe); }
"!=" { UPDATE_LOC(); RETURN_TOK(TokenType::Ne); }
"!" { UPDATE_LOC(); RETURN_TOK(TokenType::Bang); }

"===" { UPDATE_LOC(); RETURN_TOK(TokenType::StrictEq); }
"=>" { UPDATE_LOC(); RETURN_TOK(TokenType::FatArrow); }
"==" { UPDATE_LOC(); RETURN_TOK(TokenType::Eq); }
"=" { UPDATE_LOC(); RETURN_TOK(TokenType::Assign); }

"..<" { UPDATE_LOC(); RETURN_TOK(TokenType::RangeExcl); }
"..." { UPDATE_LOC(); RETURN_TOK(TokenType::Range); }
"." { UPDATE_LOC(); RETURN_TOK(TokenType::Dot); }

"??=" { UPDATE_LOC(); RETURN_TOK(TokenType::CoalesceAssign); }
"??" { UPDATE_LOC(); RETURN_TOK(TokenType::Coalesce); }
"?!" { UPDATE_LOC(); RETURN_TOK(TokenType::NonNull); }
"?" { UPDATE_LOC(); RETURN_TOK(TokenType::Question); }

"," { UPDATE_LOC(); RETURN_TOK(TokenType::Comma); }
":" { UPDATE_LOC(); RETURN_TOK(TokenType::Colon); }
";" { UPDATE_LOC(); RETURN_TOK(TokenType::Semicolon); }
"(" { UPDATE_LOC(); RETURN_TOK(TokenType::LParen); }
")" { UPDATE_LOC(); RETURN_TOK(TokenType::RParen); }
"[" { UPDATE_LOC(); RETURN_TOK(TokenType::LBracket); }
"]" { UPDATE_LOC(); RETURN_TOK(TokenType::RBracket); }
"{" { UPDATE_LOC(); RETURN_TOK(TokenType::LBrace); }
"}" { UPDATE_LOC(); RETURN_TOK(TokenType::RBrace); }
"@" { UPDATE_LOC(); RETURN_TOK(TokenType::At); }
"$" { UPDATE_LOC(); RETURN_TOK(TokenType::Dollar); }

. {
    std::ostringstream oss;
    oss << "unrecognized character: " << "'" << YYTEXT << "'";
    RAISE_ERROR(oss.str(), YYLOCATION, YYLOCATION);
    EMIT_TOK(YYTEXT, YYLOCATION, YYLOCATION);
    ++YYCOLUMNNO;
    return TokenType::Error;
}

<<EOF>> {
    EMIT_TOK("", YYLOCATION, YYLOCATION);
    return TokenType::EndOfFile;
}
%%
