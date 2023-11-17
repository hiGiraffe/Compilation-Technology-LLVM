//
// Created by 11863 on 2023/9/23.
//

#ifndef LEXER_TOKEN_H
#define LEXER_TOKEN_H

using namespace std;

#include "LexicalType.h"

class Token {
    string token;
    LexicalType type;
    int lineNum;
//    int number;
public:
    explicit Token(string token, LexicalType type, int line) : token(token), type(type), lineNum(line+1) {
//        if (type == LexicalType::INTCON) {
//            number = stoi(token);
//        } else {
//            number = 0;
//        }
    }

    string getToken (){
        return token;
    }
    LexicalType getType(){
        return type;
    }

    int getLineNum(){
        return lineNum;
    }

    friend ostream &operator<<(ostream &os, const Token token) {
        os  <<lexicalTypeToPrint(token.type)<< " " << token.token;
        return os;
    }
};

#endif //COMPILER_LEXER_TOKEN_H
