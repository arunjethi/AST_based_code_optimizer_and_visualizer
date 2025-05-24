#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"


ASTNode* create_node(NodeType type, const char* value) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (!node) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    
    node->type = type;
    node->value = value ? strdup(value) : NULL;
    node->left = NULL;
    node->right = NULL;
    node->next = NULL;
    
    return node;
}


void add_child(ASTNode* parent, ASTNode* child) {
    if (!parent || !child) return;
    
    if (!parent->left) {
        parent->left = child;
    } else if (!parent->right) {
        parent->right = child;
    } else {
       
        add_sibling(parent->right, child);
    }
}


void add_sibling(ASTNode* node, ASTNode* sibling) {
    if (!node || !sibling) return;
    
    ASTNode* current = node;
    while (current->next) {
        current = current->next;
    }
    current->next = sibling;
}


static char* int_to_str(int value) {
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%d", value);
    return strdup(buffer);
}


static char* char_to_str(char c) {
    char buffer[2] = {c, '\0'};
    return strdup(buffer);
}


ASTNode* make_int_node(int value) {
    char* val_str = int_to_str(value);
    ASTNode* node = create_node(NODE_INT, val_str);
    free(val_str);
    return node;
}


ASTNode* make_string_node(char* value) {
    return create_node(NODE_STRING, value);
}


ASTNode* make_var_node(char* name) {
    return create_node(NODE_VAR, name);
}


ASTNode* make_binop_node(char op, ASTNode* left, ASTNode* right) {
    char* op_str = char_to_str(op);
    ASTNode* node = create_node(NODE_BINOP, op_str);
    free(op_str);
    
    node->left = left;
    node->right = right;
    
    return node;
}


ASTNode* make_unary_node(char* op, ASTNode* expr) {
    ASTNode* node = create_node(NODE_UNARY, op);
    node->left = expr;
    return node;
}


ASTNode* make_decl_node(char* name, ASTNode* init_expr) {
    ASTNode* node = create_node(NODE_DECL, name);
    node->left = init_expr;  
    return node;
}


ASTNode* make_func_call_node(char* name, ASTNode* args) {
    ASTNode* node = create_node(NODE_FUNC_CALL, name);
    node->left = args;  
    return node;
}


ASTNode* make_function_node(char* name, ASTNode* body) {
    ASTNode* node = create_node(NODE_FUNC_DEF, name);
    node->left = body; 
    return node;
}


ASTNode* make_if_node(ASTNode* condition, ASTNode* then_body) {
    ASTNode* node = create_node(NODE_IF, NULL);
    node->left = condition;   
    node->right = then_body;  
    return node;
}


ASTNode* make_for_node(ASTNode* init, ASTNode* condition, ASTNode* update, ASTNode* body) {
    ASTNode* node = create_node(NODE_FOR, NULL);
    
   
    node->left = init;
    
    node->right = condition;
    
    if (condition) {
        add_sibling(condition, update);
        if (update) {
            add_sibling(update, body);
        }
    }
    
    return node;
}

ASTNode* make_return_node(ASTNode* expr) {
    ASTNode* node = create_node(NODE_RETURN, NULL);
    node->left = expr;  
    return node;
}


ASTNode* make_expr_list_node(ASTNode* expr, ASTNode* next) {
    ASTNode* node = create_node(NODE_EXPR_LIST, NULL);
    node->left = expr;  
    node->next = next; 
    return node;
}


ASTNode* make_seq_node(ASTNode* first, ASTNode* second) {
    ASTNode* node = create_node(NODE_SEQ, NULL);
    node->left = first;   
    node->right = second; 
    return node;
}


ASTNode* make_type_node(char* type_name) {
    return create_node(NODE_TYPE, type_name);
}


const char* get_node_type_str(NodeType type) {
    switch (type) {
        case NODE_INT: return "INT";
        case NODE_STRING: return "STRING";
        case NODE_VAR: return "VAR";
        case NODE_DECL: return "DECLARATION";
        case NODE_BINOP: return "BINARY_EXPR";
        case NODE_UNARY: return "UNARY_EXPR";
        case NODE_FUNC_CALL: return "FUNCTION_CALL";
        case NODE_FUNC_DEF: return "FUNCTION_DEF";
        case NODE_IF: return "IF_STMT";
        case NODE_FOR: return "FOR_STMT";
        case NODE_RETURN: return "RETURN_STMT";
        case NODE_EXPR_LIST: return "EXPR_LIST";
        case NODE_SEQ: return "SEQUENCE";
        case NODE_TYPE: return "TYPE";
        default: return "UNKNOWN";
    }
}


void print_ast(ASTNode* node, FILE* output, int indent) {
    if (!node) return;
    

    for (int i = 0; i < indent; i++) {
        fprintf(output, "  ");
    }
    

    fprintf(output, "%s", get_node_type_str(node->type));
    if (node->value) {
        fprintf(output, " (%s)", node->value);
    }
    fprintf(output, "\n");

    if (node->left) {
        print_ast(node->left, output, indent + 1);
    }
    
    if (node->right) {
        print_ast(node->right, output, indent + 1);
    }
    
 
    if (node->next) {
        print_ast(node->next, output, indent);
    }
}


void free_ast(ASTNode* node) {
    if (!node) return;
    
    if (node->value) {
        free(node->value);
    }
    
    if (node->left) {
        free_ast(node->left);
    }
    
    if (node->right) {
        free_ast(node->right);
    }
    
    if (node->next) {
        free_ast(node->next);
    }
    
    free(node);
}