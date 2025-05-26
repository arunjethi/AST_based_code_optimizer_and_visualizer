# AST_based_code_optimizer_and_visualizer

1.  bison -d parser.y
2.  flex lexer.l
3.  gcc parser.tab.h parser.tab.c lex.yy.c ast.c ast.h main.c -o ast
4.  ./ast
    (AST saved to output.txt)


download graphviz:    https://graphviz.org/download/
 
If you're using MSYS2, you can install Graphviz development files via pacman:

5.  run this command
    pacman -S mingw-w64-x86_64-graphviz


6.  Then compile with:
    gcc ast_to_png.c -o ast_to_png -lgvc -lcgraph


7.  run:
    ./ast_to_png
    message shown: AST graph saved to ast_output.png

8.  open the image
    explorer ast_output.png

/* 
    write logic to optimise the AST file : (ast_optimize.c)=> i/p: output.txt  , o/p: newOutput.txt
    newOutput.txt:  is a file in which we have AST optimized with the following techniques.
    1. removing the constant folding.
    2. remove the dead code.
    3. loop unrolling problem solved
*/

9.  compile it
    gcc ast_optimize.c -o ast_optimize

10. run
    ./ast_optimize		=> newOutput.txt

/*
    optimized AST to c code generation (ast_to_c.c)
    i/p:newOutput.txt
    o/p:optimizedCode.c (free from constant folding , dead Code and loop unrolling).
*/

11. compile it: 
    gcc ast_to_c.c -o ast_to_c

12. run  
    ./ast_to_c





