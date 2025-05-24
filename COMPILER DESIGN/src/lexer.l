%{
#include "parser.tab.h"
#include <string.h>
#include <stdlib.h>
%}

IDENTIFIER [a-zA-Z_][a-zA-Z0-9_]*
NUMBER     [0-9]+
STRING     \"[^\"]*\"

%%


"int"       { return KW_INT; }
"if"        { return KW_IF; }
"for"       { return KW_FOR; }
"return"    { return KW_RETURN; }


"="         { return ASSIGN; }
";"         { return SEMICOLON; }
","         { return COMMA; }
"("         { return LPAREN; }
")"         { return RPAREN; }
"{"         { return LBRACE; }
"}"         { return RBRACE; }
"+"         { return PLUS; }
"-"         { return MINUS; }
"*"         { return MUL; }
"/"         { return DIV; }
"<"         { return LT; }
"++"        { return INCR; }
"--"        { return DECR; }


{IDENTIFIER} { yylval.str = strdup(yytext); return IDENTIFIER; }
{NUMBER}     { yylval.ival = atoi(yytext); return NUMBER; }


[ \t\r\n]+   {  }


{STRING}     { yylval.str = strdup(yytext); return STRING; }


.            { return yytext[0]; }

%%

int yywrap() {
    return 1;
}
