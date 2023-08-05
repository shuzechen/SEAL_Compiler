# SEAL_Compiler
A compiler for a basic programming language called SEAL. \
Major reference : CS143@Stanford University

This compiler is implemented based on most classical Lexer/Parser and work on a single IR : AST. It turns out AST is powerful enough to support RISCV-generator running on qemu and many traditional optimization techniques like constant folding and tree-based register allocation etc. 


