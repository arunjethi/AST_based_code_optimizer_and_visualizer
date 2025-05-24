#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <graphviz/cgraph.h>
#include <graphviz/gvc.h>

#define MAX_LINE 512
#define MAX_NODES 2048

typedef struct
{
    int indent;
    char id[32];
} StackEntry;

int get_indent(const char *line)
{
    int count = 0;
    while (*line == ' ')
    {
        count++;
        line++;
    }
    return count;
}

void extract_label(const char *src, char *out)
{
    const char *start = src;
    while (*src && *src != '\n')
        src++;

    int len = src - start;
    strncpy(out, start, len);
    out[len] = '\0';
}

int main()
{
    // FILE *file = fopen("output.txt", "r");
    FILE *file = fopen("newOutput.txt", "r");
    if (!file)
    {
        perror("Failed to open file");
        return 1;
    }

    GVC_t *gvc = gvContext();
    Agraph_t *graph = agopen("AST", Agstrictdirected, NULL);

    char line[MAX_LINE];
    StackEntry stack[MAX_NODES];
    int top = -1, id_counter = 0;

    while (fgets(line, sizeof(line), file))
    {
        int indent = get_indent(line);
        char label[MAX_LINE];
        extract_label(line + indent, label);

        char node_id[32];
        snprintf(node_id, sizeof(node_id), "n%d", id_counter++);

        Agnode_t *node = agnode(graph, node_id, 1);
        agset(node, "label", label);

        while (top >= 0 && stack[top].indent >= indent)
            top--;

        if (top >= 0)
        {
            agedge(graph,
                   agnode(graph, stack[top].id, 0),
                   node,
                   NULL, 1);
        }

        top++;
        strcpy(stack[top].id, node_id);
        stack[top].indent = indent;
    }

    fclose(file);

    gvLayout(gvc, graph, "dot");
    gvRenderFilename(gvc, graph, "png", "ast_output.png");
    gvFreeLayout(gvc, graph);
    agclose(graph);
    gvFreeContext(gvc);

    printf("AST graph saved to ast_output.png\n");
    return 0;
}
