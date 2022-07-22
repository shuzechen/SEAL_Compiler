#ifndef SEAL_AST_HPP
#define SEAL_AST_HPP

#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <unordered_map>
#include "Lexer.hpp"
#include "SEALexception.hpp"
#include "Chen_IR.hpp"
#include "RISCV_printer.hpp"

// reference : CS143 Stanford

long long block_counter = 2;
long long string_counter = 0;
bool glob_return = false;

class ASTnode{};

class Variablenode : public ASTnode{
    public:
    std::string id;
    long long type;
    Variablenode(Token _id, Token _type):id(_id.key), type(_type.value){}
};

class Arraynode : public ASTnode{
    public:
    std::string id;
    long long type;
    long long size;
    Arraynode(Token _id, Token _type , Token _size):
    id(_id.key), type(_type.value), size(_size.value){}
};

class Stmtnode : public ASTnode{
    public:
    Tokentype _type;
    Stmtnode(Tokentype t):_type(t){}
    virtual int type_checker(std::vector<std::unordered_map<std::string, std::pair<long long, long long> > > scope_table) = 0;
    virtual std::pair<int, long long> RiscV_generator(int nt, std::vector<std::unordered_map<std::string, int> > id_register, long long base_pos) = 0;
    virtual void getString() = 0;
};

class Programnode : public ASTnode{
    public:
    std::vector<std::unique_ptr<Variablenode> > ValDel;
    std::vector<std::unique_ptr<Arraynode> > AryDel;
    std::vector<std::unique_ptr<Stmtnode> > stmtDel;
    void semantic_analysis(){
        std::vector<std::unordered_map<std::string, std::pair<long long, long long> > > scope_table;
        std::unordered_map<std::string, std::pair<long long, long long> > basic_environment;
        std::vector<std::unique_ptr<Variablenode> >::iterator ite = ValDel.begin();
        while(ite != ValDel.end()){
            for (auto  i : basic_environment){
                if (i.first == (*ite)->id) {
                    //std::cout<<i.first;
                    throw Redefinition_ERROR();
                }
            }
            basic_environment[(*ite)->id] = std::make_pair((*ite)->type, 0);
            ite ++;
        }
        std::vector<std::unique_ptr<Arraynode> >::iterator aryite = AryDel.begin();
        while(aryite != AryDel.end()){
            for (auto  i : basic_environment){
                if (i.first == (*aryite)->id) {
                    //std::cout<<i.first;
                    throw Redefinition_ERROR();
                }
            }
            basic_environment[(*aryite)->id] = std::make_pair((*aryite)->type, (*aryite)->size);
            aryite ++;
        }
        scope_table.push_back(basic_environment);
        std::unordered_map<std::string, std::pair<long long, long long> > m;
        scope_table.push_back(m);
        std::vector<std::unique_ptr<Stmtnode> >::iterator stmtite = stmtDel.begin();
        while(stmtite != stmtDel.end()){
            (*stmtite)->type_checker(scope_table);
            stmtite ++;
        }
    }
    void generate_RiscV(){
        //printString
        std::vector<std::unique_ptr<Stmtnode> >::iterator string_ite = stmtDel.begin();
        while(string_ite != stmtDel.end()){
            (*string_ite)->getString();
            string_ite ++;
        }
        std::vector<std::unordered_map<std::string, int> > id_register;
        std::unordered_map<std::string, int> basic_environment;
        std::vector<std::unique_ptr<Variablenode> >::iterator ite = ValDel.begin();
        // basic_block Program_block;
        long long cur_off = -16; // pass down the appearance number
        printheader();
        while(ite != ValDel.end()){
            /*
            IR ir((*ite)->id);
            basic_environment[(*ite)->id] = ir.re_index;
            Program_block.content.push_back(ir);
            ir.print_CIR();
            ite ++;
            */
            cur_off -= 8;
            basic_environment[(*ite)->id] = cur_off;
            ite ++;
        }
        std::vector<std::unique_ptr<Arraynode> >::iterator aryite = AryDel.begin();
        while(aryite != AryDel.end()){
            cur_off -= 8 * ((*aryite)->size);
            basic_environment[(*aryite)->id] = cur_off;
            aryite ++;
        }
        printf("\taddi\tsp,sp,%lld\n" , cur_off);
        printf("\tsd  ra, %lld(sp)\n", cur_off * (-1) - 8);
        printf("\tsd  s0, %lld(sp)\n", cur_off * (-1) - 16);
        printf("\taddi\ts0,sp,%lld\n", cur_off *(-1));
        id_register.push_back(basic_environment);
        std::vector<std::unique_ptr<Stmtnode> >::iterator stmtite = stmtDel.begin();
        while(stmtite != stmtDel.end()){
            (*stmtite)->RiscV_generator(1,id_register, cur_off);
            stmtite ++;
        }
        if (glob_return == false)  printf("\tli a0,0\n");
        printf("\tld  ra, %lld(sp)\n", cur_off * (-1) - 8);
        printf("\tld  s0, %lld(sp)\n", cur_off * (-1) - 16);
        printf("\taddi\tsp,sp,%lld\n", (-1) * cur_off);
        printtailer();
    }
    
};

class Exprnode : public Stmtnode{
    public:
    Exprnode(Tokentype t):Stmtnode(t){}
    //virtual void inorder_traverse() = 0 ;
    virtual void get_identifier(std::vector<std::unordered_map<std::string, std::pair<long long, long long> > > scope_table, std::unordered_map<std::string, std::pair<long long, long long> > &m) = 0;
    virtual void NOT_traversal() = 0;
    virtual int get_lcid() = 0;
};

class Lvaluenode : public Exprnode{
    public:
    Lvaluenode(Tokentype t):Exprnode(t){}
    void NOT_traversal(){}
    void getString(){}
    virtual long long get_offset(std::vector<std::unordered_map<std::string, int> > id_register, long long base_pos) = 0;
    int get_lcid(){return -1;}
};

class Printnode : public Stmtnode{
    public:
    std::vector<std::unique_ptr<Exprnode> > opobj;
    long long lcid;
    std::vector<int> obj_type;
    Printnode():Stmtnode(PRINT){lcid = 0;}
    int type_checker(std::vector<std::unordered_map<std::string, std::pair<long long, long long> > > scope_table){
        std::vector<std::unique_ptr<Exprnode> >::iterator ite = opobj.begin();
        int temp  = 0;
        while(ite != opobj.end()){
            temp = (*ite)->type_checker(scope_table);
            obj_type.push_back(temp);
            ite++;
        }
        return 0;
    }
    void getString(){
        std::vector<std::unique_ptr<Exprnode> >::iterator ite = opobj.begin();
        while(ite != opobj.end()){
            (*ite)->getString();
            ite++;
        }
        //for (auto i : obj_type) std::cout<<i<<'\n';
        if (obj_type.front() == CONSTRING) lcid = (*(opobj.begin()))->get_lcid();
        else{
            if (string_counter == 0) printrodata();
            lcid = string_counter++;
            printf(".LC%lld:\n", lcid);
            std::string print_key = "";
            for (int i = 1; i <= opobj.size(); i++){
                switch(obj_type[i-1]){
                    case CONSTRING : print_key += "%s";break;
                    case NUM : case BOOLEAN : print_key += "%lld";break;
                }
            }
            printf("\t.string \"%s\"\n", print_key.c_str());
        }
    }
    std::pair<int, long long> RiscV_generator(int nt, std::vector<std::unordered_map<std::string, int> > id_register, long long base_pos){
        if (obj_type.front() == CONSTRING){
            int cnt = opobj.size();
            for (int i = cnt-1; i >= 0 ; i--){
                std::pair<int, long long> ans_type = opobj[i]->RiscV_generator(nt, id_register, base_pos);
                switch(ans_type.first){
                    case -1 : case 1 : printf("\tli a%d,%lld\n", i , ans_type.second);break;
                    case 0 : printf("\tmv a%d,t0\n", i);break;
                    case 2 : printf("\tlla a%d,.LC%d\n", i, opobj[i]->get_lcid());break;
                }
            }
            printf("\tcall\tprintf@plt\n");
        }else{
            int cnt = opobj.size();
            for (int i = cnt; i > 0 ; i--){
                std::pair<int, long long> ans_type = opobj[i-1]->RiscV_generator(nt, id_register, base_pos);
                switch(ans_type.first){
                case -1 : case 1 : printf("\tli a%d,%lld\n", i , ans_type.second);break;
                case 0 : printf("\tmv a%d,t0\n", i);break;
                case 2 : printf("\tlla a%d,.LC%d\n", i, opobj[i-1]->get_lcid());break;
                }
            }
            printf("\tlla a0,.LC%lld\n", lcid);
            printf("\tcall\tprintf@plt\n");
        }
        return std::make_pair(0,0);
    }
};

class Inputnode : public Stmtnode{
    public:
    std::vector<std::unique_ptr<Lvaluenode> > ipobj;
    long long lcid;
    Inputnode():Stmtnode(INPUT){lcid = 0;}
    int type_checker(std::vector<std::unordered_map<std::string, std::pair<long long, long long> > > scope_table){
        std::vector<std::unique_ptr<Lvaluenode> >::iterator ite = ipobj.begin();
        while(ite != ipobj.end()){
            (*ite)->type_checker(scope_table);
            ite++;
        }
        return 0;
    }
    void getString(){
        if (string_counter == 0) printrodata();
        lcid = string_counter++;
        printf(".LC%lld:\n", lcid);
        std::string scan_key = "";
        for (int i = 1; i <= ipobj.size(); i++)
            scan_key += "%lld";
        printf("\t.string \"%s\"\n", scan_key.c_str());
    }
    std::pair<int, long long> RiscV_generator(int nt, std::vector<std::unordered_map<std::string, int> > id_register, long long base_pos){
        int cnt = ipobj.size();
        for (int i = cnt; i > 0 ; i--){
            printf("\taddi\ta%d,s0,%lld\n", i, (ipobj[i-1]->get_offset(id_register, base_pos)));
        }
        printf("\tlla a0,.LC%lld\n", lcid);
        printf("\tcall\tscanf@plt\n");
        return std::make_pair(0,0);
    }
};

class Returnnode : public Stmtnode{
    public:
    std::unique_ptr<Exprnode> son;
    Returnnode(std::unique_ptr<Exprnode> s):Stmtnode(RETURN), son(std::move(s)){}
    Returnnode():Stmtnode(RETURN), son(nullptr){}
    int type_checker(std::vector<std::unordered_map<std::string, std::pair<long long, long long> > > scope_table){
        return son->type_checker(scope_table);
    }
    void getString(){} // cannot return a const_string
    std::pair<int, long long> RiscV_generator(int nt, std::vector<std::unordered_map<std::string, int> > id_register, long long base_pos){
        std::pair<int, long long> ans_type = son->RiscV_generator(nt, id_register, base_pos);
        switch(ans_type.first){
            case -1 : case 1 : printf("\tli a0,%lld\n", ans_type.second);break;
            case 0 : printf("\tmv a0,t0\n");break;
            case 2 : throw Unmathced_Type_ERROR();
        }
        glob_return = true;
        return std::make_pair(0,0);
    }
};

class StmtBlocknode : public Stmtnode{
    public:
    std::vector<std::unique_ptr<Variablenode> > ValDel;
    std::vector<std::unique_ptr<Arraynode> > AryDel;
    std::vector<std::unique_ptr<Stmtnode> > stmtDel;
    StmtBlocknode():Stmtnode(STMTBLOCK){}
    int type_checker(std::vector<std::unordered_map<std::string, std::pair<long long, long long> > > scope_table){
        std::unordered_map<std::string, std::pair<long long, long long> > &block_scope = scope_table.back();
        std::vector<std::unique_ptr<Variablenode> >::iterator ite = ValDel.begin();
        while(ite != ValDel.end()){
            for (auto  i : block_scope){
                if (i.first == (*ite)->id) {
                    //std::cout<<i.first;
                    throw Redefinition_ERROR();
                }
            }
            block_scope[(*ite)->id] = std::make_pair((*ite)->type, 0);
            ite ++;
        }
        std::vector<std::unique_ptr<Arraynode> >::iterator aryite = AryDel.begin();
        while(aryite != AryDel.end()){
            for (auto  i : block_scope){
                if (i.first == (*aryite)->id) {
                    //std::cout<<i.first;
                    throw Redefinition_ERROR();
                }
            }
            block_scope[(*aryite)->id] = std::make_pair((*aryite)->type, (*aryite)->size);
            aryite ++;
        }
        scope_table.push_back(block_scope);
        std::unordered_map<std::string, std::pair<long long, long long> > m;
        scope_table.push_back(m);
        std::vector<std::unique_ptr<Stmtnode> >::iterator stmtite = stmtDel.begin();
        while(stmtite != stmtDel.end()){
            (*stmtite)->type_checker(scope_table);
            stmtite ++;
        }
        return 0;
    }
    std::pair<int, long long> RiscV_generator(int nt, std::vector<std::unordered_map<std::string, int> > id_register, long long base_pos){
        std::unordered_map<std::string, int> block_scope;
        long long ex_off = 0;
        std::vector<std::unique_ptr<Variablenode> >::iterator ite = ValDel.begin();
        while(ite != ValDel.end()){
            ex_off -= 8;
            block_scope[(*ite)->id] = ex_off + base_pos;
            ite ++;
        }
        std::vector<std::unique_ptr<Arraynode> >::iterator aryite = AryDel.begin();
        while(aryite != AryDel.end()){
            ex_off -= 8 * ((*aryite)->size);
            block_scope[(*aryite)->id] = ex_off + base_pos;
            aryite++;
        }
        if (ex_off != 0) printf("\taddi\tsp,sp,%lld\n", ex_off);
        id_register.push_back(block_scope);
        std::vector<std::unique_ptr<Stmtnode> >::iterator stmtite = stmtDel.begin();
        while(stmtite != stmtDel.end()){
            (*stmtite)->RiscV_generator(1, id_register, base_pos + ex_off);
            stmtite ++;
        }
        if (ex_off != 0) printf("\taddi\tsp,sp,%lld\n", (-1) * ex_off);
        return std::make_pair(0,0);
    }
    void getString(){
        std::vector<std::unique_ptr<Stmtnode> >::iterator stmtite = stmtDel.begin();
        while(stmtite != stmtDel.end()){
            (*stmtite)->getString();
            stmtite ++;
        }
    }
};

class Ifnode : public Stmtnode{
    public:
    std::unique_ptr<Exprnode> condition;
    std::unique_ptr<StmtBlocknode> body;
    std::unique_ptr<StmtBlocknode> else_body;
    Ifnode(std::unique_ptr<Exprnode> _condition, std::unique_ptr<StmtBlocknode> _body,
    std::unique_ptr<StmtBlocknode> _else_body):Stmtnode(IF), condition(std::move(_condition)),
    body(std::move(_body)), else_body(std::move(_else_body)){}
    int type_checker(std::vector<std::unordered_map<std::string, std::pair<long long, long long> > > scope_table){
        int condition_type = condition->type_checker(scope_table);
        if (condition_type != BOOLEAN) throw Unmathced_Type_ERROR();
        std::unordered_map<std::string, std::pair<long long, long long> > m;
        condition->get_identifier(scope_table, m);
        scope_table.push_back(m);
        body->type_checker(scope_table);
        if (else_body != nullptr) else_body->type_checker(scope_table);
        return 0;
    }
    std::pair<int, long long> RiscV_generator(int nt, std::vector<std::unordered_map<std::string, int> > id_register, long long base_pos){
        long long else_id = block_counter++;
        long long next_id = block_counter++;
        std::pair<int, long long> sjudge = condition->RiscV_generator(nt, id_register, else_id);
        if (sjudge.first == 1){
            if (sjudge.second == 1){
                body->RiscV_generator(nt, id_register, base_pos);
            }else{
                if (else_body != nullptr){
                    else_body->RiscV_generator(nt, id_register, base_pos);
                }
            }
        }else{
            body->RiscV_generator(nt, id_register, base_pos);
            printf("\tj  .L%lld\n", next_id);
            printf(".L%lld:\n", else_id);
            if (else_body != nullptr){
                else_body->RiscV_generator(nt, id_register, base_pos);
            }
            printf(".L%lld:\n", next_id); 
        }
        return std::make_pair(0,0);
    }
    void getString(){
        condition->getString();
        body->getString();
        if (else_body != nullptr) else_body->getString();
    }
};

class Whilenode : public Stmtnode{
    public:
    std::unique_ptr<Exprnode> ls;
    std::unique_ptr<StmtBlocknode> rs;
    Whilenode(std::unique_ptr<Exprnode> _ls, std::unique_ptr<StmtBlocknode> _rs):
    ls(std::move(_ls)), rs(std::move(_rs)), Stmtnode(WHILE){}
    int type_checker(std::vector<std::unordered_map<std::string, std::pair<long long, long long> > > scope_table){
        int condition_type = ls->type_checker(scope_table);
        if (condition_type != BOOLEAN) throw Unmathced_Type_ERROR();
        std::unordered_map<std::string, std::pair<long long, long long> > m;
        ls->get_identifier(scope_table, m);
        m["while"] = std::make_pair(0,0);
        scope_table.push_back(m);
        rs->type_checker(scope_table);
        return 0;
    }
    std::pair<int, long long> RiscV_generator(int nt, std::vector<std::unordered_map<std::string, int> > id_register, long long base_pos){
        long long loop_id = block_counter++;
        long long next_id = block_counter++;
        printf(".L%lld:\n", loop_id);
        std::pair<int, long long> sjudge = ls->RiscV_generator(nt, id_register, next_id);
        if (sjudge.first == 1 && sjudge.second == 0) return std::make_pair(0,0);//while(false)
        std::unordered_map<std::string, int> m;
        m["break"] = next_id;
        m["continue"] = loop_id;
        id_register.push_back(m);
        rs->RiscV_generator(nt, id_register, base_pos);
        printf("\tj  .L%lld\n", loop_id);
        printf(".L%lld:\n", next_id);
        return std::make_pair(0,0);
    }
    void getString(){
        ls->getString();
        rs->getString();
    }
};

class Fornode : public Stmtnode{
    public:
    std::unique_ptr<Exprnode> ini;
    std::unique_ptr<Exprnode> judge;
    std::unique_ptr<Exprnode> loop;
    std::unique_ptr<StmtBlocknode> body;
    Fornode(std::unique_ptr<Exprnode> _ini, std::unique_ptr<Exprnode> _judge, 
    std::unique_ptr<Exprnode> _loop, std::unique_ptr<StmtBlocknode> _body):ini(std::move(_ini)),
    judge(std::move(_judge)), loop(std::move(_loop)), body(std::move(_body)), Stmtnode(FOR){}
    int type_checker(std::vector<std::unordered_map<std::string, std::pair<long long, long long> > > scope_table){
        if (ini != nullptr) ini->type_checker(scope_table);
        if (loop != nullptr) loop->type_checker(scope_table);
        if (judge != nullptr){
            int condition_type = judge->type_checker(scope_table);
            if (condition_type != BOOLEAN) throw Unmathced_Type_ERROR();
        }
        std::unordered_map<std::string, std::pair<long long, long long> > m;
        if (ini != nullptr) ini->get_identifier(scope_table, m);
        if (loop != nullptr) loop->get_identifier(scope_table, m);
        if (judge != nullptr) judge->get_identifier(scope_table, m);
        m["for"] = std::make_pair(0,0);
        scope_table.push_back(m);
        body->type_checker(scope_table);
        return 0;
    }
    std::pair<int, long long> RiscV_generator(int nt, std::vector<std::unordered_map<std::string, int> > id_register, long long base_pos){
        long long loop_id = block_counter++;
        long long next_id = block_counter++;
        if (ini != nullptr) ini->RiscV_generator(nt, id_register, base_pos);//risk within
        printf(".L%lld:\n", loop_id);
        if (judge != nullptr){
            std::pair<int, long long> sjudge = judge->RiscV_generator(nt, id_register, next_id);
            if (sjudge.first == 1 && sjudge.second == 0) return std::make_pair(0,0);
        }
        std::unordered_map<std::string, int> m;
        m["break"] = next_id;
        m["continue"] = loop_id;
        id_register.push_back(m);
        body->RiscV_generator(nt, id_register, base_pos);
        if (loop != nullptr) loop->RiscV_generator(nt, id_register, base_pos);
        printf("\tj  .L%lld\n", loop_id);
        printf(".L%lld:\n", next_id);
        return std::make_pair(0,0);
    }
    void getString(){
        if (ini != nullptr) ini->getString();
        if (judge != nullptr) judge->getString();
        if (loop != nullptr) loop->getString();
        body->getString();
    }
};

class Continuenode : public Stmtnode{
    public:
    Continuenode():Stmtnode(CONTINUE){}
    //需要在for/while的符号表末尾多加一个特殊映射
    int type_checker(std::vector<std::unordered_map<std::string, std::pair<long long, long long> > > scope_table){
        std::vector<std::unordered_map<std::string, std::pair<long long, long long> > >::reverse_iterator outite = scope_table.rbegin();
        while(outite != scope_table.rend()){
            for (auto i : (*outite)){
                if (i.first == "while" || i.first == "for"){
                    return 0;
                }
            }
            outite++;
        }
        throw Invalid_Continue_ERROR();
    }
    std::pair<int, long long> RiscV_generator(int nt, std::vector<std::unordered_map<std::string, int> > id_register, long long base_pos){
        std::vector<std::unordered_map<std::string, int> >::reverse_iterator outite = id_register.rbegin();
        while(outite != id_register.rend()){
            for (auto i : (*outite)){
                if (i.first == "continue"){
                    printf("\tj  .L%d\n", i.second);
                    return std::make_pair(0,0);
                }
            }
            outite++;
        }
    }
    void getString(){}
};

class Breaknode : public Stmtnode{
    public:
    Breaknode():Stmtnode(BREAK){}
    int type_checker(std::vector<std::unordered_map<std::string, std::pair<long long, long long> > > scope_table){
        std::vector<std::unordered_map<std::string, std::pair<long long, long long> > >::reverse_iterator outite = scope_table.rbegin();
        while(outite != scope_table.rend()){
            for (auto i : (*outite)){
                if (i.first == "while" || i.first == "for"){
                    return 0;
                }
            }
            outite++;
        }
        throw Invalid_Break_ERROR();
    }
    std::pair<int, long long> RiscV_generator(int nt, std::vector<std::unordered_map<std::string, int> > id_register, long long base_pos){
        std::vector<std::unordered_map<std::string, int> >::reverse_iterator outite = id_register.rbegin();
        while(outite != id_register.rend()){
            for (auto i : (*outite)){
                if (i.first == "break"){
                    printf("\tj  .L%d\n", i.second);
                    return std::make_pair(0,0);
                }
            }
            outite++;
        }
    }
    void getString(){}
};

class Binopnode : public Exprnode{
    public :
    std::unique_ptr<Exprnode> ls;
    std::unique_ptr<Exprnode> rs;
    long long op;
    Binopnode(std::unique_ptr<Exprnode> _ls, std::unique_ptr<Exprnode> _rs, 
    Token tok): Exprnode(BINOP), ls(std::move(_ls)), rs(std::move(_rs)), op(tok.value){} 
    int type_checker(std::vector<std::unordered_map<std::string, std::pair<long long, long long> > > scope_table){
        int ltype = ls->type_checker(scope_table);
        int rtype = rs->type_checker(scope_table);
        if (ltype != rtype) throw Unmathced_Type_ERROR();
        switch(op){
            case ADD : case SUB : case MUL : case DIV : case BAND : case BOR : case XOR :
            return NUM;
            case ASSIGN : return ltype;
            default : break;
        }
        return BOOLEAN;
    }
    void get_identifier(std::vector<std::unordered_map<std::string, std::pair<long long, long long> > > scope_table, std::unordered_map<std::string, std::pair<long long, long long> > &m){
        ls->get_identifier(scope_table, m);
        rs->get_identifier(scope_table, m);
    }
    void NOT_traversal(){
        ls->NOT_traversal();
        rs->NOT_traversal();
        switch(op){
            case EQ : op = NOTEQ; return;
            case NOTEQ : op = EQ; return;
            case GE : op = LT; return;
            case LE : op = GT; return;
            case GT : op = LE; return;
            case LT : op = GE; return;
            case AND : op = OR; return;
            case OR : op = AND; return;
            default:return;
        }
    }
    std::pair<int, long long> RiscV_generator(int nt, std::vector<std::unordered_map<std::string, int> > id_register, long long base_pos){
        std::pair<int, long long> pre;
        std::pair<int, long long> pos;
        long long new_line = 0;
        if (op == ASSIGN){
            pre = ls->RiscV_generator(nt, id_register, base_pos);
            if (pre.first == 0) printf("\tmv t%d,t0\n", nt);
            pos = rs->RiscV_generator(nt+1, id_register, base_pos);
            if (pre.first == 2){
                printf("\tlla t%d,.LC%lld\n", nt, pre.second);
                printf("\tsd t%d,%lld(s0)\n", nt, pos.second);
                return std::make_pair(2, pre.second);
            }else if (pre.first != 0){
                printf("\tli t%d,%lld\n", nt, pre.second);
                printf("\tsd t%d,%lld(s0)\n",nt,  pos.second);
                return std::make_pair(pre.first, pre.second);
            }else{
                printf("\tsd t%d,%lld(s0)\n",nt,  pos.second);
                printf("\tld t0,%lld(s0)\n",  pos.second);
                return std::make_pair(0, pos.second);
            }
        }
        
        if (op == OR){
            new_line = block_counter++;
            pre = ls->RiscV_generator(nt, id_register, new_line);
            printf("\tj  .L%lld\n", base_pos+1);
            printf(".L%lld:\n", new_line);
        }else pre = ls->RiscV_generator(nt, id_register, base_pos);
        long long arg1 = pre.first;
        if (pre.first != 0) arg1 = pre.second;
        else printf("\tmv t%d,t0\n", nt);
        pos = rs->RiscV_generator(nt+1, id_register, base_pos);
        long long arg2 = pos.first;
        if (pos.first != 0) arg2 = pos.second;
        else printf("\tmv t%d,t0\n", nt+1);
        //IR ir(op, arg1 , arg2);
        //ir.print_CIR();

        switch(op){
            case EQ : if (pre.first != 0 && pos.first != 0) return std::make_pair(1,cal_cc(op, arg1, arg2));
                      if (pre.first != 0) printf("\tli t%d,%lld\n", nt, pre.second);
                      if (pos.first != 0) printf("\tli t%d,%lld\n", nt+1, pos.second);
                        printf("\tbne t%d,t%d,.L%lld\n",nt, nt+1, base_pos);return std::make_pair(-1,0); //base_pos = else_id
            case NOTEQ : if (pre.first != 0 && pos.first != 0) return std::make_pair(1,cal_cc(op, arg1, arg2));
                      if (pre.first != 0) printf("\tli t%d,%lld\n", nt, pre.second);
                      if (pos.first != 0) printf("\tli t%d,%lld\n", nt+1, pos.second);
                        printf("\tbeq t%d,t%d,.L%lld\n",nt, nt+1, base_pos);return std::make_pair(-1,0);  
            case GT : if (pre.first != 0 && pos.first != 0) return std::make_pair(1,cal_cc(op, arg1, arg2));
                      if (pre.first != 0) printf("\tli t%d,%lld\n", nt, pre.second);
                      if (pos.first != 0) printf("\tli t%d,%lld\n", nt+1, pos.second);
                        printf("\tble t%d,t%d,.L%lld\n",nt, nt+1, base_pos);return std::make_pair(-1,0); 
            case LT : if (pre.first != 0 && pos.first != 0) return std::make_pair(1,cal_cc(op, arg1, arg2));
                      if (pre.first != 0) printf("\tli t%d,%lld\n", nt, pre.second);
                      if (pos.first != 0) printf("\tli t%d,%lld\n", nt+1, pos.second);
                        printf("\tbge t%d,t%d,.L%lld\n",nt, nt+1, base_pos);return std::make_pair(-1,0); 
            case GE : if (pre.first != 0 && pos.first != 0) return std::make_pair(1,cal_cc(op, arg1, arg2));
                      if (pre.first != 0) printf("\tli t%d,%lld\n", nt, pre.second);
                      if (pos.first != 0) printf("\tli t%d,%lld\n", nt+1, pos.second);
                        printf("\tblt t%d,t%d,.L%lld\n",nt, nt+1, base_pos);return std::make_pair(-1,0); 
            case LE : if (pre.first != 0 && pos.first != 0) return std::make_pair(1,cal_cc(op, arg1, arg2));
                      if (pre.first != 0) printf("\tli t%d,%lld\n", nt, pre.second);
                      if (pos.first != 0) printf("\tli t%d,%lld\n", nt+1, pos.second);
                        printf("\tbgt t%d,t%d,.L%lld\n",nt, nt+1, base_pos);return std::make_pair(-1,0); 
            case AND : case OR : return std::make_pair(-1,0);
            default : break;
        }

        if (pre.first == 0 && pos.first == 0){
            printBinop(op, nt);
            return std::make_pair(0,0);
        }else if (pre.first != 0 && pos.first == 0){
            printBinop(op, arg1 , 2 ,nt);
            return std::make_pair(0,0);
        }else if (pre.first == 0 && pos.first != 0){
            printBinop(op, arg2 , 1, nt);
            return std::make_pair(0,0);
        }else if (pre.first != 0 && pos.first != 0){
            return std::make_pair(-1,cal_cc(op,arg1, arg2));
        }
        //if (op == ASSIGN) return std::make_pair(-1, arg1);
        //else return std::make_pair(ir.re_index, -1);
    }
    void getString(){
        ls->getString();
        rs->getString();
    }
    int get_lcid(){return -1;}
};

class Unopnode : public Exprnode{
    public:
    std::unique_ptr<Exprnode> child;
    long long op;
    Unopnode(std::unique_ptr<Exprnode> _child, Token tok):Exprnode(UNOP), 
    child(std::move(_child)),op(tok.value){}
    int type_checker(std::vector<std::unordered_map<std::string, std::pair<long long, long long> > > scope_table){
        int subtype = child->type_checker(scope_table);
        switch(op){
            case NEG : case COMPL : if (subtype == NUM) return NUM;else break;
            case NOT : if (subtype == BOOLEAN) return BOOLEAN;else break;
        }
        throw Unmathced_Type_ERROR();
    }
    void get_identifier(std::vector<std::unordered_map<std::string, std::pair<long long, long long> > > scope_table, std::unordered_map<std::string, std::pair<long long, long long> > &m){
        child->get_identifier(scope_table, m);
    }
    void NOT_traversal(){}
    std::pair<int, long long> RiscV_generator(int nt, std::vector<std::unordered_map<std::string, int> > id_register, long long base_pos){
        if (op == NOT){
            child->NOT_traversal();
            std::pair<int, long long> chvalue = child->RiscV_generator(nt, id_register, base_pos);
            if (chvalue.first == 1) return std::make_pair(1, cal_c(op, chvalue.second));
            else{
                /*
                printf("\tmv t1,t0\n");
                printUnop(op);*/
                return std::make_pair(0,0);
            }
        }

        std::pair<int, long long> subvalue = child->RiscV_generator(nt, id_register, base_pos);
        if (subvalue.first != 0){
            return std::make_pair(-1, cal_c(op, subvalue.second));
        }else{
            printf("\tmv t%d,t0\n", nt);
            printUnop(op, nt);
            return std::make_pair(0,0);
        }
    }
    void getString(){
        child->getString();
    }
    int get_lcid(){return -1;}
};

class Constnode : public Exprnode{
    public : 
    Constnode(Tokentype t) : Exprnode(t){}
    void get_identifier(std::vector<std::unordered_map<std::string, std::pair<long long, long long> > > scope_table, std::unordered_map<std::string, std::pair<long long, long long> > &m){}
    void NOT_traversal(){}
};

class intConstnode : public Constnode{
    public:
    long long value;
    intConstnode(Token tok):Constnode(NUM), value(tok.value){}
    int type_checker(std::vector<std::unordered_map<std::string, std::pair<long long, long long> > > scope_table){
        return NUM;
    }
    std::pair<int, long long> RiscV_generator(int nt, std::vector<std::unordered_map<std::string, int> > id_register, long long base_pos){
        return std::make_pair(-1,value);
    }
    void getString(){}
    int get_lcid(){return -1;}
};

class boolConstnode : public Constnode{
    public :
    bool value;
    boolConstnode(Token tok):Constnode(BOOLEAN), value(tok.value){}
    int type_checker(std::vector<std::unordered_map<std::string, std::pair<long long, long long> > > scope_table){
        return BOOLEAN;
    }
    std::pair<int, long long> RiscV_generator(int nt, std::vector<std::unordered_map<std::string, int> > id_register, long long base_pos){
        return std::make_pair(1,value);
    }
    void getString(){}
    int get_lcid(){return -1;}
};

class stringConstnode : public Constnode{
    public :
    std::string content;
    long long lcid;
    stringConstnode(Token tok):Constnode(CONSTRING), content(tok.key){lcid = 0;}
    int type_checker(std::vector<std::unordered_map<std::string, std::pair<long long, long long> > > scope_table){
        return CONSTRING;
    }
    std::pair<int, long long> RiscV_generator(int nt, std::vector<std::unordered_map<std::string, int> > id_register, long long base_pos){
        return std::make_pair(2, lcid); // 2 means string
    }
    void getString(){
        if (string_counter == 0) printrodata();
        lcid = string_counter++;
        printf(".LC%lld:\n", lcid);
        printf("\t.string %s\n", content.c_str());
    }
    int get_lcid(){return lcid;}
};

class Lvariablenode : public Lvaluenode{
    public:
    std::string _id;
    long long off_set;
    Lvariablenode(Token tok):Lvaluenode(ID),_id(tok.key){off_set = -1;}
    int type_checker(std::vector<std::unordered_map<std::string, std::pair<long long, long long> > > scope_table){
        std::vector<std::unordered_map<std::string, std::pair<long long, long long> > >::reverse_iterator outite = scope_table.rbegin();
        while(outite != scope_table.rend()){
            for (auto i : (*outite)){
                if (i.first == _id){
                    if (i.second.first == _INT) return NUM;
                    else if (i.second.first == _BOOL) return BOOLEAN;
                    else if (i.second.first == _STRING) return CONSTRING;
                }
            }
            outite++;
        }
        throw Undefined_Identifier_ERROR();
    }
    void get_identifier(std::vector<std::unordered_map<std::string, std::pair<long long, long long> > > scope_table, std::unordered_map<std::string, std::pair<long long, long long> > &m){
        std::vector<std::unordered_map<std::string, std::pair<long long, long long> > >::reverse_iterator outite = scope_table.rbegin();
        while(outite != scope_table.rend()){
            for (auto i : (*outite)){
                if (i.first == _id){
                    m[i.first] = i.second;
                    return;
                }
            }
            outite++;
        }
    }
    std::pair<int, long long> RiscV_generator(int nt, std::vector<std::unordered_map<std::string, int> > id_register, long long base_pos){
        if (off_set >= 0) return std::make_pair(0,off_set);
        else{
        std::vector<std::unordered_map<std::string, int> >::reverse_iterator ite = id_register.rbegin();
        while(ite != id_register.rend()){
            for (auto i : (*ite)){
                if (i.first == _id){
                    printf("\tld t0,%d(s0)\n", i.second);
                    off_set = i.second;
                    return std::make_pair(0, i.second); //(register, offset)
                }
            }
            ite++;
        }
        }
    }
    long long get_offset(std::vector<std::unordered_map<std::string, int> > id_register, long long base_pos){
        if (off_set >= 0) return off_set;
        else{
        std::vector<std::unordered_map<std::string, int> >::reverse_iterator ite = id_register.rbegin();
        while(ite != id_register.rend()){
            for (auto i : (*ite)){
                if (i.first == _id){
                    //printf("\tld t0,%d(s0)\n", i.second);
                    off_set = i.second;
                    return i.second; 
                }
            }
            ite++;
        }
        }
    }
};

class Larraynode : public Lvaluenode{
    public: 
    long long _pos;
    std::string _id;
    long long off_set;
    Larraynode(Token id_tok, Token size_tok):Lvaluenode(ID), _id(id_tok.key), _pos(size_tok.value){off_set = -1;}
    int type_checker(std::vector<std::unordered_map<std::string, std::pair<long long, long long> > > scope_table){
        std::vector<std::unordered_map<std::string, std::pair<long long, long long> > >::reverse_iterator outite = scope_table.rbegin();
        while(outite != scope_table.rend()){
            for (auto i : (*outite)){
                if (i.first == _id){
                    if (_pos < 0 || _pos >= i.second.second) throw Out_Of_Boundary_ERROR();
                    if (i.second.first == INT_ARRAY) return NUM;
                    else if (i.second.first == BOOL_ARRAY) return BOOLEAN;
                    else if (i.second.first == STRING_ARRAY) return CONSTRING;
                }
            }
            outite++;
        }
        throw Undefined_Identifier_ERROR();
    }
    void get_identifier(std::vector<std::unordered_map<std::string, std::pair<long long, long long> > > scope_table, std::unordered_map<std::string, std::pair<long long, long long> > &m){
        std::vector<std::unordered_map<std::string, std::pair<long long, long long> > >::reverse_iterator outite = scope_table.rbegin();
        while(outite != scope_table.rend()){
            for (auto i : (*outite)){
                if (i.first == _id){
                    m[i.first] = i.second;
                    return;
                }
            }
            outite++;
        }
    }
    std::pair<int, long long> RiscV_generator(int nt, std::vector<std::unordered_map<std::string, int> > id_register, long long base_pos){
        if (off_set >= 0) return std::make_pair(0,off_set);
        else{
        std::vector<std::unordered_map<std::string, int> >::reverse_iterator ite = id_register.rbegin();
        while(ite != id_register.rend()){
            for (auto i : (*ite)){
                if (i.first == _id){
                    printf("\tld t0,%lld(s0)\n", (i.second + 8*_pos));
                    off_set = (i.second + 8*_pos);
                    return std::make_pair(0, (i.second + 8*_pos));
                }
            }
            ite++;
        }
        }
    }
    long long get_offset(std::vector<std::unordered_map<std::string, int> > id_register, long long base_pos){
        if (off_set >= 0) return off_set;
        else{
        std::vector<std::unordered_map<std::string, int> >::reverse_iterator ite = id_register.rbegin();
        while(ite != id_register.rend()){
            for (auto i : (*ite)){
                if (i.first == _id){
                    //printf("\tld t0,%lld(s0)\n", (i.second + 8*_pos));
                    off_set = (i.second + 8*_pos);
                    return off_set;
                }
            }
            ite++;
        }
        }
    }
};


#endif