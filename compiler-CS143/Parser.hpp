#ifndef SEAL_PARSER_HPP
#define SEAL_PARSER_HPP

#include <iostream>
#include <memory>
#include <utility>
#include <unordered_map>
#include <stack>
#include "Lexer.hpp"
#include "AST.hpp"
#include "SEALexception.hpp"

// A parser implemented based on recursive dencent algorithm

class Parser{
    public:
    std::vector<Token> token_sq;
    std::vector<Token>::iterator next;
    Parser(std::vector<Token> toklis):token_sq(toklis){
        next = token_sq.begin();
        getPrecedence();
    }

    bool term(Tokentype tok , Token &eva){
        //return (*next++).type == tok;
        if ((*next).type == tok){
            eva = *next;
            next++;
            return true;
        }else return false;
    }

    std::unique_ptr<Variablenode> VariableDecl(){
        std::vector<Token>::iterator save = next;
        Token get_id;
        Token get_type;
        Token temp;
        if (term(VAR, temp) && term(ID, get_id) && 
        term(TYPE, get_type) && term(SEMICOLON, temp)){
            return std::make_unique<Variablenode>(get_id, get_type);
        }else{
            next = save;
            return nullptr;
        }
    }

    //Array is not the same category of Variable
    std::unique_ptr<Arraynode> ArrayDecl(){
        std::vector<Token>::iterator save = next;
        Token temp;
        Token get_id;
        Token get_type;
        Token get_size;
        if (term(VAR, temp) && term(ID, get_id) && term(LSB,temp)
        && term(NUM, get_size) && term(RSB, temp) && term(TYPE, get_type)){
            return std::make_unique<Arraynode>(get_id, get_type, get_size);
        }else{
            next = save;
            return nullptr;
        }
    }

    // Auxiliary function for Exprnode
    bool isExpr(std::unique_ptr<Exprnode> &p){
        p = ExprStmt(); 
        return (p != nullptr);
    }

    std::unique_ptr<Lvaluenode> getLvalue(){
        std::vector<Token>::iterator save = next;
        //Larray
        Token get_size, temp;
        Token get_id;
        std::unique_ptr<Exprnode> p = nullptr;
        if (term(ID, get_id) && term(LSB, temp) && term(NUM, get_size) && term(RSB, temp)){
            return std::make_unique<Larraynode>(get_id, get_size);
        }else next = save;
        if (term(ID, get_id) && term(LSB, temp) && isExpr(p) && term(RSB, temp)){
            return std::make_unique<Larraynode>(get_id, std::move(p));
        }else next = save;
        //Lvariable
        if (term(ID, get_id)){
            return std::make_unique<Lvariablenode>(get_id);
        }else next = save;

        return nullptr;
    }

    bool isLvalue(std::unique_ptr<Lvaluenode> &p){
        p = getLvalue();
        return (p != nullptr);
    }

    std::unique_ptr<Constnode> getConst(){
        std::vector<Token>::iterator save = next;
        Token temp;
        if (term(NUM, temp)){
            auto icp = std::make_unique<intConstnode>(temp);
            return std::move(icp);
        }else next = save;
        if (term(BOOLEAN, temp)){
            auto bcp = std::make_unique<boolConstnode>(temp);
            return std::move(bcp);
        }else next = save;
        if (term(CONSTRING, temp)){
            auto scp = std::make_unique<stringConstnode>(temp);
            return std::move(scp);
        }else next = save;
        return nullptr;
    }

    std::unique_ptr<Exprnode> getargument(){
        std::vector<Token>::iterator save = next;
        Token temp;
        //Const
        auto cp = getConst();
        if (cp != nullptr) return std::move(cp);
        else next = save;
        //Lvalue
        auto lp = getLvalue();
        if (lp != nullptr) return std::move(lp);
        else next = save;

        return nullptr;
    }

    std::unique_ptr<Unopnode> getUnop(){
        std::vector<Token>::iterator save = next;
        Token temp;
        if (term(UNOP, temp)){
            auto p = getargument();
            if (p != nullptr){
                return std::make_unique<Unopnode>(std::move(p), temp);
            }else{
                next = save;
                return nullptr;
            }
        }else{
            next = save;
            return nullptr;
        }
    }

    std::unordered_map<long long, int> Precedence;
    void getPrecedence(){
        Precedence[OR] = 1;
        Precedence[AND] = 2;
        Precedence[EQ] = 3;
        Precedence[NOTEQ] = 3;
        Precedence[LT] = 4;
        Precedence[GT] = 4;
        Precedence[LE] = 4;
        Precedence[GE] = 4;
        Precedence[SUB] = 5;
        Precedence[ADD] = 5;
        Precedence[MUL] = 6;
        Precedence[DIV] = 6;
        Precedence[NEG] = 7;
        Precedence[NOT] = 7;
        Precedence[COMPL] = 8;
        Precedence[BAND] = 8;
        Precedence[BOR] = 8;
        Precedence[XOR] = 8;
    }

    std::unique_ptr<Exprnode> getExpr(){
        std::vector<Token>::iterator save = next;
        std::stack<std::unique_ptr<Exprnode> > node_stack;
        std::stack<Token> symbol_stack;
        //node_stack.push(std::move(head));
        Token get_op, temp;
        while(1){
            if (term(BINOP, get_op)){
                if (get_op.value == ASSIGN) throw Lvalue_ERROR();
                while(!(symbol_stack.empty()) && symbol_stack.top().type != LBT && 
                Precedence[get_op.value] < Precedence[symbol_stack.top().value]){
                    temp = symbol_stack.top();
                    symbol_stack.pop();
                    if (temp.type == BINOP){
                        auto rs = std::move(node_stack.top());
                        node_stack.pop();
                        auto ls = std::move(node_stack.top());
                        node_stack.pop();
                        auto bp = std::make_unique<Binopnode>(std::move(ls), std::move(rs), temp);
                        node_stack.push(std::move(bp));
                    }else if (temp.type == UNOP){
                        auto ch = std::move(node_stack.top());
                        node_stack.pop();
                        auto up = std::make_unique<Unopnode>(std::move(ch), temp);
                        node_stack.push(std::move(up));
                    }
                }
                symbol_stack.push(get_op);continue;
            }
            if (term(UNOP, get_op)){
                while(!(symbol_stack.empty()) && symbol_stack.top().type != LBT &&  
                Precedence[get_op.value] < Precedence[symbol_stack.top().value]){
                    temp = symbol_stack.top();
                    symbol_stack.pop();
                    if (temp.type == BINOP){
                        auto rs = std::move(node_stack.top());
                        node_stack.pop();
                        auto ls = std::move(node_stack.top());
                        node_stack.pop();
                        auto bp = std::make_unique<Binopnode>(std::move(ls), std::move(rs), temp);
                        node_stack.push(std::move(bp));
                    }else if (temp.type == UNOP){
                        auto ch = std::move(node_stack.top());
                        node_stack.pop();
                        auto up = std::make_unique<Unopnode>(std::move(ch), temp);
                        node_stack.push(std::move(up));
                    }
                }
                symbol_stack.push(get_op);continue;
            }
            if (term(LBT, temp)){
                symbol_stack.push(temp);continue;
            }
            if (term(RBT, temp)){
                //int x = symbol_stack.size();
                while((!(symbol_stack.empty())) && symbol_stack.top().type != LBT){
                    temp = symbol_stack.top();
                    symbol_stack.pop();
                    if (temp.type == BINOP){
                        auto rs = std::move(node_stack.top());
                        node_stack.pop();
                        auto ls = std::move(node_stack.top());
                        node_stack.pop();
                        auto bp = std::make_unique<Binopnode>(std::move(ls), std::move(rs), temp);
                        node_stack.push(std::move(bp));
                    }else if (temp.type == UNOP){
                        auto ch = std::move(node_stack.top());
                        node_stack.pop();
                        auto up = std::make_unique<Unopnode>(std::move(ch), temp);
                        node_stack.push(std::move(up));
                    }
                }
                //x = symbol_stack.size();
                if (symbol_stack.empty()) throw Parentheses_Unmatched_ERROR();
                symbol_stack.pop();
                continue;
            }
            auto ap = getargument();
            if (ap != nullptr){
                node_stack.push(std::move(ap));continue;
            }
            break;
        }
        //int x = symbol_stack.size();
        //int y = node_stack.size();
        while(!(symbol_stack.empty())){
            temp = symbol_stack.top();
            symbol_stack.pop();
            if (temp.type == LBT) throw Parentheses_Unmatched_ERROR();
            if (temp.type == BINOP){
                auto rs = std::move(node_stack.top());
                node_stack.pop();
                auto ls = std::move(node_stack.top());
                node_stack.pop();
                auto bp = std::make_unique<Binopnode>(std::move(ls), std::move(rs), temp);
                node_stack.push(std::move(bp));
            }else if (temp.type == UNOP){
                auto ch = std::move(node_stack.top());
                node_stack.pop();
                auto up = std::make_unique<Unopnode>(std::move(ch), temp);
                node_stack.push(std::move(up));
            }
        }
        if (node_stack.empty()){
            next = save;
            return nullptr;
        }
        return std::move(node_stack.top());
    }
        
    std::unique_ptr<Exprnode> ExprStmt(){
        std::vector<Token>::iterator save = next;
        //(Expr)
        Token temp;
        std::unique_ptr<Exprnode> p = nullptr;
        /*
        if (term(LBT, temp) && isExpr(p) && term(RBT, temp))
            return std::move(p);
        else next = save;
        */  // dumped code since getExpr is strong enough
        //Lvalue = Expr
        std::unique_ptr<Lvaluenode> lp;
        std::unique_ptr<Exprnode> q = nullptr;
        if (isLvalue(lp) && term(BINOP, temp) && (temp.value == ASSIGN) && isExpr(q)){
            auto bp = std::make_unique<Binopnode>(std::move(q), std::move(lp), temp);
            return std::move(bp);
        }else next = save;
        /*
        //Unop
        auto up = getUnop();
        if (up != nullptr) return std::move(getExpr(std::move(up)));
        else next = save;
        //Lvalue/intConst
        auto ap = getargument();
        if (ap != nullptr) return std::move(getExpr(std::move(ap)));
        else next = save;
        */
        //All in one
        auto ep = getExpr();
        if (ep != nullptr) return std::move(ep);
        else next = save;

        return nullptr;

    }

    std::unique_ptr<Returnnode> ReturnStmt(){
        std::vector<Token>::iterator save = next;
        Token temp;
        std::unique_ptr<Exprnode> p;
        if (term(RETURN, temp) && isExpr(p) && term(SEMICOLON, temp)){
            return std::make_unique<Returnnode>(std::move(p));
        }else next = save;
        if (term(RETURN, temp) && term(SEMICOLON, temp)){
            return std::make_unique<Returnnode>();
        }else next = save;

        return nullptr;
    }

    std::unique_ptr<Printnode> PrintStmt(){
        std::vector<Token>::iterator save = next;
        Token temp;
        std::unique_ptr<Exprnode> p;
        if (term(PRINT, temp) && isExpr(p)){
            auto oplis = std::make_unique<Printnode>();
            oplis->opobj.push_back(std::move(p));
            std::vector<Token>::iterator savep = next;
            while(term(COMMA, temp) && isExpr(p)){
                savep = next;
                oplis->opobj.push_back(std::move(p));
            }
            next = savep;
            if (term(SEMICOLON, temp)){
                return std::move(oplis);
            }else{
                next = save;
                return nullptr;
            }
        }else{
            next = save;
            return nullptr;
        }
    }

    std::unique_ptr<Inputnode> InputStmt(){
        std::vector<Token>::iterator save = next;
        Token temp;
        std::unique_ptr<Lvaluenode> p;
        if (term(INPUT, temp) && isLvalue(p)){
            auto iplis = std::make_unique<Inputnode>();
            iplis->ipobj.push_back(std::move(p));
            std::vector<Token>::iterator savep = next;
            while(term(COMMA, temp) && isLvalue(p)){
                savep = next;
                iplis->ipobj.push_back(std::move(p));
            }
            next = savep;
            if (term(SEMICOLON, temp)){
                return std::move(iplis);
            }else{
                next = save;
                return nullptr;
            }
        }else{
            next = save;
            return nullptr;
        }
    }
    
    std::unique_ptr<StmtBlocknode> StmtBlock(){
        std::vector<Token>::iterator save = next;
        Token tok;
        if(term(LBC, tok)){
        auto temp = std::make_unique<StmtBlocknode>();
            while (1){
                auto p = VariableDecl();
                if (p == nullptr) break;
                else temp->ValDel.push_back(std::move(p));
            }
            while(1){
                auto q = ArrayDecl();
                if (q == nullptr) break;
                else temp->AryDel.push_back(std::move(q));
            }
            while(1){
                auto sp = Statement();
                if (sp == nullptr) break;
                else temp->stmtDel.push_back(std::move(sp));
            }
            if (term(RBC, tok)){
                return std::move(temp);
            }else {
                next = save;
                return nullptr;
            }
        }else{
            next = save;
            return nullptr;
        }
    }
    // Auxiliary function for StmtBlocknode 
    bool isStmtBlock(std::unique_ptr<StmtBlocknode> &p){
        p = StmtBlock();
        return (p != nullptr);
    }

    std::unique_ptr<Whilenode> WhileStmt(){
        std::vector<Token>::iterator save = next;
        Token temp;
        std::unique_ptr<Exprnode> p;
        std::unique_ptr<StmtBlocknode> q;
        if (term(WHILE, temp) && isExpr(p) && isStmtBlock(q)){
                return std::make_unique<Whilenode>(std::move(p), std::move(q));
        }else{
            next = save;
            return nullptr;
        }
    }
    
    std::unique_ptr<Fornode> ForStmt(){
        std::vector<Token>::iterator save = next;
        Token temp;
        std::unique_ptr<StmtBlocknode> _body;
        if (term(FOR, temp)){
            auto _ini = ExprStmt();
            if (term(SEMICOLON, temp)){
                auto _judge = ExprStmt();
                if (term(SEMICOLON, temp)){
                    auto _loop = ExprStmt();
                    if (isStmtBlock(_body)){
                        return std::make_unique<Fornode>(std::move(_ini), std::move(_judge),
                        std::move(_loop), std::move(_body));
                    }else{
                        next = save;
                        return nullptr;
                    }
                }else{
                    next = save;
                    return nullptr;
                }
            }else{
                next = save;
                return nullptr;
            }
        }else{
            next = save;
            return nullptr;
        }
    }

    std::unique_ptr<Ifnode> IfStmt(){
        std::vector<Token>::iterator save = next;
        Token temp;
        std::unique_ptr<Exprnode> p;
        std::unique_ptr<StmtBlocknode> q, eq;
        if (term(IF, temp) && isExpr(p) && isStmtBlock(q)){
            std::vector<Token>::iterator savep = next;
            if (term(ELSE, temp) && isStmtBlock(eq)){
                return std::make_unique<Ifnode>(std::move(p),std::move(q),std::move(eq));
            }else{
                next = savep;
                return std::make_unique<Ifnode>(std::move(p),std::move(q),nullptr);
            }
        }else{
            next = save;
            return nullptr;
        }
    }

    std::unique_ptr<Breaknode> BreakStmt(){
        std::vector<Token>::iterator save = next;
        Token temp;
        if (term(BREAK, temp) && term(SEMICOLON, temp)) return std::make_unique<Breaknode>();
        else{
            next = save;
            return nullptr;
        }
    }

    std::unique_ptr<Continuenode> ContinueStmt(){
        std::vector<Token>::iterator save = next;
        Token temp;
        if (term(CONTINUE, temp) && term(SEMICOLON, temp)) return std::make_unique<Continuenode>();
        else{
            next = save;
            return nullptr;
        }
    }

    std::unique_ptr<Stmtnode> Statement(){
        std::vector<Token>::iterator save = next;
        Token temp;
        std::unique_ptr<Exprnode> p;
        if (isExpr(p) && term(SEMICOLON, temp)) return std::move(p);
        else next = save;
        auto q = IfStmt();
        if (q != nullptr) return std::move(q);
        auto wp = WhileStmt();
        if (wp != nullptr) return std::move(wp);
        auto fp = ForStmt();
        if (fp != nullptr) return std::move(fp);
        auto bp = BreakStmt();
        if (bp != nullptr) return std::move(bp);
        auto cp = ContinueStmt();
        if (cp != nullptr) return std::move(cp);
        auto rp = ReturnStmt();
        if (rp != nullptr) return std::move(rp);
        auto pp = PrintStmt();
        if (pp != nullptr) return std::move(pp);
        auto ip = InputStmt();
        if (ip != nullptr) return std::move(ip);
        auto sp = StmtBlock();
        if (sp != nullptr) return std::move(sp);

        return nullptr;
    }

    std::unique_ptr<Programnode> buildAST(){
        auto temp = std::make_unique<Programnode>();
            while (1){
                auto p = VariableDecl();
                if (p != nullptr){
                    temp->ValDel.push_back(std::move(p));
                    continue;
                }
                auto q = ArrayDecl();
                if (q != nullptr){
                    temp->AryDel.push_back(std::move(q));
                    continue;
                }
                break;
            }
            
            while(1){
                auto sp = Statement();
                if (sp == nullptr) break;
                else temp->stmtDel.push_back(std::move(sp));
            }
            
        if ((*next).type != _EOF){
            //std::cout<<"FAULT!!\n";
            throw Syntactic_ERROR();
        }else {
            //std::cout<<"SUCCEED!!\n";
            return std::move(temp);
        }
    }

};






#endif