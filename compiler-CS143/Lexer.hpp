#ifndef SEAL_LEXER_HPP
#define SEAL_LEXER_HPP

// Inplementation of a transition-diagram-based LA

#include <iostream>
#include <cstdio>
#include <vector>
#include <string>
#include <cctype>
#include "SEALexception.hpp"

enum Tokentype{
    ID = 0, INT_ARRAY = 1, BOOL_ARRAY = 2, NUM = 3, 
    IF = 4, ELSE = 5, WHILE = 6, FOR = 7,  SENTI=8, BREAK=9, CONTINUE=10, 
    ASSIGN=11, ADD=12, SUB=13, MUL=14, DIV=15, 
    INPUT=16, PRINT=17, RETURN=18, MOD=19, NEG=20, 
    AND=21, BAND=22, OR=23 ,BOR=24, XOR=25, BXOR=26, COMPL=27, 
    BINOP=28, NOTEQ=29, EQ=30, NOT=31, LT=32 , LE=33 , GT=34 , GE=35,
    UNOP=36,
    _EOF=37,COMMENT=38, DQM=39, QM=40, SEMICOLON=41, 
    VAR=42, TYPE=43, _INT=44, _BOOL=45, _STRING=46,
    CONSTRING=47, LSB=48, RSB=49, LBC=50, RBC=51, LBT=52, RBT=53,
    STMTBLOCK = 54, BOOLEAN = 55, _BIND = 56, STRING_ARRAY = 57, COMMA = 58
};
// BAND : bit-and
class Token{
    public:
    long long value;
    std::string key;
    Tokentype type;
    Token(){};
    Token(Tokentype t):type(t){};
    Token(int v, Tokentype t, std::string id_name):value(v),type(t), key(id_name){};
    Token(Tokentype t, std::string id_name):value(0),type(t), key(id_name){};
    Token(Tokentype t, long long v):value(v),type(t), key(""){};
    Token(const Token &other){
        value = other.value;
        key = other.key;
        type = other.type;
    }
    void print_test(){
        std::cout<<"["<<type<<": "<<key<<", "<< value <<"]"<<'\n';
    }
};

class Lexer{
    public:
    char sentinel;
    std::istream &is;
    std::vector<Token> token_sq;
    //std::vector<Token>::iterator next;
    Lexer(std::istream &inp):is(inp),sentinel(0){};

    inline char read(){
        sentinel = getchar();
        return sentinel;
    }

    Token Tokenize(){
        read();
        std::string keyword = "";
        //space
        while (isspace(sentinel))// || sentinel == '\n' || sentinel == '\r' || sentinel == '\t' )
        {
            read();
        }

        if (sentinel == EOF) return Token(_EOF);
        if (isalpha(sentinel)){
            keyword += sentinel;
            while(isalpha(read())){
                keyword += sentinel;
            }
            std::cin.putback(sentinel);
            if(keyword == "Int") return Token(TYPE, _INT);
            else if(keyword == "var") return Token(VAR);
            else if(keyword == "Bool") return Token(TYPE, _BOOL);
            else if(keyword == "String") return Token(TYPE, _STRING);
            else if(keyword == "print") return Token(PRINT);
            else if(keyword == "input") return Token(INPUT);
            else if(keyword == "if") return Token(IF);
            else if(keyword == "else") return Token(ELSE);
            else if(keyword == "while") return Token(WHILE);
            else if(keyword == "for") return Token(FOR);
            else if(keyword == "return") return Token(RETURN);
            else if(keyword == "continue") return Token(CONTINUE);
            else if(keyword == "break") return Token(BREAK);
            else if(keyword == "true") return Token(BOOLEAN, 1);
            else if(keyword == "false") return Token(BOOLEAN, 0);
            else if(keyword == "Array"){
                read();read();read();read();
                std::string atype = "";
                while(isalpha(read())){
                    atype += sentinel;
                }
                if (atype == "Int") return Token(TYPE, INT_ARRAY);
                else if (atype == "Bool") return Token(TYPE, BOOL_ARRAY);
                else if (atype == "String") return Token(TYPE, STRING_ARRAY);              
            }else{
                bool flag = false;
                while(isalpha(sentinel) || isdigit(sentinel) || sentinel == '_'){
                    keyword += sentinel;
                    flag = true;
                    read();
                }
                if (flag == true) std::cin.putback(sentinel);
                // TO DO 
                // varname check
                // ConstString 
                return Token(ID, keyword);
            }

        }else if(isdigit(sentinel)){
            keyword += sentinel;
            read();
            if (isdigit(sentinel)){
                keyword += sentinel;
                while (isdigit(read())){
                    keyword += sentinel;
                }
                std::cin.putback(sentinel);
                return Token(NUM, stoll(keyword));
            }else if (keyword =="0" && (sentinel == 'x' || sentinel == 'X')){
                while (isdigit(sentinel) || isalpha(sentinel)){
                    keyword += sentinel;
                    read();
                }
                std::cin.putback(sentinel);
                return Token(NUM, stoll(keyword,0,0));
            }else{
                std::cin.putback(sentinel);
                return Token(NUM, stoll(keyword));
            }
        }else if(sentinel == '<'){
            read();
            if (sentinel == '=') return Token(BINOP , LE);
            std::cin.putback(sentinel);
            return Token(BINOP, LT);
        }else if(sentinel == '>'){
            read();
            if (sentinel == '=') return Token(BINOP , GE);
            std::cin.putback(sentinel);
            return Token(BINOP, GT);
        }else if(sentinel == '='){
            read();
            if (sentinel == '=') return Token(BINOP , EQ);
            std::cin.putback(sentinel);
            return Token(BINOP, ASSIGN);
        }else if(sentinel == '!'){
            read();
            if (sentinel == '=') return Token(BINOP , NOTEQ);
            std::cin.putback(sentinel);
            return Token(UNOP, NOT); 
        }else if(sentinel == '+') return Token(BINOP, ADD);
        else if (sentinel == '-'){
            if (token_sq.empty()) return Token(UNOP, NEG);
            if (token_sq.back().type == ID || token_sq.back().type == NUM || token_sq.back().type == RSB)
                return Token(BINOP, SUB);
            else return Token(UNOP, NEG);
            /*
                Not support String here
            */
        }else if(sentinel == '*') return Token(BINOP, MUL);
        else if(sentinel == '&') {
            read();
            if (sentinel == '&') return Token(BINOP, AND);
            std::cin.putback(sentinel);
            return Token(BINOP, BAND);
        }else if(sentinel == '|') {
            read();
            if (sentinel == '|') return Token(BINOP, OR);
            std::cin.putback(sentinel);
            return Token(BINOP, BOR);
        }else if(sentinel == '^') return Token(BINOP,XOR);
        else if(sentinel == '~') return Token(UNOP, COMPL);
        else if(sentinel == '/'){
            read();
            if (sentinel == '/'){
                do{
                    read();
                }while(sentinel != '\n' && sentinel != EOF && sentinel != '\r');
                std::cin.putback(sentinel);
                //return Tokenize(is);
                return Token(COMMENT);
            }else if (sentinel == '*'){
                read();
                if (sentinel == EOF) 
                    throw Syntactic_ERROR();//std::cout<<"ERROR"<<'\n';
                while(1){
                    char temp = sentinel;
                    read();
                    if (temp == '*' && sentinel == '/') break;
                    if (sentinel == EOF) 
                        throw Syntactic_ERROR();
                        //std::cout<<"ERROR"<<'\n';
                }
                //return Tokenize(is);
                return Token(COMMENT);
            }else{
                std::cin.putback(sentinel);
                return Token(BINOP, DIV);    
            }
        }
        else if(sentinel == '[') return Token(LSB);
        else if(sentinel == ']') return Token(RSB);
        else if(sentinel == '{') return Token(LBC);
        else if(sentinel == '}') return Token(RBC);
        else if(sentinel == '(') return Token(LBT);
        else if(sentinel == ')') return Token(RBT);
        else if(sentinel == '\"'){
            keyword += sentinel;
            read();
            if (sentinel == EOF) 
                throw Syntactic_ERROR();
            while(sentinel != '\"'){
                keyword += sentinel;
                read();
                if (sentinel == EOF) 
                    throw Syntactic_ERROR();
            }
            keyword += sentinel;
            return Token(CONSTRING, keyword);
        }else if(sentinel == ';') return Token(SEMICOLON);
        else if (sentinel ==',') return Token(COMMA);
        // TO DO THROW AN ERROR
        // 转义字符
        //std::cout<<"ERROR"<<'\n';
        throw Unrecognized_symbol_ERROR();
    }

    std::vector<Token> gettoken(){
        Token temp;
        while(1){
            temp = Tokenize();
            if (temp.type == _EOF){
                token_sq.push_back(temp);
                break;
            }else if(temp.type == COMMENT){
                continue;
            }
            else{
                token_sq.push_back(temp);
            }
        }

        return token_sq;
    }
};



#endif
