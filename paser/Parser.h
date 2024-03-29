//
// Created by 11863 on 2023/9/30.
//

/*
 * 错误处理
 * 缺少分号 i
 * 缺少右小括号 j
 * 缺少右中括号 k
 */

#ifndef PASER_PARSER_H
#define PASER_PARSER_H

#define curToken tokens[cur].getToken()
#define next1Token tokens[cur+1].getToken()
#define next2Token tokens[cur+2].getToken()
#define curType tokens[cur].getType()
#define curLineNum tokens[cur].getLineNum()
#define beforeLineNum tokens[cur-1].getLineNum()

using namespace std;

#include "vector"
#include "../lexer/Token.h"
#include "Node.h"
#include "../semanticAnalyzer/ErrorType.h"


class Parser {
    int cur;
    vector<Token> tokens;
    Node *ancestor;
    vector<string> output;
    map<int, string> &errorLog_;
public:
    explicit Parser(const Lexer &lexer, map<int, string> &errorLog) : errorLog_(errorLog) {
        tokens = lexer.getTokens();
        cur = 0;
        output.clear();
        parseCompUnit();
        postorderTraversal(ancestor);
    }

    Node *getAncestor() {
        return ancestor;
    }

    friend ostream &operator<<(ostream &os, const Parser parser) {
        for (int outputNum = parser.output.size() - 1; outputNum >= 0; outputNum--) {
            os << parser.output[outputNum] << endl;
        }
        return os;
    }

    void postorderTraversal(Node *parent) {
        output.push_back(parent->getToken_());
        vector<Node *> children = parent->getChildren();
        for (int childNum = children.size() - 1; childNum >= 0; childNum--) {
            postorderTraversal(children[childNum]);
        }
    }

    void error() {
        cout << "error" << endl;
    }

    void buildRelationship(Node *parent, Node *child) {
        parent->addChild(child);
        child->setParent(parent);
    }

    void buildNonTerminalSymbol(Node *parent, Node *child) {
        buildRelationship(parent, child);
    }

    void addBetweenParentAndTheLastNode(Node *parent, Node *add) {
        Node *child = parent->popChild();
        buildRelationship(parent, add);
        buildRelationship(add, child);
    }

    //可以补充一个对的回去
    void buildErrorSymbol(Node *parent, ErrorType errorType) {
        errorLog_.insert(make_pair(beforeLineNum, errorTypeToPrint(errorType)));
        LexicalType type;
        string token;
        switch (errorType) {
            //缺少分号 i
            case ErrorType::MissingSEMICN:
                type=LexicalType::SEMICN;
                token =";";
                break;

            //缺少右小括号’)’ j
            case ErrorType::MissingRPARENT:
                type=LexicalType::RPARENT;
                token =")";
                break;

            //缺少右中括号’]’ k
            case ErrorType::MissingRBRACK:
                type=LexicalType::RBRACK;
                token ="]";
                break;
        }
        buildRelationship(parent, new Node(type, token, beforeLineNum));
    }

    void buildTerminalSymbol(Node *parent, string token) {
        if (curToken != token) {
            error();
        }
        buildRelationship(parent, new Node(curType, curToken, curLineNum));
        cur++;
    }

    void buildTerminalSymbol(Node *parent, LexicalType type) {
        if (curType != type)
            error();
        buildRelationship(parent, new Node(curType, curToken, curLineNum));
        cur++;
    }

    bool isExp() {
        for (int count = 0; cur + count < tokens.size(); count++) {
            if (tokens[cur + count].getToken() == "=") return false;
            else if (tokens[cur + count].getToken() == ";") return true;
        }
        return false;
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
            buildNonTerminalSymbol(ancestor, parseFuncDef());
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
        if (curToken == ";")
            buildTerminalSymbol(constDecl, ";");
        else
            buildErrorSymbol(constDecl, ErrorType::MissingSEMICN);

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
            if (curToken == "]")
                buildTerminalSymbol(constDef, "]");
            else
                buildErrorSymbol(constDef, ErrorType::MissingRBRACK);
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
        if (curToken == ";")
            buildTerminalSymbol(varDecl, ";");
        else
            buildErrorSymbol(varDecl, ErrorType::MissingSEMICN);
        return varDecl;
    }

    // VarDef → Ident { '[' ConstExp ']' }  ['=' InitVal]
    Node *parseVarDef() {
        Node *varDef = new Node(LexicalType::VarDef);
        //Ident
        buildTerminalSymbol(varDef, LexicalType::IDENFR);
        // { '[' ConstExp ']' }
        while (curToken == "[") {
            buildTerminalSymbol(varDef, "[");
            buildNonTerminalSymbol(varDef, parseConstExp());
            if (curToken == "]")
                buildTerminalSymbol(varDef, "]");
            else
                buildErrorSymbol(varDef, ErrorType::MissingRBRACK);
        }
        if (curToken == "=") {
            buildTerminalSymbol(varDef, "=");
            buildNonTerminalSymbol(varDef, parseInitVal());
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
        if (curToken != ")"&&curToken!="{") {
            buildNonTerminalSymbol(funcDef, parseFuncFParams());
        }
        // ')'
        if (curToken == ")")
            buildTerminalSymbol(funcDef, ")");
        else
            buildErrorSymbol(funcDef, ErrorType::MissingRPARENT);
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
            if (curToken == "]")
                buildTerminalSymbol(funcFParam, "]");
            else
                buildErrorSymbol(funcFParam, ErrorType::MissingRBRACK);
            // { '[' ConstExp ']' }
            while (curToken == "[") {
                buildTerminalSymbol(funcFParam, "[");
                buildNonTerminalSymbol(funcFParam, parseConstExp());
                if (curToken == "]")
                    buildTerminalSymbol(funcFParam, "]");
                else
                    buildErrorSymbol(funcFParam, ErrorType::MissingRBRACK);
            }
        }

        return funcFParam;
    }

    // Block → '{' { BlockItem } '}'
    // BlockItem → Decl | Stmt
    Node *parseBlock() {
        Node *block = new Node(LexicalType::Block);
        buildTerminalSymbol(block, "{");

        while (curToken != "}") {
            if (curToken == "const") {
                buildNonTerminalSymbol(block, parseConstDecl());
            } else if (curToken == "int") {
                buildNonTerminalSymbol(block, parseVarDecl());
            } else {
                buildNonTerminalSymbol(block, parseStmt());
            }
        }

        buildTerminalSymbol(block, "}");

        return block;
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
        Node *stmt = new Node(LexicalType::Stmt);
        if (curToken == "{") {
            //  Block
            buildNonTerminalSymbol(stmt, parseBlock());
        } else if (curToken == "if") {
            //  'if' '(' Cond ')' Stmt [ 'else' Stmt ]
            buildTerminalSymbol(stmt, "if");
            buildTerminalSymbol(stmt, "(");
            buildNonTerminalSymbol(stmt, parseCond());
            if (curToken == ")")
                buildTerminalSymbol(stmt, ")");
            else
                buildErrorSymbol(stmt, ErrorType::MissingRPARENT);
            buildNonTerminalSymbol(stmt, parseStmt());
            if (curToken == "else") {
                buildTerminalSymbol(stmt, "else");
                buildNonTerminalSymbol(stmt, parseStmt());
            }
        } else if (curToken == "for") {
            //  'for' '(' [ForStmt] ';' [Cond] ';' [forStmt] ')' Stmt
            buildTerminalSymbol(stmt, "for");
            buildTerminalSymbol(stmt, "(");
            if (curToken != ";")
                buildNonTerminalSymbol(stmt, parseForStmt());
            if (curToken == ";")
                buildTerminalSymbol(stmt, ";");
            else
                buildErrorSymbol(stmt, ErrorType::MissingSEMICN);
            if (curToken != ";")
                buildNonTerminalSymbol(stmt, parseCond());
            if (curToken == ";")
                buildTerminalSymbol(stmt, ";");
            else
                buildErrorSymbol(stmt, ErrorType::MissingSEMICN);
            if (curToken != ")")
                buildNonTerminalSymbol(stmt, parseForStmt());
            if (curToken == ")")
                buildTerminalSymbol(stmt, ")");
            else
                buildErrorSymbol(stmt, ErrorType::MissingRPARENT);
            buildNonTerminalSymbol(stmt, parseStmt());
        } else if (curToken == "break") {
            //  'break' ';'
            buildTerminalSymbol(stmt, "break");
            if (curToken == ";")
                buildTerminalSymbol(stmt, ";");
            else
                buildErrorSymbol(stmt, ErrorType::MissingSEMICN);
        } else if (curToken == "continue") {
            //'continue' ';'
            buildTerminalSymbol(stmt, "continue");
            if (curToken == ";")
                buildTerminalSymbol(stmt, ";");
            else
                buildErrorSymbol(stmt, ErrorType::MissingSEMICN);
        } else if (curToken == "return") {
            //  'return' [Exp] ';'
            buildTerminalSymbol(stmt, "return");
            if (curToken != ";") {
                buildNonTerminalSymbol(stmt, parseExp());
            }
            if (curToken == ";")
                buildTerminalSymbol(stmt, ";");
            else
                buildErrorSymbol(stmt, ErrorType::MissingSEMICN);
        } else if (curToken == "printf") {
            //  'printf''('FormatString{','Exp}')'';'
            buildTerminalSymbol(stmt, "printf");
            buildTerminalSymbol(stmt, "(");
            buildTerminalSymbol(stmt, LexicalType::STRCON);
            while (curToken == ",") {
                buildTerminalSymbol(stmt, ",");
                buildNonTerminalSymbol(stmt, parseExp());
            }
            if (curToken == ")")
                buildTerminalSymbol(stmt, ")");
            else
                buildErrorSymbol(stmt, ErrorType::MissingRPARENT);
            if (curToken == ";")
                buildTerminalSymbol(stmt, ";");
            else
                buildErrorSymbol(stmt, ErrorType::MissingSEMICN);
        } else if (isExp()) {
            //  [Exp] ';'
            if (curToken != ";") {
                buildNonTerminalSymbol(stmt, parseExp());
            }
            if (curToken == ";")
                buildTerminalSymbol(stmt, ";");
            else
                buildErrorSymbol(stmt, ErrorType::MissingSEMICN);
        } else {
            //  LVal '='
            buildNonTerminalSymbol(stmt, parseLVal());
            buildTerminalSymbol(stmt, "=");
            if (curToken == "getint") {
                //'getint''('')'';'
                buildTerminalSymbol(stmt, "getint");
                buildTerminalSymbol(stmt, "(");
                if (curToken == ")")
                    buildTerminalSymbol(stmt, ")");
                else
                    buildErrorSymbol(stmt, ErrorType::MissingRPARENT);
                if (curToken == ";")
                    buildTerminalSymbol(stmt, ";");
                else
                    buildErrorSymbol(stmt, ErrorType::MissingSEMICN);
            } else {
                //Exp ';'
                buildNonTerminalSymbol(stmt, parseExp());
                if (curToken == ";")
                    buildTerminalSymbol(stmt, ";");
                else
                    buildErrorSymbol(stmt, ErrorType::MissingSEMICN);
            }
        }
        return stmt;
    }

    // ForStmt → LVal '=' Exp
    Node *parseForStmt() {
        Node *forStmt = new Node(LexicalType::ForStmt);
        //LVal
        buildNonTerminalSymbol(forStmt, parseLVal());
        // '='
        buildTerminalSymbol(forStmt, "=");
        // Exp
        buildNonTerminalSymbol(forStmt, parseExp());

        return forStmt;
    }

    // Exp → AddExp
    Node *parseExp() {
        Node *exp = new Node(LexicalType::Exp);
        buildNonTerminalSymbol(exp, parseAddExp());
        return exp;
    }

    // Cond → LOrExp
    Node *parseCond() {
        Node *cond = new Node(LexicalType::Cond);
        buildNonTerminalSymbol(cond, parseLOrExp());
        return cond;
    }

    // LVal → Ident {'[' Exp ']'}
    Node *parseLVal() {
        Node *lVal = new Node(LexicalType::LVal);
        //Ident
        buildTerminalSymbol(lVal, LexicalType::IDENFR);
        // {'[' Exp ']'}
        while (curToken == "[") {
            buildTerminalSymbol(lVal, "[");
            buildNonTerminalSymbol(lVal, parseExp());
            if (curToken == "]")
                buildTerminalSymbol(lVal, "]");
            else
                buildErrorSymbol(lVal, ErrorType::MissingRBRACK);
        }

        return lVal;
    }

    // PrimaryExp → '(' Exp ')' | LVal | Number
    Node *parsePrimaryExp() {
        Node *primaryExp = new Node(LexicalType::PrimaryExp);

        if (curToken == "(") {
            //'('
            buildTerminalSymbol(primaryExp, "(");
            // Exp
            buildNonTerminalSymbol(primaryExp, parseExp());
            // ')'
            buildTerminalSymbol(primaryExp, ")");
        } else if (curType == LexicalType::IDENFR) {
            // LVal
            buildNonTerminalSymbol(primaryExp, parseLVal());
        } else if (curType == LexicalType::INTCON) {
            // Number
            buildNonTerminalSymbol(primaryExp, parseNumber());
        } else {
            error();
        }

        return primaryExp;
    }

    // Number → IntConst
    Node *parseNumber() {
        Node *number = new Node(LexicalType::Number);
        //IntConst
        buildTerminalSymbol(number, LexicalType::INTCON);
        return number;
    }

    // UnaryExp → PrimaryExp
    // | Ident '(' [FuncRParams] ')'
    // | UnaryOp UnaryExp
    Node *parseUnaryExp() {
        Node *unaryExp = new Node(LexicalType::UnaryExp);
        if (curType == LexicalType::IDENFR && next1Token == "(") {
            //Ident
            buildTerminalSymbol(unaryExp, LexicalType::IDENFR);
            // '('
            buildTerminalSymbol(unaryExp, "(");
            // [FuncRParams]
            if (curToken != ")"&&curToken!=";") {
                buildNonTerminalSymbol(unaryExp, parseFuncRParams());
            }
            // ')'
            if (curToken == ")")
                buildTerminalSymbol(unaryExp, ")");
            else
                buildErrorSymbol(unaryExp, ErrorType::MissingRPARENT);
        } else if (curToken == "+" || curToken == "-" || curToken == "!") {
            //UnaryOp
            buildNonTerminalSymbol(unaryExp, parseUnaryOp());
            // UnaryExp
            buildNonTerminalSymbol(unaryExp, parseUnaryExp());
        } else {
            // PrimaryExp
            buildNonTerminalSymbol(unaryExp, parsePrimaryExp());
        }
        return unaryExp;
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

    // MulExp → UnaryExp | MulExp ('*' | '/' | '%') UnaryExp
    Node *parseMulExp() {
        Node *mulExp = new Node(LexicalType::MulExp);

        buildNonTerminalSymbol(mulExp, parseUnaryExp());
        while (curToken == "*" || curToken == "/" || curToken == "%") {
            //左边的合并
            Node *add = new Node(LexicalType::MulExp);
            addBetweenParentAndTheLastNode(mulExp, add);
            if (curToken == "*") {
                buildTerminalSymbol(mulExp, "*");
            } else if (curToken == "/") {
                buildTerminalSymbol(mulExp, "/");
            } else if (curToken == "%") {
                buildTerminalSymbol(mulExp, "%");
            }
            buildNonTerminalSymbol(mulExp, parseUnaryExp());

        }

        return mulExp;
    }

    // AddExp → MulExp | AddExp ('+' | '−') MulExp
    Node *parseAddExp() {
        Node *addExp = new Node(LexicalType::AddExp);

        buildNonTerminalSymbol(addExp, parseMulExp());
        while (curToken == "+" || curToken == "-") {
            Node *add = new Node(LexicalType::AddExp);
            addBetweenParentAndTheLastNode(addExp, add);
            if (curToken == "+") {
                buildTerminalSymbol(addExp, "+");
            } else if (curToken == "-") {
                buildTerminalSymbol(addExp, "-");
            }
            buildNonTerminalSymbol(addExp, parseMulExp());
        }

        return addExp;
    }

    // RelExp → AddExp | RelExp ('<' | '>' | '<=' | '>=') AddExp
    Node *parseRelExp() {
        Node *relExp = new Node(LexicalType::RelExp);

        buildNonTerminalSymbol(relExp, parseAddExp());
        while (curToken == "<" || curToken == ">" || curToken == "<=" || curToken == ">=") {
            Node *add = new Node(LexicalType::RelExp);
            addBetweenParentAndTheLastNode(relExp, add);
            if (curToken == "<") {
                buildTerminalSymbol(relExp, "<");
            } else if (curToken == ">") {
                buildTerminalSymbol(relExp, ">");
            } else if (curToken == "<=") {
                buildTerminalSymbol(relExp, "<=");
            } else if (curToken == ">=") {
                buildTerminalSymbol(relExp, ">=");
            }
            buildNonTerminalSymbol(relExp, parseAddExp());
        }


        return relExp;
    }

    // EqExp → RelExp | EqExp ('==' | '!=') RelExp
    Node *parseEqExp() {
        Node *eqExp = new Node(LexicalType::EqExp);
        buildNonTerminalSymbol(eqExp, parseRelExp());
        while (curToken == "==" || curToken == "!=") {
            Node *add = new Node(LexicalType::EqExp);
            addBetweenParentAndTheLastNode(eqExp, add);
            if (curToken == "==") {
                buildTerminalSymbol(eqExp, "==");
            } else if (curToken == "!=") {
                buildTerminalSymbol(eqExp, "!=");
            }
            buildNonTerminalSymbol(eqExp, parseRelExp());
        }

        return eqExp;
    }

    // LAndExp → EqExp | LAndExp '&&' EqExp
    Node *parseLAndExp() {
        Node *lAndExp = new Node(LexicalType::LAndExp);
        buildNonTerminalSymbol(lAndExp, parseEqExp());
        while (curToken == "&&") {
            Node *add = new Node(LexicalType::LAndExp);
            addBetweenParentAndTheLastNode(lAndExp, add);
            buildTerminalSymbol(lAndExp, "&&");
            buildNonTerminalSymbol(lAndExp, parseEqExp());
        }
        return lAndExp;
    }

    // LOrExp → LAndExp | LAndExp '||' LOrExp
    Node *parseLOrExp() {
        Node *lOrExp = new Node(LexicalType::LOrExp);
        buildNonTerminalSymbol(lOrExp, parseLAndExp());
        while (curToken == "||") {
            Node *add = new Node(LexicalType::LOrExp);
            addBetweenParentAndTheLastNode(lOrExp, add);

            buildTerminalSymbol(lOrExp, "||");
            buildNonTerminalSymbol(lOrExp, parseLAndExp());
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
