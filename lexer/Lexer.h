//
// Created by 11863 on 2023/9/22.
//

#ifndef LEXER_LEXER_H
#define LEXER_LEXER_H
using namespace std;

#include "Source.h"
#include "LexicalType.h"
#include "Token.h"

#define line sourceFile[lineNum]
#define currentCharacter line[charNum]
#define nextCharacter line[charNum+1]
#define EatCurrentSpace \
while (isspace(currentCharacter)) {\
charNum++;\
}
#define EatNextSpace \
while (isspace(currentCharacter)) {\
charNum++;\
}


class Lexer {
    vector<string> sourceFile;
    vector<Token> tokens;
public:
    explicit Lexer(Source &source) {
        sourceFile = source.getSourceFile();
        lexicalAnalysis();
    }

    vector<Token> getTokens() const {
        return tokens;
    }
    friend ostream &operator<<(ostream &os, const Lexer lexer) {
        for (auto &token: lexer.tokens) {
            os << token << endl;
        }
        return os;
    }

private:
    void lexicalAnalysis() {
        for (int lineNum = 0; lineNum < sourceFile.size(); lineNum++) {
            for (int charNum = 0; charNum < line.length(); charNum++) {
                //清除空白字符
                EatCurrentSpace
                //分类讨论
                switch (currentCharacter) {
                    case '/':
                        if (nextCharacter == '/') {
                            charNum = line.length();
                        } else if (nextCharacter == '*') {
                            //读到*/
                            while (currentCharacter != '*' || nextCharacter != '/') {
                                if (charNum + 1 < line.length() - 1) {
                                    charNum++;
                                } else if (lineNum < sourceFile.size() - 1) {
                                    lineNum++;
                                    charNum = 0;
                                } else {
                                    //出错
                                }
                            }
                            //消掉*/
                            charNum++;
                        } else {
                            tokens.push_back((Token("/", LexicalType::DIV, lineNum)));
                        }
                        break;
                    case '&':
                        if (nextCharacter == '&') {
                            tokens.push_back(Token("&&", LexicalType::AND, lineNum));
                            charNum++;
                        } else {
                            //出错
                        }
                        break;
                    case '|':
                        if (nextCharacter == '|') {
                            tokens.push_back(Token("||", LexicalType::OR, lineNum));
                            charNum++;
                        } else {
                            //出错
                        }
                        break;
                    case '+':
                        tokens.push_back((Token("+", LexicalType::PLUS, lineNum)));
                        break;
                    case '-':
                        tokens.push_back((Token("-", LexicalType::MINU, lineNum)));
                        break;
                    case '*':
                        tokens.push_back((Token("*", LexicalType::MULT, lineNum)));
                        break;
                    case '%':
                        tokens.push_back((Token("%", LexicalType::MOD, lineNum)));
                        break;
                    case '<':
                        if (charNum +1< line.length() && nextCharacter == '=') {
                            tokens.push_back((Token("<=", LexicalType::LEQ, lineNum)));
                            charNum++;
                        } else {
                            tokens.push_back((Token("<", LexicalType::LSS, lineNum)));
                        }
                        break;
                    case '>':
                        if (charNum +1< line.length() && nextCharacter == '=') {
                            tokens.push_back((Token(">=", LexicalType::GEQ, lineNum)));
                            charNum++;
                        } else {
                            tokens.push_back((Token(">", LexicalType::GRE, lineNum)));
                        }
                        break;
                    case '=':
                        if (charNum +1< line.length() && nextCharacter == '=') {
                            tokens.push_back((Token("==", LexicalType::EQL, lineNum)));
                            charNum++;
                        } else {
                            tokens.push_back((Token("=", LexicalType::ASSIGN, lineNum)));
                        }
                        break;
                    case '!':
                        if (charNum +1< line.length() && nextCharacter == '=') {
                            tokens.push_back((Token("!=", LexicalType::NEQ, lineNum)));
                            charNum++;
                        } else {
                            tokens.push_back((Token("!", LexicalType::NOT, lineNum)));
                        }
                        break;
                    case ';':
                        tokens.push_back((Token(";", LexicalType::SEMICN, lineNum)));
                        break;
                    case ',':
                        tokens.push_back((Token(",", LexicalType::COMMA, lineNum)));
                        break;
                    case '(':
                        tokens.push_back((Token("(", LexicalType::LPARENT, lineNum)));
                        break;
                    case ')':
                        tokens.push_back((Token(")", LexicalType::RPARENT, lineNum)));
                        break;
                    case '[':
                        tokens.push_back((Token("[", LexicalType::LBRACK, lineNum)));
                        break;
                    case ']':
                        tokens.push_back((Token("]", LexicalType::RBRACK, lineNum)));
                        break;
                    case '{':
                        tokens.push_back((Token("{", LexicalType::LBRACE, lineNum)));
                        break;
                    case '}':
                        tokens.push_back((Token("}", LexicalType::RBRACE, lineNum)));
                        break;
                    case '"': {
                        string tmpStr{};
                        tmpStr.push_back(currentCharacter);
                        charNum++;
                        bool isAfterBackslash = false;
                        while (charNum < line.length()) {
                            if (isAfterBackslash) {
                                isAfterBackslash = false;
                                tmpStr.push_back(currentCharacter);
                                charNum++;
                            } else if (currentCharacter == '"') {
                                tmpStr.push_back(currentCharacter);
                                break;
                            } else if (currentCharacter == '\\') {
                                tmpStr.push_back(currentCharacter);
                                isAfterBackslash = true;
                                charNum++;
                            } else {
                                tmpStr.push_back(currentCharacter);
                                charNum++;
                            }
                        }
                        tokens.push_back((Token(tmpStr, LexicalType::STRCON, lineNum)));
                        break;
                    }

                    default:
                        if (isdigit(currentCharacter)) {
                            string tmpStr{};
                            tmpStr.push_back(currentCharacter);
                            charNum++;
                            while (charNum < line.length()) {
                                if (isdigit(currentCharacter)) {
                                    tmpStr.push_back(currentCharacter);
                                    charNum++;
                                } else {
                                    charNum--;
                                    break;
                                }
                            }
                            tokens.push_back((Token(tmpStr, LexicalType::INTCON, lineNum)));
                        }
                        if (isIdentStart(currentCharacter)) {
                            string tmpStr{};
                            tmpStr.push_back(currentCharacter);
                            charNum++;
                            while (charNum < line.length()) {
                                if (isIdentContinue(currentCharacter)) {
                                    tmpStr.push_back(currentCharacter);
                                    charNum++;
                                } else {
                                    charNum--;
                                    break;
                                }
                            }
                            judgingLexicology(tmpStr, lineNum);
                        } else {
                            // 出错
                        }
                        break;
                }

            }
        }
    }

    static bool isIdentStart(char c) {
        return isalpha(c) || c == '_';
    }

    static bool isIdentContinue(char c) {
        return isalpha(c) || isdigit(c) || c == '_';
    }

    void judgingLexicology(string str, int lineNum) {
        if (str == "main") {
            tokens.push_back((Token(str, LexicalType::MAINTK, lineNum)));
        } else if (str == "const") {
            tokens.push_back((Token(str, LexicalType::CONSTTK, lineNum)));
        } else if (str == "int") {
            tokens.push_back((Token(str, LexicalType::INTTK, lineNum)));
        } else if (str == "break") {
            tokens.push_back((Token(str, LexicalType::BREAKTK, lineNum)));
        } else if (str == "continue") {
            tokens.push_back((Token(str, LexicalType::CONTINUETK, lineNum)));
        } else if (str == "if") {
            tokens.push_back((Token(str, LexicalType::IFTK, lineNum)));
        } else if (str == "else") {
            tokens.push_back((Token(str, LexicalType::ELSETK, lineNum)));
        } else if (str == "for") {
            tokens.push_back((Token(str, LexicalType::FORTK, lineNum)));
        } else if (str == "getint") {
            tokens.push_back((Token(str, LexicalType::GETINTTK, lineNum)));
        } else if (str == "printf") {
            tokens.push_back((Token(str, LexicalType::PRINTFTK, lineNum)));
        } else if (str == "return") {
            tokens.push_back((Token(str, LexicalType::RETURNTK, lineNum)));
        } else if (str == "void") {
            tokens.push_back((Token(str, LexicalType::VOIDTK, lineNum)));
        } else {
            tokens.push_back((Token(str, LexicalType::IDENFR, lineNum)));
        }
    }
};

#endif //LEXER_LEXER_H
