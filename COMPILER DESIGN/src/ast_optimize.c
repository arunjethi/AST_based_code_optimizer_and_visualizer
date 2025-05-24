#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_CHILDREN 10
#define MAX_LINE_LEN 256

typedef enum {
    NODE_FUNCTION_DEF,
    NODE_SEQUENCE,
    NODE_DECLARATION,
    NODE_INT,
    NODE_BINARY_EXPR,
    NODE_VAR,
    NODE_IF_STMT,
    NODE_FUNCTION_CALL,
    NODE_EXPR_LIST,
    NODE_FOR_STMT,
    NODE_UNARY_EXPR,
    NODE_RETURN_STMT,
    NODE_STRING,
    NODE_UNKNOWN
} NodeType;

typedef struct ASTNode {
    NodeType type;
    // For named nodes: FUNCTION_DEF(main), DECLARATION(a), VAR(a), FUNCTION_CALL(printf), etc.
    char *name;
    // For INT nodes, store integer value
    int int_value;
    // For STRING nodes, store string value
    char *string_value;
    // For operators in binary/unary expressions
    char op[4];
    // Children nodes
    struct ASTNode *children[MAX_CHILDREN];
    int child_count;
} ASTNode;

/* Forward declarations */
ASTNode *parse_ast(FILE *f);
void free_ast(ASTNode *node);
void optimize_ast(ASTNode *node);
void print_ast_to_file(ASTNode *node, int indent, FILE *out);
ASTNode *clone_ast(ASTNode *node);

/* Helper function to skip spaces */
void skip_spaces(const char **str) {
    while (**str == ' ' || **str == '\t') (*str)++;
}

/* Helper to count the leading spaces */
int count_leading_spaces(const char *line) {
    int count = 0;
    while (*line == ' ') {
        count++;
        line++;
    }
    return count;
}

/* Convert string to NodeType */
NodeType node_type_from_string(const char *str) {
    if (strcmp(str, "FUNCTION_DEF") == 0) return NODE_FUNCTION_DEF;
    if (strcmp(str, "SEQUENCE") == 0) return NODE_SEQUENCE;
    if (strcmp(str, "DECLARATION") == 0) return NODE_DECLARATION;
    if (strcmp(str, "INT") == 0) return NODE_INT;
    if (strcmp(str, "BINARY_EXPR") == 0) return NODE_BINARY_EXPR;
    if (strcmp(str, "VAR") == 0) return NODE_VAR;
    if (strcmp(str, "IF_STMT") == 0) return NODE_IF_STMT;
    if (strcmp(str, "FUNCTION_CALL") == 0) return NODE_FUNCTION_CALL;
    if (strcmp(str, "EXPR_LIST") == 0) return NODE_EXPR_LIST;
    if (strcmp(str, "FOR_STMT") == 0) return NODE_FOR_STMT;
    if (strcmp(str, "UNARY_EXPR") == 0) return NODE_UNARY_EXPR;
    if (strcmp(str, "RETURN_STMT") == 0) return NODE_RETURN_STMT;
    if (strcmp(str, "STRING") == 0) return NODE_STRING;
    return NODE_UNKNOWN;
}

/* Parse a line in the AST text and (optionally) extract an argument */
NodeType parse_line(const char *line, char **arg) {
    *arg = NULL;
    const char *p = line;

    char type_buf[64];
    int i = 0;
    while (*p && *p != ' ' && *p != '(' && *p != '\n' && i < 63) {
        type_buf[i++] = *p++;
    }
    type_buf[i] = 0;
    NodeType t = node_type_from_string(type_buf);
    if (t == NODE_UNKNOWN) return NODE_UNKNOWN;
    
    skip_spaces(&p);
    if (*p == '(') {
        p++;
        const char *start = p;
        while (*p && *p != ')') p++;
        if (*p != ')') return NODE_UNKNOWN;
        int len = (int)(p - start);
        *arg = malloc(len + 1);
        strncpy(*arg, start, len);
        (*arg)[len] = 0;
    }
    return t;
}

/* Recursively parse the AST from a file */
ASTNode *parse_ast_recursive(FILE *f, int current_indent) {
    char line[MAX_LINE_LEN];
    long last_pos = ftell(f);
    if (!fgets(line, MAX_LINE_LEN, f)) return NULL;

    int indent = count_leading_spaces(line);
    if (indent < current_indent) {
        fseek(f, last_pos, SEEK_SET);
        return NULL;
    }
    if (indent > current_indent) {
        fprintf(stderr, "Unexpected indentation\n");
        return NULL;
    }
    
    char *arg = NULL;
    char *trim_line = line + indent;
    NodeType t = parse_line(trim_line, &arg);
    if (t == NODE_UNKNOWN) {
        fprintf(stderr, "Unknown node type in line: %s\n", trim_line);
        if (arg) free(arg);
        return NULL;
    }
    
    ASTNode *node = calloc(1, sizeof(ASTNode));
    node->type = t;
    node->child_count = 0;
    
    if (arg) {
        switch (t) {
            case NODE_FUNCTION_DEF:
            case NODE_DECLARATION:
            case NODE_VAR:
            case NODE_FUNCTION_CALL:
                node->name = arg;
                break;
            case NODE_BINARY_EXPR:
            case NODE_UNARY_EXPR:
                strncpy(node->op, arg, 3);
                node->op[3] = 0;
                free(arg);
                break;
            case NODE_INT:
                node->int_value = atoi(arg);
                free(arg);
                break;
            case NODE_STRING:
                node->string_value = arg;
                break;
            default:
                free(arg);
                break;
        }
    }
    
    while (1) {
        long pos_before = ftell(f);
        ASTNode *child = parse_ast_recursive(f, current_indent + 2);
        if (!child) {
            fseek(f, pos_before, SEEK_SET);
            break;
        }
        if (node->child_count < MAX_CHILDREN) {
            node->children[node->child_count++] = child;
        } else {
            fprintf(stderr, "Too many children for node\n");
            free_ast(child);
            break;
        }
    }
    
    return node;
}

/* Wrapper to parse AST from file */
ASTNode *parse_ast(FILE *f) {
    return parse_ast_recursive(f, 0);
}

/* Free the AST recursively */
void free_ast(ASTNode *node) {
    if (!node) return;
    if (node->name) free(node->name);
    if (node->string_value) free(node->string_value);
    for (int i = 0; i < node->child_count; i++) {
        free_ast(node->children[i]);
    }
    free(node);
}

/* Deep clone an AST node */
ASTNode *clone_ast(ASTNode *node) {
    if (!node) return NULL;
    ASTNode *copy = calloc(1, sizeof(ASTNode));
    copy->type = node->type;
    copy->int_value = node->int_value;
    if (node->name) copy->name = strdup(node->name);
    if (node->string_value) copy->string_value = strdup(node->string_value);
    strncpy(copy->op, node->op, sizeof(copy->op));
    copy->child_count = node->child_count;
    for (int i = 0; i < node->child_count; i++) {
        copy->children[i] = clone_ast(node->children[i]);
    }
    return copy;
}

/* Optimize the AST with constant folding, dead code elimination, and loop unrolling */
void optimize_ast(ASTNode *node) {
    if (!node) return;

    /* Recursively optimize children first */
    for (int i = 0; i < node->child_count; i++) {
        optimize_ast(node->children[i]);
    }

    /* Constant folding for binary expressions */
    if (node->type == NODE_BINARY_EXPR && node->child_count == 2) {
        ASTNode *left = node->children[0];
        ASTNode *right = node->children[1];
        if (left->type == NODE_INT && right->type == NODE_INT) {
            int res = 0, valid = 1;
            if (strcmp(node->op, "+") == 0)
                res = left->int_value + right->int_value;
            else if (strcmp(node->op, "-") == 0)
                res = left->int_value - right->int_value;
            else if (strcmp(node->op, "*") == 0)
                res = left->int_value * right->int_value;
            else if (strcmp(node->op, "/") == 0 && right->int_value != 0)
                res = left->int_value / right->int_value;
            else
                valid = 0;
            if (valid) {
                free_ast(left);
                free_ast(right);
                node->type = NODE_INT;
                node->int_value = res;
                node->child_count = 0;
                node->op[0] = 0;
            }
        }
    }

    /* Constant folding for unary expressions */
    if (node->type == NODE_UNARY_EXPR && node->child_count == 1) {
        ASTNode *child = node->children[0];
        if (child->type == NODE_INT) {
            int res = child->int_value;
            if (strcmp(node->op, "++") == 0) res++;
            else if (strcmp(node->op, "--") == 0) res--;
            else return;
            free_ast(child);
            node->type = NODE_INT;
            node->int_value = res;
            node->child_count = 0;
            node->op[0] = 0;
        }
    }

    /* Dead code elimination for IF_STMT with constant condition */
    if (node->type == NODE_IF_STMT && node->child_count >= 2) {
        ASTNode *cond = node->children[0];
        if (cond->type == NODE_INT) {
            if (cond->int_value == 0) {
                for (int i = 0; i < node->child_count; i++) {
                    free_ast(node->children[i]);
                }
                node->type = NODE_SEQUENCE;
                node->child_count = 0;
            } else {
                ASTNode *then_branch = node->children[1];
                free_ast(cond);
                for (int i = 2; i < node->child_count; i++) {
                    free_ast(node->children[i]);
                }
                /* Instead of a shallow copy (which can lead to double frees),
                   we deeply clone then_branch and replace node's data */
                ASTNode *cloned = clone_ast(then_branch);
                /* Free current node contents (except the node pointer itself) */
                for (int i = 0; i < node->child_count; i++) {
                    node->children[i] = NULL;
                }
                *node = *cloned;
                free(cloned);
            }
        }
    }

    /* Loop Unrolling for simple for-loops */
    if (node->type == NODE_FOR_STMT && node->child_count == 4) {
        ASTNode *init = node->children[0];
        ASTNode *cond = node->children[1];
        ASTNode *update = node->children[2];
        ASTNode *body = node->children[3];

        if (init->type == NODE_DECLARATION && init->child_count == 1 &&
            init->children[0]->type == NODE_INT &&
            cond->type == NODE_BINARY_EXPR && strcmp(cond->op, "<") == 0 &&
            cond->child_count == 2 &&
            cond->children[0]->type == NODE_VAR &&
            cond->children[1]->type == NODE_INT &&
            update->type == NODE_UNARY_EXPR && strcmp(update->op, "++") == 0 &&
            update->child_count == 1 &&
            update->children[0]->type == NODE_VAR &&
            body->type == NODE_FUNCTION_CALL) {

            int start = init->children[0]->int_value;
            int end = cond->children[1]->int_value;
            const char *var = cond->children[0]->name;
            if (strcmp(var, init->name) == 0 && strcmp(var, update->children[0]->name) == 0 &&
                end - start <= 16) {
                /* Save a clone of the loop body before freeing children */
                ASTNode *saved_body = clone_ast(body);
                for (int i = 0; i < node->child_count; i++) {
                    free_ast(node->children[i]);
                }
                node->type = NODE_SEQUENCE;
                node->child_count = 0;
                for (int i = start; i < end; i++) {
                    ASTNode *replica = clone_ast(saved_body);
                    node->children[node->child_count++] = replica;
                }
                free_ast(saved_body);
            }
        }
    }
}

/* Print indentation */
void print_indent_to_file(int indent, FILE *out) {
    for (int i = 0; i < indent; i++)
        fputc(' ', out);
}

/* Recursively print the AST to a file */
void print_ast_to_file(ASTNode *node, int indent, FILE *out) {
    if (!node) return;
    print_indent_to_file(indent, out);
    switch (node->type) {
        case NODE_FUNCTION_DEF: fprintf(out, "FUNCTION_DEF (%s)\n", node->name ? node->name : ""); break;
        case NODE_SEQUENCE: fprintf(out, "SEQUENCE\n"); break;
        case NODE_DECLARATION: fprintf(out, "DECLARATION (%s)\n", node->name ? node->name : ""); break;
        case NODE_INT: fprintf(out, "INT (%d)\n", node->int_value); break;
        case NODE_BINARY_EXPR: fprintf(out, "BINARY_EXPR (%s)\n", node->op); break;
        case NODE_VAR: fprintf(out, "VAR (%s)\n", node->name ? node->name : ""); break;
        case NODE_IF_STMT: fprintf(out, "IF_STMT\n"); break;
        case NODE_FUNCTION_CALL: fprintf(out, "FUNCTION_CALL (%s)\n", node->name ? node->name : ""); break;
        case NODE_EXPR_LIST: fprintf(out, "EXPR_LIST\n"); break;
        case NODE_FOR_STMT: fprintf(out, "FOR_STMT\n"); break;
        case NODE_UNARY_EXPR: fprintf(out, "UNARY_EXPR (%s)\n", node->op); break;
        case NODE_RETURN_STMT: fprintf(out, "RETURN_STMT\n"); break;
        case NODE_STRING: fprintf(out, "STRING (\"%s\")\n", node->string_value ? node->string_value : ""); break;
        default: fprintf(out, "UNKNOWN\n"); break;
    }
    for (int i = 0; i < node->child_count; i++) {
        print_ast_to_file(node->children[i], indent + 2, out);
    }
}

/* Entry point */
int main() {
    FILE *f = fopen("output.txt", "r");
    if (!f) {
        perror("Failed to open input file output.txt");
        return 1;
    }
    
    ASTNode *root = parse_ast(f);
    fclose(f);
    if (!root) {
        fprintf(stderr, "Failed to parse AST\n");
        return 1;
    }
    
    optimize_ast(root);
    
    FILE *out = fopen("newOutput.txt", "w");
    if (!out) {
        perror("Failed to open output file newOutput.txt");
        free_ast(root);
        return 1;
    }
    
    print_ast_to_file(root, 0, out);
    fclose(out);
    free_ast(root);
    return 0;
}
