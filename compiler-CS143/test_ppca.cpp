#include <string>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <memory>
#include <utility>
#include <stack>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <map>
//#include "SEALexception.hpp"
//#include "Chen_IR.hpp"
//#include "Lexer.hpp"
using namespace std;

unique_ptr<int> test_unique_ptr1();
unique_ptr<int> test_unique_ptr(unique_ptr<int> s){
    unique_ptr<int> p = nullptr;
    //auto p = make_unique<int>(5);
    return std::move(test_unique_ptr1());
}

unique_ptr<int> test_unique_ptr1(){
    auto p = make_unique<int>(5);
    stack<unique_ptr<int> > s;
    s.push(std::move(p));
    return std::move(s.top());
}

class hah{
    public : 
    int index;
    unique_ptr<int> data;
    hah() : data(nullptr){}
    virtual void work() = 0;
};

class pu : public hah{
    public :
    void lala(){cout<<"kaka\n";};
};

class hehe : public pu{
    public :
    void work(){

    }
    void isthisreal(){
        std::cout<<"help\n";
    }
};

enum testit{monday};

class testag{
    public :
    testit id;
    testag(testit a ):id(a){};
};

class kk{};

//class pu: public hah{};
/*
bool test(unique_ptr<hah> &p){
    p = make_unique<pu>();
    return (p != nullptr);
}*/

class a{
    public : 
    int index;
    a(int x) : index(x){}
    virtual void happy() = 0;
};

class b : public a{
    public :
    b(int x) : a(x){}
    void happy(){
        cout<<"correct !\n";
    }
};

pair<int, int> geta(){
    return make_pair(2,3);
}

int main()
{
    //freopen("RISCV-target/hello.s", "w", stdout);
    //bool a = true;
    
    map<string, int> m;
    m["haha"] = 1;
    //m["haha"] = 2;
    for (auto i : m){
        //cout<<i.first;
        m[i.first] = 2;
    }
    cout<<m["haha"]<<'\n';

}