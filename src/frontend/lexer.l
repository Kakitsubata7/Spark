%option bison-bridge reentrant noyywrap noyylineno
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

#define get_lstate(scanner) (*static_cast<LexerState*>(yyget_extra(scanner)))
%}

%%

"\r\n" { handleNewline(get_lstate(yyscanner)); }
"\n" { handleNewline(get_lstate(yyscanner)); }
"\r" { handleNewline(get_lstate(yyscanner)); }

[ \t\r]+ {
    consumeCharacters(static_cast<size_t>(yyget_leng(yyscanner)), get_lstate(yyscanner));
}

[0-9_]*\.[0-9_]+ {
    makeToken(std::string_view(yyget_text(yyscanner), yyget_leng(yyscanner)), get_lstate(yyscanner));
    return TokenType::Real;
}

[0-9_]+|0[bB][0-9_]+|0[oO][0-9_]+|0[xX][0-9A-Za-z_]+ {
    makeToken(std::string_view(yyget_text(yyscanner), yyget_leng(yyscanner)), get_lstate(yyscanner));
    return TokenType::Integer;
}

[A-Za-z_][A-Za-z0-9_]* {
    std::string_view word(yyget_text(yyscanner), yyget_leng(yyscanner));
    makeToken(word, get_lstate(yyscanner));
    return classifyWord(word);
}

"//"[^\n]* {
    *yylval = makeToken(std::string_view(yyget_text(yyscanner), yyget_leng(yyscanner)).substr(2), get_lstate(yyscanner));
    return TokenType::LineComment;
}

"/*" {
    LexerState& lstate = get_lstate(yyscanner);
    consumeCharacters(2, lstate);
    lstate.tokenBuffer.clear();
    BEGIN(IN_BLOCK_COMMENT);
}

<IN_BLOCK_COMMENT>{
    "*/" {
        LexerState& lstate = get_lstate(yyscanner);
        consumeCharacters(2, lstate);
        makeToken(lstate.tokenBuffer, lstate);
        lstate.tokenBuffer.clear();
        BEGIN(INITIAL);
        return TokenType::BlockComment;
    }

    [^*\n]+ {
        size_t n = static_cast<size_t>(yyget_leng(yyscanner));
        LexerState& lstate = get_lstate(yyscanner);
        consumeCharacters(n, lstate);
        lstate.tokenBuffer.append(yyget_text(yyscanner), n);
    }

    "*" {
        LexerState& lstate = get_lstate(yyscanner);
        consumeCharacters(1, lstate);
        lstate.tokenBuffer.push_back('*');
    }

    "\n" {
        LexerState& lstate = get_lstate(yyscanner);
        handleNewline(lstate);
        lstate.tokenBuffer.push_back('\n');
    }

    <<EOF>> {
        LexerState& lstate = get_lstate(yyscanner);
        lstate.errors.emplace_back("unterminated block comment", lstate.line, lstate.column);
        return TokenType::BlockComment;
    }
}

"+=" { return TokenType::AddAssign; }
"+" { return TokenType::Add; }

"->" { return TokenType::Arrow; }
"-=" { return TokenType::SubAssign; }
"-" { return TokenType::Sub; }

"*=" { return TokenType::MulAssign; }
"*" { return TokenType::Mul; }

"/=" { return TokenType::DivAssign; }
"/" { return TokenType::Div; }

"%=" { return TokenType::ModAssign; }
"%" { return TokenType::Mod; }

"~" { return TokenType::BitNot; }

"&&" { return TokenType::LogAnd; }
"&=" { return TokenType::BitAndAssign; }
"&" { return TokenType::BitAnd; }

"||" { return TokenType::LogOr; }
"|=" { return TokenType::BitOrAssign; }
"|" { return TokenType::BitOr; }

"^=" { return TokenType::BitXorAssign; }
"^" { return TokenType::BitXor; }

"<<=" { return TokenType::BitShlAssign; }
"<<" { return TokenType::BitShl; }
"<=" { return TokenType::Le; }
"<" { return TokenType::Lt; }

">>=" { return TokenType::BitShrAssign; }
">>" { return TokenType::BitShr; }
">=" { return TokenType::Ge; }
">" { return TokenType::Gt; }

"!=" { return TokenType::Ne; }
"!" { return TokenType::LogNot; }

"=>" { return TokenType::FatArrow; }
"==" { return TokenType::Eq; }
"=" { return TokenType::Assign; }

"..<" { return TokenType::RangeExcl; }
"..." { return TokenType::Range; }
"." { return TokenType::Dot; }

"," { return TokenType::Comma; }
":" { return TokenType::Colon; }
";" { return TokenType::Semicolon; }
"(" { return TokenType::LParen; }
")" { return TokenType::RParen; }
"[" { return TokenType::LBracket; }
"]" { return TokenType::RBracket; }
"{" { return TokenType::LBrace; }
"}" { return TokenType::RBrace; }

. {

}

%%
