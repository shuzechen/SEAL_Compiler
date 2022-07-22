#ifndef CONTROL_FLOW_HPP
#define CONTROL_FLOW_HPP

#include <vector>
#include <map>

enum Statustype{
    _BOTTOM = 0, _CONSTANT = 1, _TOP = 2
};

class Status{
    public : 
    Statustype type;
    long long value;
    Status(Statustype _type, long long v = 0):type(_type),value(v){}
    Status():type(_TOP), value(0){}
    Status &operator = (const Status &rhs){
        type = rhs.type;
        value = rhs.value;
        return (*this);
    }
};

void swap_status(Status a, Status b){
    Status temp(_TOP);
    temp = a;
    a = b;
    b = temp;
    
}

Status lub(Status a, Status b){
    if (a.type == b.type){
        if (a.type == _CONSTANT){
            if (a.value == b.value) return a;
            else return Status(_TOP);
        }else return a;
    }else{
        if (a.type < b.type) swap_status(a,b);
        if (a.type == _CONSTANT) return a;
        else return Status(_TOP);
    }
}

class data_flow{
    public:
    std::vector<std::map<std::string, Status> > flow_table;
};

data_flow renew_flow(data_flow a, data_flow b){
    std::vector<std::map<std::string, Status> >::iterator aite = a.flow_table.begin();
    std::vector<std::map<std::string, Status> >::iterator bite = b.flow_table.begin();
    data_flow temp;
    while(aite != a.flow_table.end() && bite != b.flow_table.end()){
        std::map<std::string, Status> m;
        for (auto i = (*aite).begin(), j = (*bite).begin(); i != (*aite).end(); i++, j++){
            m[i->first] = lub(i->second, j->second);
        }
        temp.flow_table.push_back(m);
        aite++;
        bite++;
    }
    return temp;
}

#endif