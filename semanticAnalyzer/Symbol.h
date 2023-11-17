//
// Created by 11863 on 2023/10/25.
//

#ifndef SEMANTICANALYZER_SYMBOL_H
#define SEMANTICANALYZER_SYMBOL_H
using namespace std;

#include "../paser/Node.h"

class Symbol {


public:
    enum Type {
        //普通变量
        VAR,
        VAR_CONST,

        //一维数组
        ONE_DIMENSION,
        ONE_DIMENSION_CONST,

        //两维数组
        TWO_DIMENSIONS,
        TWO_DIMENSIONS_CONST,

        //函数类型
        FUNCTION_INT,
        FUNCTION_VOID,

        //调用
        FuncR_Params,

        //什么都不是
        NOTHING,
    };

    explicit Symbol(Node *node, Type type) : node_(node), type_(type),typesOfParams() {}

    string getToken() {
        return node_->getPureToken();
    }

    void addParam(Type type){
        typesOfParams.push_back(type);
    }

    bool isNotCon(){
        return type_!=VAR_CONST &&type_!=ONE_DIMENSION_CONST &&type_!=TWO_DIMENSIONS_CONST ;
    }

    Type getType(){
        return type_;
    }

    int getLine(){
        return node_->getLine();
    }

    int getSizeOfParams(){
        return typesOfParams.size();
    }

    Type getTypeAt(int i){
        return typesOfParams.at(i);
    }
private:
    //语法信息
    Node *node_;
    //类型
    Type type_;
    //函数参数类型
    vector<Type> typesOfParams;
    //数组每一维度大小
    //大小
};

#endif //SEMANTICANALYZER_SYMBOL_H
