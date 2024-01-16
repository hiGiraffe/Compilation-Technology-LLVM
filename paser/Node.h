//
// Created by 11863 on 2023/9/27.
//

#ifndef PASER_NODE_H
#define PASER_NODE_H

#include "vector"
#include "../lexer/lib.h"
#include "../semanticAnalyzer/ErrorType.h"

using namespace std;

class Node {
    LexicalType type;
    string token_;
    vector<Node *> children = {};
    int lineNum_;
    Node *parent_;

    ErrorType errorType_;
    bool isErrorNode;

    //llvm ir 代码生成
    int regNum = -1; //寄存器id
    int num = 0; //计算结果

    vector<vector<int>> twoDimensionNum = {};
    vector<int> oneDimensionNum = {};

    vector<vector<int>> twoDimensionRegNum = {};
    vector<int> oneDimensionRegNum = {};

    //条件语句
    string label="";
public:
    //终结符
    explicit Node(LexicalType lexicalType, string token, int lineNum) : type(lexicalType), token_(token),
                                                                        lineNum_(lineNum), isErrorNode(false) {}

    //非终结符
    explicit Node(LexicalType lexicalType) : type(lexicalType), token_(string()), isErrorNode(false) {}

    void addNumInOneDimension(int num) {
        oneDimensionNum.push_back(num);
    }

    vector<int> getOneDimensionNum() {
        return oneDimensionNum;
    }

    void addVectorInTwoDimension(vector<int> oneDimension) {
        twoDimensionNum.push_back(oneDimension);
    }

    vector<vector<int>> getTwoDimensionNum() {
        return twoDimensionNum;
    }

    //寄存器值
    void addRegNumInOneDimension(int num) {
        oneDimensionRegNum.push_back(num);
    }

    vector<int> getOneDimensionRegNum() {
        return oneDimensionRegNum;
    }
    void addRegVectorInTwoDimension(vector<int> oneDimension) {
        twoDimensionRegNum.push_back(oneDimension);
    }

    vector<vector<int>> getTwoDimensionRegNum() {
        return twoDimensionRegNum;
    }

    void setRegNum(int num) {
        regNum = num;
    }

    int getRegNum() {
        return regNum;
    }

    void setNum(int numSet) {
        num = numSet;
    }

    int getNum() {
        return num;
    }

    string getOp() {
        if (regNum != -1)
            return "%" + to_string(regNum);
        else
            return to_string(num);
    }
    //错误节点
//    explicit Node(ErrorType errorType, int lineNum) : errorType_(errorType), lineNum_(lineNum), isErrorNode(true) {}

    //在右边插入一个子节点
    void addChild(Node *child) {
        children.insert(children.end(), child);
    }

    //弹出最右边的子节点
    Node *popChild() {
        Node *child = children[children.size() - 1];
        children.pop_back();
        return child;
    }

    //设置父节点
    void setParent(Node *parent) {
        this->parent_ = parent;
    }

    string getToken_() {
        if (isErrorNode) {
            return errorTypeToPrint(errorType_);
        } else if (!token_.empty()) {
            return lexicalTypeToPrint(type) + " " + token_;
            //+ " " + to_string(lineNum_)
        } else {
            return "<" + lexicalTypeToPrint(type) + ">";
        }
    }

    string getPureToken() {
        return token_;
    }

    LexicalType getType() {
        return type;
    }

    int getLine() {
        return lineNum_;
    }

    vector<Node *> getChildren() {
        return children;
    }

    Node *getChild(int i) {
        return children[i];
    }

    Node *getParent() {
        return parent_;
    }

    bool getIsErrorNode() {
        return isErrorNode;
    }

    int getSize() {
        return children.size();
    }

    void setLabel(string label) {
        this->label = label;
    }

    string getLabel() {
        return label;
    }

};

#endif //PASER_NODE_H
