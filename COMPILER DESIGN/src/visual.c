#include <stdio.h>

int main() {
    FILE *f = fopen("ast.dot", "w");
    if (!f) {
        perror("Failed to open file");
        return 1;
    }

    fprintf(f, "digraph AST {\n");
    fprintf(f, "  node0 [label=\"FUNCTION_DEF (main)\"];\n");

    fprintf(f, "  node1 [label=\"DECLARATION (a)\"];\n");
    fprintf(f, "  node2 [label=\"BINARY_EXPR (*)\"];\n");
    fprintf(f, "  node3 [label=\"INT (5)\"];\n");
    fprintf(f, "  node4 [label=\"INT (3)\"];\n");
    fprintf(f, "  node0 -> node1;\n");
    fprintf(f, "  node1 -> node2;\n");
    fprintf(f, "  node2 -> node3;\n");
    fprintf(f, "  node2 -> node4;\n");

    fprintf(f, "  node5 [label=\"DECLARATION (b)\"];\n");
    fprintf(f, "  node6 [label=\"INT (10)\"];\n");
    fprintf(f, "  node0 -> node5;\n");
    fprintf(f, "  node5 -> node6;\n");

    fprintf(f, "  node7 [label=\"DECLARATION (c)\"];\n");
    fprintf(f, "  node8 [label=\"INT (0)\"];\n");
    fprintf(f, "  node0 -> node7;\n");
    fprintf(f, "  node7 -> node8;\n");

    fprintf(f, "  node9 [label=\"IF_STMT (0)\"];\n");
    fprintf(f, "  node10 [label=\"FUNCTION_CALL (printf)\"];\n");
    fprintf(f, "  node11 [label=\"STRING (This will never be printed.)\"];\n");
    fprintf(f, "  node0 -> node9;\n");
    fprintf(f, "  node9 -> node10;\n");
    fprintf(f, "  node10 -> node11;\n");

    fprintf(f, "  node12 [label=\"IF_STMT (1)\"];\n");
    fprintf(f, "  node13 [label=\"FUNCTION_CALL (printf)\"];\n");
    fprintf(f, "  node14 [label=\"STRING (This will always be printed.)\"];\n");
    fprintf(f, "  node0 -> node12;\n");
    fprintf(f, "  node12 -> node13;\n");
    fprintf(f, "  node13 -> node14;\n");

    fprintf(f, "  node15 [label=\"DECLARATION (d)\"];\n");
    fprintf(f, "  node16 [label=\"BINARY_EXPR (+)\"];\n");
    fprintf(f, "  node17 [label=\"VAR (a)\"];\n");
    fprintf(f, "  node18 [label=\"BINARY_EXPR (*)\"];\n");
    fprintf(f, "  node19 [label=\"INT (2)\"];\n");
    fprintf(f, "  node20 [label=\"INT (4)\"];\n");
    fprintf(f, "  node0 -> node15;\n");
    fprintf(f, "  node15 -> node16;\n");
    fprintf(f, "  node16 -> node17;\n");
    fprintf(f, "  node16 -> node18;\n");
    fprintf(f, "  node18 -> node19;\n");
    fprintf(f, "  node18 -> node20;\n");

    fprintf(f, "  node21 [label=\"FOR_STMT\"];\n");
    fprintf(f, "  node22 [label=\"DECLARATION (i)\"];\n");
    fprintf(f, "  node23 [label=\"INT (0)\"];\n");
    fprintf(f, "  node24 [label=\"BINARY_EXPR (<)\"];\n");
    fprintf(f, "  node25 [label=\"VAR (i)\"];\n");
    fprintf(f, "  node26 [label=\"INT (3)\"];\n");
    fprintf(f, "  node27 [label=\"UNARY_EXPR (++)\"];\n");
    fprintf(f, "  node28 [label=\"VAR (i)\"];\n");
    fprintf(f, "  node29 [label=\"FUNCTION_CALL (printf)\"];\n");
    fprintf(f, "  node30 [label=\"STRING (Hello Kunal)\"];\n");

    fprintf(f, "  node0 -> node21;\n");
    fprintf(f, "  node21 -> node22;\n");
    fprintf(f, "  node22 -> node23;\n");
    fprintf(f, "  node21 -> node24;\n");
    fprintf(f, "  node24 -> node25;\n");
    fprintf(f, "  node24 -> node26;\n");
    fprintf(f, "  node21 -> node27;\n");
    fprintf(f, "  node27 -> node28;\n");
    fprintf(f, "  node21 -> node29;\n");
    fprintf(f, "  node29 -> node30;\n");

    fprintf(f, "  node31 [label=\"RETURN_STMT\"];\n");
    fprintf(f, "  node32 [label=\"INT (0)\"];\n");
    fprintf(f, "  node0 -> node31;\n");
    fprintf(f, "  node31 -> node32;\n");

    fprintf(f, "}\n");

    fclose(f);

    printf("DOT file 'ast.dot' generated.\n");
    printf("Run: dot -Tpng ast.dot -o ast.png\n");
    return 0;
}
