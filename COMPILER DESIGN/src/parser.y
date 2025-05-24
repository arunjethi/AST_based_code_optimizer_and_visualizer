%code requires {
    #include "ast.h"
}

%{
#include <stdio.h>
#include "ast.h"

extern int yylex();
void yyerror(const char* s) { fprintf(stderr, "Parse error: %s\n", s); }

ASTNode* ast_root;
%}

%union {
    int ival;
    char* str;
    ASTNode* node;
}

%token <ival> NUMBER
%token <str> IDENTIFIER
%token <str> STRING

%token KW_INT KW_IF KW_FOR KW_RETURN
%token LPAREN RPAREN LBRACE RBRACE SEMICOLON ASSIGN COMMA
%token PLUS MINUS MUL DIV LT
%token INCR DECR

%type <node> stmt stmt_list compound_stmt expr expr_list decl_stmt
               if_stmt for_stmt return_stmt function type program for_init

%left LT
%left PLUS MINUS
%left MUL DIV

%start program

%%

program:
      function                          { ast_root = $1; }
    ;

function:
      type IDENTIFIER LPAREN RPAREN compound_stmt
                                        { $$ = make_function_node($2, $5); }
    ;

type:
      KW_INT                            { $$ = make_type_node("int"); }
    ;

stmt_list:
      stmt                              { $$ = $1; }
    | stmt_list stmt                    { $$ = make_seq_node($1, $2); }
    ;

compound_stmt:
      LBRACE stmt_list RBRACE           { $$ = $2; }
    ;

stmt:
      decl_stmt                         { $$ = $1; }
    | expr SEMICOLON                    { $$ = $1; }
    | if_stmt                           { $$ = $1; }
    | for_stmt                          { $$ = $1; }
    | return_stmt                       { $$ = $1; }
    ;

decl_stmt:
      KW_INT IDENTIFIER ASSIGN expr SEMICOLON
                                        { $$ = make_decl_node($2, $4); }
    | KW_INT IDENTIFIER SEMICOLON       { $$ = make_decl_node($2, NULL); }
    ;

if_stmt:
      KW_IF LPAREN expr RPAREN compound_stmt
                                        { $$ = make_if_node($3, $5); }
    ;

for_init:
      KW_INT IDENTIFIER ASSIGN expr     { $$ = make_decl_node($2, $4); }
    | KW_INT IDENTIFIER                 { $$ = make_decl_node($2, NULL); }
    | expr                              { $$ = $1; }
    | /* empty */                       { $$ = NULL; }
    ;

for_stmt:
      KW_FOR LPAREN for_init SEMICOLON expr SEMICOLON expr RPAREN compound_stmt
                                        { $$ = make_for_node($3, $5, $7, $9); }
    ;

return_stmt:
      KW_RETURN expr SEMICOLON          { $$ = make_return_node($2); }
    ;

expr:
      expr PLUS expr                    { $$ = make_binop_node('+', $1, $3); }
    | expr MINUS expr                   { $$ = make_binop_node('-', $1, $3); }
    | expr MUL expr                     { $$ = make_binop_node('*', $1, $3); }
    | expr DIV expr                     { $$ = make_binop_node('/', $1, $3); }
    | expr LT expr                      { $$ = make_binop_node('<', $1, $3); }
    | IDENTIFIER INCR                   { $$ = make_unary_node("++", make_var_node($1)); }
    | IDENTIFIER DECR                   { $$ = make_unary_node("--", make_var_node($1)); }
    | NUMBER                            { $$ = make_int_node($1); }
    | STRING                            { $$ = make_string_node($1); }
    | IDENTIFIER                        { $$ = make_var_node($1); }
    | IDENTIFIER LPAREN RPAREN          { $$ = make_func_call_node($1, NULL); }
    | IDENTIFIER LPAREN expr_list RPAREN
                                        { $$ = make_func_call_node($1, $3); }
    ;

expr_list:
      expr                              { $$ = make_expr_list_node($1, NULL); }
    | expr_list COMMA expr              { $$ = make_expr_list_node($3, $1); }
    ;
