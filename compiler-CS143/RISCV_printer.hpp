# ifndef RISCV_GEN_HPP
# define RISCV_GEN_HPP

# include <cstdio>
# include "Lexer.hpp"

void printheader(){
    printf("\t.text\n");
    printf("\t.globl	main\n");
    printf("\t.type	main, @function\n");
    printf("main:\n");
}

void printtailer(){
    printf("\tjr ra\n");
}

void printBinop(long long op, int nt){
    switch(op){
        case ADD : printf("\tadd t%d,t%d,t%d\n",nt, nt, nt+1);break;
        case SUB : printf("\tsub t%d,t%d,t%d\n",nt, nt, nt+1);break;
        case MUL : printf("\tmul t%d,t%d,t%d\n",nt, nt, nt+1);break;
        case DIV : printf("\tdiv t%d,t%d,t%d\n",nt, nt, nt+1);break;
        case BAND : printf("\tand t%d,t%d,t%d\n",nt, nt, nt+1);break;
        case BOR : printf("\tor t%d,t%d,t%d\n",nt, nt, nt+1);break;
        case XOR : printf("\txor t%d,t%d,t%d\n",nt, nt, nt+1);break;
    }
}

void printUnop(long long op, int nt){
    switch(op){
        case NEG : printf("\tneg t%d,t%d\n",nt, nt);break;
        case COMPL : printf("\tnot t%d,t%d\n",nt, nt);break;
    }
}

void printBinop(long long op , long long imm, short pos, int nt){
    if (pos == 1){ // set 1 the register
        switch(op){
            case ADD : printf("\taddi\tt%d,t%d,%lld\n" ,nt, nt, imm);break;
            case SUB : printf("\taddi\tt%d,t%d,%lld\n" ,nt, nt, (-1) * imm);break;
            case MUL : printf("\tli t%d,%lld\n\tmul t%d,t%d,t%d\n",nt+1, imm, nt, nt,nt+1);break;
            case DIV : printf("\tli t%d,%lld\n\tdiv t%d,t%d,t%d\n",nt+1, imm, nt, nt, nt+1);break;
            case BAND : printf("\tandi t%d,t%d,%lld\n" ,nt, nt, imm);break;
            case BOR : printf("\tori t%d,t%d,%lld\n" ,nt, nt, imm);break;
            case XOR : printf("\txori t%d,t%d,%lld\n" ,nt, nt, imm);break;
        }
    }else{
        switch(op){
            case ADD : printf("\taddi\tt%d,t%d,%lld\n" ,nt, nt+1, imm);break;
            case SUB : printf("\tli t%d,%lld\n\tsub t%d,t%d,t%d\n",nt, imm, nt, nt, nt+1);break;
            case MUL : printf("\tli t%d,%lld\n\tmul t%d,t%d,t%d\n",nt, imm, nt, nt,nt+1);break;
            case DIV : printf("\tli t%d,%lld\n\tdiv t%d,t%d,t%d\n", nt, imm, nt, nt, nt+1);break;
            case BAND : printf("\tandi t%d,t%d,%lld\n",nt, nt+1, imm);break;
            case BOR : printf("\tori t%d,t%d,%lld\n",nt, nt+1, imm);break;
            case XOR : printf("\txori t%d,t%d,%lld\n",nt, nt+1, imm);break;
        }
    }
}

long long cal_cc(long long op, long long a,  long long b){
    switch(op){
        case ADD : return a+b;
        case SUB : return a-b;
        case MUL : return a*b;
        case DIV : return a/b;
        case BAND : return a&b;
        case BOR : return a|b;
        case XOR : return a^b; 
        case EQ : return a == b;
        case NOTEQ : return a != b;
        case GT : return a>b;
        case LT : return a<b;
        case GE : return a>=b;
        case LE : return a<=b;
    }
}

long long cal_c(long long op , long long v){
    switch(op){
        case NEG : return -v;
        case COMPL : return ~v;
        case NOT : if (v == 0) return 1;
                    else return 0;
    }
}

void printrodata(){
    printf("\t.text\n");
    printf("\t.section\t.rodata\n");
}



# endif 