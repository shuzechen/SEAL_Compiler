- [x] bne 1, 1 two const cases
- [x] 对于左值 ld t0, (0)s0 不一定要打印
2. Constant folding
- [x] 如果是数字则直接计算
- [x] if 直接判断的分支优化
3. minimize ld/sd
 - [x] mv t0,t1  
 - [x] too much ld
 - [x] Eliminating unreachable basic block 
 - [ ] common expression elimination （可以实现）
4. asm-optimizer
	- [x] peephole（可以实现）（潜力大）
5. Register allocation
	- [x] post-traversal the expression tree (containing risk when the depth is too large)
6. Dataflow analysis
	- ![dataflow_analysis](C:\Users\Connor\Desktop\workshop\dataflow_analysis.png)
	- [ ]  Constant propagation 
			It seems that I can make it better with rule 7.
			- How to break the endless loop? 
				- **Key Idea :** we guess one at the very beginning
			- Ordering : **Lub(least-upper-bound)** 
				- **Key Idea :** a way to abstract 8 rules in one;
									a way to prove the termination of the algorithm. 
	- [ ]  Dead code elimination
		
	