%{
int yylex(void);
void yyerror(const char* msg);
%}

%require "3.5"

%union {

}

%%
start:
;
%%
