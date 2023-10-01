//
// Created by 11863 on 2023/9/30.
//

#ifndef PASER_PARSER_H
#define PASER_PARSER_H

#define curToken tokens[cur].getToken()
#define next1Token tokens[cur+1].getToken()
#define next2Token tokens[cur+2].getToken()
#define curType tokens[cur].getType()

using namespace std;

#include "vector"
#include "../lexer/Token.h"
#include "Node.h"

class Parser {
    int cur;
    vector<Token> tokens;
    Node *ancestor;
public:
    explicit Parser(const Lexer &lexer) {
        tokens = lexer.getTokens();
        cur = 0;
    }

    void error() {

    }

    void buildRelationship(Node *parent, Node *child) {
        parent->addChild(child);
        child->setParent(parent);
    }

    void buildNonTerminalSymbol(Node *parent, Node *child) {
        buildRelationship(parent, child);
    }

    void buildTerminalSymbol(Node *parent, string token) {
        if (curToken != token)
            error();
        buildRelationship(parent, new Node(curType, curToken));
        cur++;
    }

    void buildTerminalSymbol(Node *parent, LexicalType type) {
        if (curType != type)
            error();
        buildRelationship(parent, new Node(curType, curToken));
        cur++;
    }

    // CompUnit → {Decl} {FuncDef} MainFuncDef
    // ConstDecl | VarDecl
    void parseCompUnit() {
        ancestor = new Node(LexicalType::CompUnit);
        //ConstDecl | VarDecl
        while (cur < tokens.size() - 2 && next2Token != "(") {
            if (curToken == "const") {
                buildNonTerminalSymbol(ancestor, parseConstDecl());
            } else {
                buildNonTerminalSymbol(ancestor, parseVarDecl());
            }
        }

        //FuncDef
        while (cur < tokens.size() - 1 && next1Token != "main") {
            buildNonTerminalSymbol(ancestor, parseFuncType());
        }

        //MainFuncDef
        {
            buildNonTerminalSymbol(ancestor, parseMainFuncDef());
        }
    }

    // ConstDecl → 'const' 'int' ConstDef { ',' ConstDef } ';'
    Node *parseConstDecl() {
        Node *constDecl = new Node(LexicalType::ConstDecl);

        //const
        buildTerminalSymbol(constDecl, "const");
        //int
        buildTerminalSymbol(constDecl, "int");
        //ConstDef
        buildNonTerminalSymbol(constDecl, parseConstDef());
        //{ ',' ConstDef }
        while (curToken == ",") {
            buildTerminalSymbol(constDecl, ",");
            buildNonTerminalSymbol(constDecl, parseConstDef());
        }
        //;
        buildTerminalSymbol(constDecl, ";");

        return constDecl;
    }

    // ConstDef → Ident { '[' ConstExp ']' } '=' ConstInitVal
    Node *parseConstDef() {
        Node *constDef = new Node(LexicalType::ConstDef);
        //Ident
        buildTerminalSymbol(constDef, LexicalType::IDENFR);
        // { '[' ConstExp ']' }
        while (curToken == "[") {
            buildTerminalSymbol(constDef, "[");
            buildNonTerminalSymbol(constDef, parseConstExp());
            buildTerminalSymbol(constDef, "]");
        }
        // '='
        buildTerminalSymbol(constDef, "=");
        // ConstInitVal
        buildNonTerminalSymbol(constDef, parseConstInitVal());
        return constDef;

    }

    // ConstInitVal → ConstExp | '{' [ ConstInitVal { ',' ConstInitVal } ] '}'
    Node *parseConstInitVal() {
        Node *constInitVal = new Node(LexicalType::ConstInitVal);
        // ConstExp
        if (curToken != "{") {
            buildNonTerminalSymbol(constInitVal, parseConstExp());
        } else { // '{' [ ConstInitVal { ',' ConstInitVal } ] '}'
            // '{'
            buildTerminalSymbol(constInitVal, "{");
            // [ ConstInitVal { ',' ConstInitVal } ]
            if (curToken != "}") {
                buildNonTerminalSymbol(constInitVal, parseConstInitVal());
                while (curToken == ",") {
                    buildTerminalSymbol(constInitVal, ",");
                    buildNonTerminalSymbol(constInitVal, parseConstInitVal());
                }
            }
            // '}'
            buildTerminalSymbol(constInitVal, "}");
        }
        return constInitVal;
    }

    // VarDecl → 'int' VarDef { ',' VarDef } ';'
    Node *parseVarDecl() {
        Node *varDecl = new Node(LexicalType::VarDecl);
        //'int'
        buildTerminalSymbol(varDecl, "int");
        // VarDef
        buildNonTerminalSymbol(varDecl, parseVarDef());
        // { ',' VarDef }
        while (curToken == ",") {
            buildTerminalSymbol(varDecl, ",");
            buildNonTerminalSymbol(varDecl, parseVarDef());
        }
        // ';'
        buildTerminalSymbol(varDecl, ";");
        return varDecl;
    }

    // VarDef → Ident { '[' ConstExp ']' }
    Node *parseVarDef() {
        Node *varDef = new Node(LexicalType::VarDef);
        //Ident
        buildTerminalSymbol(varDef, LexicalType::IDENFR);
        // { '[' ConstExp ']' }
        while (curToken == "[") {
            buildTerminalSymbol(varDef, "[");
            buildNonTerminalSymbol(varDef, parseConstExp());
            buildTerminalSymbol(varDef, "]");
        }
        return varDef;
    }

    // InitVal → Exp | '{' [ InitVal { ',' InitVal } ] '}'
    Node *parseInitVal() {
        Node *initVal = new Node(LexicalType::InitVal);
        //Exp
        if (curToken != "{") {
            buildNonTerminalSymbol(initVal, parseExp());
        } else { // '{' [ InitVal { ',' InitVal } ] '}'
            buildTerminalSymbol(initVal, "{");
            if (curToken != "}") {
                buildNonTerminalSymbol(initVal, parseInitVal());
                while (curToken == ",") {
                    buildTerminalSymbol(initVal, ",");
                    buildNonTerminalSymbol(initVal, parseInitVal());
                }
            }
            buildTerminalSymbol(initVal, "}");
        }

        return initVal;
    }

    // FuncDef → FuncType Ident '(' [FuncFParams] ')' Block
    Node *parseFuncDef() {
        Node *funcDef = new Node(LexicalType::FuncDef);

        //FuncType
        buildNonTerminalSymbol(funcDef, parseFuncType());
        // Ident
        buildTerminalSymbol(funcDef, LexicalType::IDENFR);
        // '('
        buildTerminalSymbol(funcDef, "(");
        // [FuncFParams]
        if (curToken != ")") {
            buildNonTerminalSymbol(funcDef, parseFuncFParams());
        }
        // ')'
        buildTerminalSymbol(funcDef, ")");
        // Block
        buildNonTerminalSymbol(funcDef, parseBlock());

        return funcDef;
    }

    // MainFuncDef → 'int' 'main' '(' ')' Block
    Node *parseMainFuncDef() {
        Node *mainFuncDef = new Node(LexicalType::MainFuncDef);

        //'int'
        buildTerminalSymbol(mainFuncDef, "int");
        // 'main'
        buildTerminalSymbol(mainFuncDef, "main");
        // '('
        buildTerminalSymbol(mainFuncDef, "(");
        // ')'
        buildTerminalSymbol(mainFuncDef, ")");
        // Block
        buildNonTerminalSymbol(mainFuncDef, parseBlock());

        return mainFuncDef;
    }

    // FuncType → 'void' | 'int'
    Node *parseFuncType() {
        Node *funcType = new Node(LexicalType::FuncType);
        //'void'
        if (curToken == "void") {
            buildTerminalSymbol(funcType, "void");
        } else { // 'int'
            buildTerminalSymbol(funcType, "int");
        }
        return funcType;
    }

    //  FuncFParams → FuncFParam { ',' FuncFParam }
    Node *parseFuncFParams() {
        Node *funcFParams = new Node(LexicalType::FuncFParams);
        //FuncFParam
        buildNonTerminalSymbol(funcFParams, parseFuncFParam());
        // { ',' FuncFParam }
        while (curToken == ",") {
            buildTerminalSymbol(funcFParams, ",");
            buildNonTerminalSymbol(funcFParams, parseFuncFParam());
        }

        return funcFParams;
    }

    // FuncFParam → BType Ident ['[' ']' { '[' ConstExp ']' }]
    Node *parseFuncFParam() {
        Node *funcFParam = new Node(LexicalType::FuncFParam);
        //'int'
        buildTerminalSymbol(funcFParam, "int");
        // Ident
        buildTerminalSymbol(funcFParam, LexicalType::IDENFR);
        // ['[' ']' { '[' ConstExp ']' }]
        if (curToken == "[") {
            //'['
            buildTerminalSymbol(funcFParam, "[");
            // ']'
            buildTerminalSymbol(funcFParam, "]");
            // { '[' ConstExp ']' }
            while (curToken == "[") {
                buildTerminalSymbol(funcFParam, "[");
                buildNonTerminalSymbol(funcFParam, parseConstExp());
                buildTerminalSymbol(funcFParam, "]");
            }
        }

        return funcFParam;
    }

    // Block → '{' { BlockItem } '}'
    // BlockItem → Decl | Stmt
    Node *parseBlock() {
        Node* = new Node(LexicalType::);


        return;
    }

    // Stmt → LVal '=' Exp ';'
    // | [Exp] ';'
    // | Block
    // | 'if' '(' Cond ')' Stmt [ 'else' Stmt ]
    // | 'for' '(' [ForStmt] ';' [Cond] ';' [forStmt] ')' Stmt
    // | 'break' ';' | 'continue' ';'
    // | 'return' [Exp] ';'
    // | LVal '=' 'getint''('')'';'
    // | 'printf''('FormatString{','Exp}')'';'
    Node *parseStmt() {
        Node* = new Node(LexicalType::);


        return;
    }

    // ForStmt → LVal '=' Exp
    Node *parseForStmt() {
        Node* = new Node(LexicalType::);


        return;
    }

    // Exp → AddExp
    Node *parseExp() {
        Node* = new Node(LexicalType::);


        return;
    }

    // Cond → LOrExp
    Node *parseCond() {
        Node* = new Node(LexicalType::);


        return;
    }

    // LVal → Ident {'[' Exp ']'}
    Node *parseLVal() {
        Node* = new Node(LexicalType::);


        return;
    }

    // PrimaryExp → '(' Exp ')' | LVal | Number
    Node *parsePrimaryExp() {
        Node* = new Node(LexicalType::);


        return;
    }

    // Number → IntConst
    Node *parseNumber() {
        Node* = new Node(LexicalType::);


        return;
    }

    // UnaryExp → PrimaryExp
    // | Ident '(' [FuncRParams] ')'
    // | UnaryOp UnaryExp
    Node *parseUnaryExp() {
        Node* = new Node(LexicalType::);


        return;
    }

    // UnaryOp → '+' | '−' | '!'
    Node *parseUnaryOp() {
        Node *unaryOp = new Node(LexicalType::UnaryOp);
        if (curToken == "+") {
            buildTerminalSymbol(unaryOp, "+");
        } else if (curToken == "-") {
            buildTerminalSymbol(unaryOp, "-");
        } else if (curToken == "!") {
            buildTerminalSymbol(unaryOp, "!");
        } else {
            error();
        }
        return unaryOp;
    }

    // FuncRParams → Exp { ',' Exp }
    Node *parseFuncRParams() {
        Node *funcRParams = new Node(LexicalType::FuncRParams);
        buildNonTerminalSymbol(funcRParams, parseExp());
        while (curToken == ",") {
            buildTerminalSymbol(funcRParams, ",");
            buildNonTerminalSymbol(funcRParams, parseExp());
        }
        return funcRParams;
    }

    // MulExp → UnaryExp | UnaryExp ('*' | '/' | '%') MulExp
    Node *parseMulExp() {
        Node *mulExp = new Node(LexicalType::MulExp);

        buildNonTerminalSymbol(mulExp, parseUnaryExp());
        if (curToken == "*") {
            buildTerminalSymbol(mulExp, "*");
            buildNonTerminalSymbol(mulExp, parseMulExp());
        } else if (curToken == "/") {
            buildTerminalSymbol(mulExp, "/");
            buildNonTerminalSymbol(mulExp, parseMulExp());
        } else if (curToken == "%") {
            buildTerminalSymbol(mulExp, "%");
            buildNonTerminalSymbol(mulExp, parseMulExp());
        }
        return mulExp;
    }

    // AddExp → MulExp | MulExp ('+' | '−') AddExp
    Node *parseAddExp() {
        Node *addExp = new Node(LexicalType::AddExp);

        buildNonTerminalSymbol(addExp, parseMulExp());
        if (curToken == "+") {
            buildTerminalSymbol(addExp, "+");
            buildNonTerminalSymbol(addExp, parseAddExp());
        } else if (curToken == "-") {
            buildTerminalSymbol(addExp, "-");
            buildNonTerminalSymbol(addExp, parseAddExp());
        }
        return addExp;
    }

    // RelExp → AddExp | AddExp ('<' | '>' | '<=' | '>=') RelExp
    Node *parseRelExp() {
        Node *relExp = new Node(LexicalType::RelExp);

        buildNonTerminalSymbol(relExp, parseAddExp());
        if (curToken == "<") {
            buildTerminalSymbol(relExp, "<");
            buildNonTerminalSymbol(relExp, parseRelExp());
        } else if (curToken == ">") {
            buildTerminalSymbol(relExp, ">");
            buildNonTerminalSymbol(relExp, parseRelExp());
        } else if (curToken == "<=") {
            buildTerminalSymbol(relExp, "<=");
            buildNonTerminalSymbol(relExp, parseRelExp());
        } else if (curToken == ">=") {
            buildTerminalSymbol(relExp, ">=");
            buildNonTerminalSymbol(relExp, parseRelExp());
        }

        return relExp;
    }

    // EqExp → RelExp | RelExp ('==' | '!=') EqExp
    Node *parseEqExp() {
        Node *eqExp = new Node(LexicalType::EqExp);
        buildNonTerminalSymbol(eqExp, parseRelExp());
        if (curToken == "==") {
            buildTerminalSymbol(eqExp, "==");
            buildNonTerminalSymbol(eqExp, parseEqExp());
        } else if (curToken == "!=") {
            buildTerminalSymbol(eqExp, "!=");
            buildNonTerminalSymbol(eqExp, parseEqExp());
        }
        return eqExp;
    }

    // LAndExp → EqExp |  EqExp '&&' LAndExp
    Node *parseLAndExp() {
        Node *lAndExp = new Node(LexicalType::LAndExp);
        buildNonTerminalSymbol(lAndExp, parseEqExp());
        if (curToken == "&&") {
            buildTerminalSymbol(lAndExp, "&&");
            buildNonTerminalSymbol(lAndExp, parseLAndExp());
        }
        return lAndExp;
    }

    // LOrExp → LAndExp | LAndExp '||' LOrExp
    Node *parseLOrExp() {
        Node *lOrExp = new Node(LexicalType::LOrExp);
        buildNonTerminalSymbol(lOrExp, parseLAndExp());
        if (curToken == "||") {
            buildTerminalSymbol(lOrExp, "||");
            buildNonTerminalSymbol(lOrExp, parseLOrExp());
        }
        return lOrExp;
    }

    // ConstExp → AddExp
    Node *parseConstExp() {
        Node *constExp = new Node(LexicalType::ConstExp);
        buildNonTerminalSymbol(constExp, parseAddExp());
        return constExp;
    }
};

#endif //PASER_PARSER_H
