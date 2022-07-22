1. **GOAL** : Catch all remaining errors
2. **Method** : Recursive- descent of AST
	 - Pre-processing of node
	 - RECURS on its child
	 - Post-processing of node
3. **Scope** :
	- [x]  Not declared
	- [x] Redefinition
	- SOLUTION : a stack of (unordered_)map  
4. **Type-checker** :
	- calculate a type for all expressions
	- TYPE-INFERENCE 
		- Recursion : the result is on its sub-expression
	- [x] Lvalue 未测试
	- [x] unmatched type
5. **Other Problems : 
	- [x] Out-of-boundary
	- [x] Break/Continue 