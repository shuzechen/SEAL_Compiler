1. 怎么绑定寄存器和变量地址
	- sp 相对寻址
2. x = 1+2
	- 如果op两头都是数字，那么直接让c++算好
3. 输出的效果是什么  ---> hello.s 
- [x] 打印.string的顺序
	- [ ] 遍历所有scanf/printf
	- [ ] preprocessing
	- [ ] list
	- [x] **solution :** 在结尾打印并添加.text .section .rodata
4. 用s0维护fp
	- [x] addi  s0 sp 32(cnt+2)
	- [x] 第 i (1-based) 个出现的位置为 -16-8i
5. 为什么要做 s0 相对寻址
	- Answer : 因为 sp 寻址不方便在统计时获得 offset
6. if (expr1 && expr2 && expr3...)   ---> ```
```C++
if (expr1){
	if (expr2){
		if (expr3){
			....
		}
	}
}
```


b[1] = a + 1 * 5/a &a|a^a;

4. RISC-V ISA

##### ATTENTION:
1. lw : sign-extension
   sw : unsigned-extension 
2. RISC-V is little-endian
3. 



2. riscv64-linux-gnu-gcc -S hello.c
3. riscv64-linux-gnu-gcc -o hello hello.s
4. qemu-riscv64 hello