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

    explicit Node(LexicalType lexicalType) : type(lexicalType), token_(nullptr) {}

    void addChild(Node *child);  //在右边插入一个子节点
    void setParent(Node *parent); //设置父节点
};

void Node::addChild(Node *child) {
    children.insert(children.end(), child);
}
void Node::setParent(Node *parent) {
    this->parent_ = parent;
}
#endif //PASER_NODE_H