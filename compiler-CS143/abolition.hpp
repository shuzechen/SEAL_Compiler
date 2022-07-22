    // An implement of Parser based on changing the grammar
    // However, the precedence problem cannot be fixed. 
    std::unique_ptr<Exprnode> ExprStmt(){
        std::vector<Token>::iterator save = next;
        //(Expr)
        Token temp;
        std::unique_ptr<Exprnode> p = nullptr;
        if (term(LBT, temp) && isExpr(p) && term(RBT, temp)){
            auto prim = matchExprprime();
            if (prim.second != nullptr){
                auto q = std::make_unique<Binopnode>
                (std::move(p), std::move(prim.second), prim.first);
                return std::move(q);
            }else{
                return std::move(p);
            }
        }else next = save;
        //intConst
        if (term(NUM, temp)){
            auto icp = std::make_unique<intConstnode>(temp);
            auto prim = matchExprprime();
            if (prim.second != nullptr){
                auto q = std::make_unique<Binopnode>
                (std::move(icp), std::move(prim.second), prim.first);
                return std::move(q);
            }else{
                return std::move(icp);
            }
        }else next = save;
        //Lvariable
        Token get_id;
        if (term(ID, get_id)){
            return std::make_unique
        }else next = save;
        //Larray

        return nullptr;
    }

    std::pair<Token, std::unique_ptr<Exprnode> > matchExprprime(){
        std::vector<Token>::iterator save = next;
        Token get_op;
        std::unique_ptr<Exprnode> p = nullptr;
        if (term(BINOP, get_op) && isExpr(p)){
                auto temp = matchExprprime();
                if(temp.second == nullptr)
                    return std::make_pair(get_op, std::move(p));
                else{
                    auto q = std::make_unique<Binopnode>
                    (std::move(p), std::move(temp.second), temp.first);
                    return std::make_pair(get_op, std::move(q));
                }
        }else{
            next = save;
            return std::make_pair(Token(ID),nullptr);
        }
    }