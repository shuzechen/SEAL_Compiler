#ifndef SEAL_CHEN_IR_HPP
#define SEAL_CHEN_IR_HPP

#include "AST.hpp"
#include "Lexer.hpp"
#include <iostream>
#include <vector>
#include <string>

/*
 * This is a self-desgined Linear IR (name it Chen-IR).
 * Chen-IR can be genreated through traversing the AST.
*/

class IR{
    public :
    int op;
    std::string id;
    long long arg_1;
    long long arg_2;
    int re_index;
    static int ricounter;
    IR(std::string _id):op(_BIND),id(_id){re_index = ricounter++;}
    //IR(int _op, long long _arg_1):op(_op),arg_1(_arg_1){re_index = ricounter++;}
    IR(int _op, long long _arg_1, long long _arg_2):op(_op),arg_1(_arg_1),arg_2(_arg_2){
        if (op == ASSIGN){
            re_index = _arg_2;
        }else re_index = ricounter++;
    }
    void print_CIR(){
        switch(op){
            case _BIND : std::cout<<"BIND "<<id<<" t" <<re_index<<'\n';break;
            case ASSIGN : std::cout<<"= "<<arg_1<<" t" <<re_index<<'\n';break;
            case MUL: std::cout<<"* "<<arg_1<<" "<<arg_2<<" t" <<re_index<<'\n';break;
            case ADD: std::cout<<"+ "<<arg_1<<" "<<arg_2<<" t" <<re_index<<'\n';break;
            case SUB: std::cout<<"- "<<arg_1<<" "<<arg_2<<" t" <<re_index<<'\n';break;
            case DIV: std::cout<<"/ "<<arg_1<<" "<<arg_2<<" t" <<re_index<<'\n';break;
        }
    }
};

int IR::ricounter = 0;

class basic_block{
    public :
    std::vector<IR> content;
};

/*
class CFG{
    public :
    std::vector<basic_block> 
};
*/




#endif