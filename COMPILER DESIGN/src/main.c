#include <stdio.h>
#include "ast.h"

extern int yyparse();
extern FILE* yyin;              
extern ASTNode* ast_root;

int main() {
  
    yyin = fopen("input.c", "r");   
    if (!yyin) {
        perror("input.c");
        return 1; 
    }

   
    FILE* out = fopen("output.txt", "w");
    if (!out) {
        perror("output.txt");
        return 1;
    }

    yyparse();

    print_ast(ast_root, out, 0);

    fclose(yyin);
    fclose(out);

    printf("AST saved to output.txt\n");
    return 0;
}
