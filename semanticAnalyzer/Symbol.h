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

    explicit Symbol(Node *node, Type type) : node_(node), type_(type), typesOfParams(), sizeOfArray(),
                                             symbolOfParams() {}

    string getToken() {
        return node_->getPureToken();
    }

    void addParam(Type type) {
        typesOfParams.push_back(type);
    }

    bool isNotCon() {
        return type_ != VAR_CONST && type_ != ONE_DIMENSION_CONST && type_ != TWO_DIMENSIONS_CONST;
    }

    Type getType() {
        return type_;
    }

    int getLine() {
        return node_->getLine();
    }

    //参数个数
    int getSizeOfParams() {
        return typesOfParams.size();
    }

    Type getTypeAt(int i) {
        return typesOfParams.at(i);
    }

    void setGlobalVaclTrue() {
        isGlobalVacl = true;
    }

    bool isGlobal() {
        return isGlobalVacl;
    }

    string getOp() {
        if (isGlobalVacl)
            return "@" + getToken();
        else
            return node_->getOp();
    }

    int getNum() {
        return node_->getNum();
    }

    void setNum(int num) {
        node_->setNum(num);
    }

    void addSizeOfArray(int num) {
        sizeOfArray.push_back(num);
    }

    int getSizeOfArray(int num) {
        return sizeOfArray.at(num);
    }

    void addSymbolOfParam(Symbol *symbol) {
        symbolOfParams.push_back(symbol);
    }

    Symbol *getSymbol(int i) {
        return symbolOfParams.at(i);
    }

    Node *getNode() {
        return node_;
    }

private:
    //语法信息
    Node *node_;
    //类型
    Type type_;
    //函数参数类型
    vector<Type> typesOfParams;
    //函数参数
    vector<Symbol *> symbolOfParams;
    //数组大小
    vector<int> sizeOfArray;
    //是否为全局变量
    bool isGlobalVacl = false;
};

#endif //SEMANTICANALYZER_SYMBOL_H
