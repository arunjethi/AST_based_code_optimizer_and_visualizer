#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CHILDREN 10
#define MAX_LINE_LEN 256

typedef enum
{
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

typedef struct ASTNode
{
    NodeType type;
    char *name;         // e.g. function name, variable name, function call name
    int int_value;      // for INT nodes
    char op[4];         // operator for binary/unary expr
    char *string_value; // for STRING nodes
    struct ASTNode *children[MAX_CHILDREN];
    int child_count;
} ASTNode;

// Forward declarations
ASTNode *parse_ast_recursive(FILE *f, int indent);
void free_ast(ASTNode *node);
void generate_c_code(ASTNode *node, int indent, FILE *out);

// Helper functions from previous example
void skip_spaces(const char **str)
{
    while (**str == ' ' || **str == '\t')
        (*str)++;
}

int count_leading_spaces(const char *line)
{
    int count = 0;
    while (*line == ' ')
    {
        count++;
        line++;
    }
    return count;
}

NodeType node_type_from_string(const char *str)
{
    if (strcmp(str, "FUNCTION_DEF") == 0)
        return NODE_FUNCTION_DEF;
    if (strcmp(str, "SEQUENCE") == 0)
        return NODE_SEQUENCE;
    if (strcmp(str, "DECLARATION") == 0)
        return NODE_DECLARATION;
    if (strcmp(str, "INT") == 0)
        return NODE_INT;
    if (strcmp(str, "BINARY_EXPR") == 0)
        return NODE_BINARY_EXPR;
    if (strcmp(str, "VAR") == 0)
        return NODE_VAR;
    if (strcmp(str, "IF_STMT") == 0)
        return NODE_IF_STMT;
    if (strcmp(str, "FUNCTION_CALL") == 0)
        return NODE_FUNCTION_CALL;
    if (strcmp(str, "EXPR_LIST") == 0)
        return NODE_EXPR_LIST;
    if (strcmp(str, "FOR_STMT") == 0)
        return NODE_FOR_STMT;
    if (strcmp(str, "UNARY_EXPR") == 0)
        return NODE_UNARY_EXPR;
    if (strcmp(str, "RETURN_STMT") == 0)
        return NODE_RETURN_STMT;
    if (strcmp(str, "STRING") == 0)
        return NODE_STRING;
    return NODE_UNKNOWN;
}

NodeType parse_line(const char *line, char **arg)
{
    *arg = NULL;
    const char *p = line;
    char type_buf[64];
    int i = 0;
    while (*p && *p != ' ' && *p != '(' && *p != '\n' && i < 63)
    {
        type_buf[i++] = *p;
        p++;
    }
    type_buf[i] = 0;
    NodeType t = node_type_from_string(type_buf);
    if (t == NODE_UNKNOWN)
        return NODE_UNKNOWN;

    skip_spaces(&p);
    if (*p == '(')
    {
        p++;
        const char *start = p;
        while (*p && *p != ')')
            p++;
        if (*p != ')')
            return NODE_UNKNOWN;
        int len = (int)(p - start);
        *arg = malloc(len + 1);
        strncpy(*arg, start, len);
        (*arg)[len] = 0;
    }
    return t;
}
void strip_outer_quotes(char *s) {
    int len = strlen(s);
    // Remove all leading quotes
    int start = 0;
    while (start < len && s[start] == '"') {
        start++;
    }
    // Remove all trailing quotes
    int end = len - 1;
    while (end >= start && s[end] == '"') {
        end--;
    }

    if (start > 0 || end < len - 1) {
        int new_len = end - start + 1;
        if (new_len > 0) {
            memmove(s, s + start, new_len);
        }
        s[new_len] = '\0';
    }
}

ASTNode *parse_ast_recursive(FILE *f, int current_indent)
{
    char line[MAX_LINE_LEN];
    ASTNode *node = NULL;

    long last_pos = ftell(f);
    if (!fgets(line, MAX_LINE_LEN, f))
        return NULL;

    int indent = count_leading_spaces(line);
    if (indent < current_indent)
    {
        fseek(f, last_pos, SEEK_SET);
        return NULL;
    }
    if (indent > current_indent)
    {
        fprintf(stderr, "Unexpected indentation\n");
        return NULL;
    }

    char *arg = NULL;
    char *trim_line = line + indent;
    NodeType t = parse_line(trim_line, &arg);
    if (t == NODE_UNKNOWN)
    {
        fprintf(stderr, "Unknown node type: %s\n", trim_line);
        if (arg)
            free(arg);
        return NULL;
    }

    node = calloc(1, sizeof(ASTNode));
    node->type = t;

    if (arg)
    {
        switch (t)
        {
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
            strip_outer_quotes(node->string_value);
            break;
        default:
            free(arg);
            break;
        }
    }

    while (1)
    {
        long pos_before = ftell(f);
        ASTNode *child = parse_ast_recursive(f, current_indent + 2);
        if (!child)
        {
            fseek(f, pos_before, SEEK_SET);
            break;
        }
        if (node->child_count < MAX_CHILDREN)
        {
            node->children[node->child_count++] = child;
        }
        else
        {
            fprintf(stderr, "Too many children\n");
            free_ast(child);
            break;
        }
    }

    return node;
}

ASTNode *parse_ast(FILE *f)
{
    return parse_ast_recursive(f, 0);
}

void free_ast(ASTNode *node)
{
    if (!node)
        return;
    if (node->name)
        free(node->name);
    if (node->string_value)
        free(node->string_value);
    for (int i = 0; i < node->child_count; i++)
    {
        free_ast(node->children[i]);
    }
    free(node);
}

void print_indent(FILE *out, int indent)
{
    for (int i = 0; i < indent; i++)
        fputc(' ', out);
}

// Forward declaration for expressions printing
void print_expression(ASTNode *node, FILE *out);

void generate_c_code(ASTNode *node, int indent, FILE *out)
{
    if (!node)
        return;

    switch (node->type)
    {
    case NODE_FUNCTION_DEF:
        if (node->name)
        {
            fprintf(out, "int %s() {\n", node->name);
            for (int i = 0; i < node->child_count; i++)
                generate_c_code(node->children[i], indent + 4, out);
            fprintf(out, "}\n");
        }
        break;

    case NODE_SEQUENCE:
        for (int i = 0; i < node->child_count; i++)
            generate_c_code(node->children[i], indent, out);
        break;

    case NODE_DECLARATION:
        print_indent(out, indent);
        if (node->child_count == 1 && node->children[0]->type == NODE_INT)
        {
            fprintf(out, "int %s = %d;\n", node->name, node->children[0]->int_value);
        }
        else if (node->child_count == 1)
        {
            // e.g. int d = a + 8;
            fprintf(out, "int %s = ", node->name);
            print_expression(node->children[0], out);
            fprintf(out, ";\n");
        }
        else
        {
            fprintf(out, "int %s;\n", node->name);
        }
        break;

    case NODE_RETURN_STMT:
        print_indent(out, indent);
        fprintf(out, "return ");
        if (node->child_count == 1 && node->children[0]->type == NODE_INT)
        {
            fprintf(out, "%d", node->children[0]->int_value);
        }
        else if (node->child_count == 1)
        {
            print_expression(node->children[0], out);
        }
        fprintf(out, ";\n");
        break;

    case NODE_FOR_STMT:
        if (node->child_count == 4)
        {
            // Format: declaration, condition, unary_expr (increment), body
            print_indent(out, indent);
            fprintf(out, "for (");

            // Declaration (int i = 0)
            ASTNode *decl = node->children[0];
            if (decl->type == NODE_DECLARATION && decl->child_count == 1 && decl->children[0]->type == NODE_INT)
            {
                fprintf(out, "int %s = %d; ", decl->name, decl->children[0]->int_value);
            }
            else
            {
                fprintf(out, "; "); // fallback
            }

            // Condition
            ASTNode *cond = node->children[1];
            print_expression(cond, out);
            fprintf(out, "; ");

            // Increment (UNARY_EXPR)
            ASTNode *inc = node->children[2];
            if (inc->type == NODE_UNARY_EXPR && inc->child_count == 1 && inc->children[0]->type == NODE_VAR)
            {
                fprintf(out, "%s%s", inc->children[0]->name, inc->op);
            }
            else
            {
                // fallback
                fprintf(out, ";");
            }

            fprintf(out, ") {\n");

            // Body (usually FUNCTION_CALL)
            generate_c_code(node->children[3], indent + 4, out);

            print_indent(out, indent);
            fprintf(out, "}\n");
        }
        break;

    case NODE_FUNCTION_CALL:
        print_indent(out, indent);
        if (node->name)
        {
            fprintf(out, "%s(", node->name);
            if (node->child_count == 1 && node->children[0]->type == NODE_EXPR_LIST)
            {
                ASTNode *expr_list = node->children[0];
                for (int i = 0; i < expr_list->child_count; i++)
                {
                    if (i > 0)
                        fprintf(out, ", ");
                    ASTNode *expr = expr_list->children[i];
                    if (expr->type == NODE_STRING)
                    {
                        fprintf(out, "\"%s\"", expr->string_value);
                    }
                    else
                    {
                        print_expression(expr, out);
                    }
                }
            }
            fprintf(out, ");\n");
        }
        break;

    case NODE_IF_STMT:
        if (node->child_count >= 2)
        {
            print_indent(out, indent);
            fprintf(out, "if (");
            print_expression(node->children[0], out);
            fprintf(out, ") {\n");
            generate_c_code(node->children[1], indent + 4, out);
            print_indent(out, indent);
            fprintf(out, "}\n");
        }
        break;

    default:
        // For other nodes, just recurse on children
        for (int i = 0; i < node->child_count; i++)
            generate_c_code(node->children[i], indent, out);
        break;
    }
}

// Print expressions (used in declarations, conditions, etc.)
void print_expression(ASTNode *node, FILE *out)
{
    if (!node)
        return;
    switch (node->type)
    {
    case NODE_INT:
        fprintf(out, "%d", node->int_value);
        break;

    case NODE_VAR:
        fprintf(out, "%s", node->name);
        break;

    case NODE_BINARY_EXPR:
        if (node->child_count == 2)
        {
            fprintf(out, "(");
            print_expression(node->children[0], out);
            fprintf(out, " %s ", node->op);
            print_expression(node->children[1], out);
            fprintf(out, ")");
        }
        break;

    case NODE_UNARY_EXPR:
        if (node->child_count == 1)
        {
            fprintf(out, "%s%s", node->children[0]->name, node->op);
        }
        break;

    case NODE_FUNCTION_CALL:
        fprintf(out, "%s(", node->name);
        if (node->child_count == 1 && node->children[0]->type == NODE_EXPR_LIST)
        {
            ASTNode *expr_list = node->children[0];
            for (int i = 0; i < expr_list->child_count; i++)
            {
                if (i > 0)
                    fprintf(out, ", ");
                print_expression(expr_list->children[i], out);
            }
        }
        fprintf(out, ")");
        break;

    case NODE_STRING:
        fprintf(out, "\"%s\"", node->string_value);
        break;

    default:
        // Unknown expression fallback
        fprintf(out, "/* expr */");
        break;
    }
}

int main()
{
    FILE *in = fopen("newOutput.txt", "r");
    if (!in)
    {
        fprintf(stderr, "Cannot open newOutput.txt for reading\n");
        return 1;
    }

    ASTNode *root = parse_ast(in);
    fclose(in);

    if (!root)
    {
        fprintf(stderr, "Failed to parse AST\n");
        return 1;
    }

    FILE *out = fopen("optimizedCode.c", "w");
    if (!out)
    {
        fprintf(stderr, "Cannot open output.c for writing\n");
        free_ast(root);
        return 1;
    }

    fprintf(out, "#include <stdio.h>\n\n");
    generate_c_code(root, 0, out);

    fclose(out);
    free_ast(root);

    return 0;
}
