//
// Created by 11863 on 2023/9/27.
//

#ifndef PASER_NODE_H
#define PASER_NODE_H

#include "vector"
#include "../lexer/lib.h"

using namespace std;

class Node {
    LexicalType type;
    string token_;
    vector<Node *> children = {};
    Node *parent_;
public:
    explicit Node(LexicalType lexicalType, string token) : type(lexicalType), token_(token) {}

    explicit Node(LexicalType lexicalType) : type(lexicalType), token_(string()) {}

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
        if (!token_.empty()) {
            return lexicalTypeToPrint(type) + " " + token_;
        } else {
            return "<" + lexicalTypeToPrint(type) + ">";
        }
    }

    vector<Node *> getChildren() {
        return children;
    }

};

#endif //PASER_NODE_H
