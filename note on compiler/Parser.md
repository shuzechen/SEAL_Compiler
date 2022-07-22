1. AST:
	- The root is labeled by the start symbol.
	- Each leaf is labeled by a terminal or by ε.
	- Each interior node is labeled by a nonterminal.
2. 不同环节对应的AST
3. Grammar 
	- A set of production rules
	- **Motivation :** To verofy if a string is a product of the language.
	- But we want more than verification
4. Derivation:
	- left-most Derivation: choose the most left non-terminal
5. Top-down Parsing 
	- Recursive decent
		- **Key Idea :** always try the left-most production until the *terminal*, if the terminal equals the token-sequence, then keeps recursing; Otherwise backtrack.
	- ![RDA](C:\Users\Connor\Desktop\Missing-semester-HW1.assets\RDA.png)
6. Problems of RDA
	- It does not support 'full' test. Say : **int | int * int** and the INPUT is int * int. Then we will give a FALSE.
	- Ledt Recursion: S--> Sa is an endless loop
7. Eliminating ambiguity 
	- if 
8. Elimianting Left-recurison
9. 我能不能在语法检查时修正Left-value
	-  遍历AST的过程中，如果ls不是Lvalue，就报错