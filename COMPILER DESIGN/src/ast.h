#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef enum {
    NODE_INT,
    NODE_STRING,
    NODE_VAR,
    NODE_DECL,
    NODE_BINOP,
    NODE_UNARY,
    NODE_FUNC_CALL,
    NODE_FUNC_DEF,
    NODE_IF,
    NODE_FOR,
    NODE_RETURN,
    NODE_EXPR_LIST,
    NODE_SEQ,
    NODE_TYPE
} NodeType;


typedef struct ASTNode {
    NodeType type;
    char* value;           
    struct ASTNode* left;  
    struct ASTNode* right; 
    struct ASTNode* next;  
} ASTNode;


ASTNode* make_int_node(int value);


ASTNode* make_string_node(char* value);

ASTNode* make_var_node(char* name);

ASTNode* make_binop_node(char op, ASTNode* left, ASTNode* right);

ASTNode* make_unary_node(char* op, ASTNode* expr);

ASTNode* make_decl_node(char* name, ASTNode* init_expr);

ASTNode* make_func_call_node(char* name, ASTNode* args);

ASTNode* make_function_node(char* name, ASTNode* body);


ASTNode* make_if_node(ASTNode* condition, ASTNode* then_body);


ASTNode* make_for_node(ASTNode* init, ASTNode* condition, ASTNode* update, ASTNode* body);

ASTNode* make_return_node(ASTNode* expr);


ASTNode* make_expr_list_node(ASTNode* expr, ASTNode* next);


ASTNode* make_seq_node(ASTNode* first, ASTNode* second);

ASTNode* make_type_node(char* type_name);

ASTNode* create_node(NodeType type, const char* value);
void add_child(ASTNode* parent, ASTNode* child);
void add_sibling(ASTNode* node, ASTNode* sibling);

void free_ast(ASTNode* node);


void print_ast(ASTNode* node, FILE* output, int indent);

#endif