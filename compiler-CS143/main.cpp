#include <iostream>
#include <fstream>
#include "Lexer.hpp"
#include "Parser.hpp"
#include "AST.hpp"
#include "SEALexception.hpp"
#include "peephole.hpp"
using namespace std;

int main()
{
    freopen("input.txt", "r", stdin);
    freopen("RISCV-target/pre_asm.txt", "w", stdout);
    Lexer lexer(cin);
    vector<Token> tokenlis = lexer.gettoken();
    vector<Token>::iterator ite = tokenlis.begin();
    /*
    while(ite != tokenlis.end()){
        (*ite++).print_test();
    }
    */
    
    Parser parser(tokenlis);
    auto root = parser.buildAST();
    root->semantic_analysis();
    //root->data_flow_analysis();
    root->generate_RiscV();
    Peephole_op();
    
}