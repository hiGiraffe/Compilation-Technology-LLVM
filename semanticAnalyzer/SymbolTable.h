//
// Created by 11863 on 2023/10/25.
//

#ifndef SEMANTICANALYZER_SYMBOLTABLE_H
#define SEMANTICANALYZER_SYMBOLTABLE_H
using namespace std;

#include <unordered_map>
#include "vector"
#include "Symbol.h"


class SymbolTable {
    SymbolTable *parent_;
    vector<SymbolTable *> children = {};
    unordered_map<string, Symbol*> symbols = {};

public:
    explicit SymbolTable() : parent_(nullptr) {}

    explicit SymbolTable(SymbolTable *parent) : parent_(parent) {}

    bool isIdentIn(string token) {
        if (symbols.find(token) != symbols.end())
            return true;
        return false;
    }

    bool isIdentInAllTables(string token) {
        unordered_map<string, Symbol*>* symbolsToFind = &symbols;
        SymbolTable* parent= parent_;
        if (symbolsToFind->find(token) != symbolsToFind->end())
            return true;
        while (parent != nullptr) {
            symbolsToFind = parent->getSymbols();
            if (symbolsToFind->find(token) != symbolsToFind->end())
                return true;
            parent=parent->getParents();
        }
        return false;
    }

    Symbol* findIdentInAllTables(string token){
        unordered_map<string, Symbol*> *symbolsToFind = &symbols;
        SymbolTable* parent= parent_;
        if (symbolsToFind->find(token) != symbolsToFind->end())
            return symbolsToFind->at(token);
        while (parent != nullptr) {
            symbolsToFind = parent->getSymbols();
            if (symbolsToFind->find(token) != symbolsToFind->end())
                return symbolsToFind->at(token);
            parent=parent->getParents();
        }
        return nullptr;
    }

    void addSymbolTable(SymbolTable *child) {
        children.push_back(child);
    }

    void addSymbol(Node *node, Symbol::Type type) {
        Symbol* symbol=new Symbol(node, type);
        symbols.insert(make_pair(node->getPureToken(), symbol));
    }

    void addSymbol(Symbol *symbol) {
        symbols.insert(make_pair(symbol->getToken(), symbol));
    }

    SymbolTable *getParents() {
        return parent_;
    }

    Symbol *getSymbol(string name) {
        return symbols.at(name);
    }

    unordered_map<string, Symbol*> *getSymbols() {
        return &symbols;
    }
};

#endif //SEMANTICANALYZER_SYMBOLTABLE_H
