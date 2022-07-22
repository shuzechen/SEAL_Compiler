1. 3 axes
	- structure
	- abstraction
	- name space
2. From graphical IR to linear IR
	- motivation : the input of a assembly-language compiler is linear IR
	- GOAL : one linear IR called "three-address-code"
	- Format :    i <-- j op k
3. How to generate TAC ?
	- build DAG
		- Hash table
		- examples ?