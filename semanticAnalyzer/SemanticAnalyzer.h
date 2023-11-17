//
// Created by 11863 on 2023/10/24.
//

#ifndef SEMANTICANALYZER_SEMANTICANALYZER_H
#define SEMANTICANALYZER_SEMANTICANALYZER_H

using namespace std;

#include "../paser/Parser.h"
#include "../paser/Node.h"
#include "SymbolTable.h"

#define curChildType parents->getChild(cur)->getType()
#define curChildToken parents->getChild(cur)->getPureToken()
#define curChildLine parents->getChild(cur)->getLine()
#define curChildIsError parents->getChild(cur)->getIsErrorNode()
#define curChild parents->getChild(cur)

#define child(i) parents->getChild(i)
#define childToken(i) parents->getChild(i)->getPureToken()


class SemanticAnalyzer {
    map<int, string> &errorLog_;
    Node *ancestor;
    SymbolTable *curTable = new SymbolTable();
public:
    explicit SemanticAnalyzer(
            Parser parser,
            map<int, string> &errorLog
    ) : errorLog_(errorLog), ancestor(parser.getAncestor()) {
        analyzeCompUnit(ancestor);
    }

    friend ostream &operator<<(ostream &os, const SemanticAnalyzer semanticAnalyzer) {
        for (const auto &pair: semanticAnalyzer.errorLog_) {
            os << pair.first << " " << pair.second << endl;
        }
        return os;
    }

    void error() {
        cout << "error" << endl;
    }

    //    编译单元    CompUnit → {Decl} {FuncDef} MainFuncDef   Decl → ConstDecl | VarDecl
    void analyzeCompUnit(Node *parents) {
        int cur = 0;
        while (curChildType == LexicalType::ConstDecl || curChildType == LexicalType::VarDecl) {
            if (curChildType == LexicalType::ConstDecl) {
                analyzeConstDecl(curChild);
                cur++;
            } else if (curChildType == LexicalType::VarDecl) {
                analyzeVarDecl(curChild);
                cur++;
            }
        }

        while (curChildType == LexicalType::FuncDef) {
            analyzeFuncDef(curChild);
            cur++;
        }

        analyzeMainFuncDef(curChild);
    }

    //    常量声明    ConstDecl → 'const' 'int' ConstDef { ',' ConstDef } ';' // i
    void analyzeConstDecl(Node *parents) {
        int cur = 2;

        analyzeConstDef(curChild);
        cur++;
        while (curChildToken != ";") {
            cur++;
            analyzeConstDef(curChild);
            cur++;
        }
    }

    //    常数定义    ConstDef → Ident { '[' ConstExp ']' } '=' ConstInitVal  // b k
    void analyzeConstDef(Node *parents) {
        int cur = 0;
        //变量重定义
        if (curTable->isIdentIn(curChildToken)) {
            errorLog_.insert(make_pair(curChildLine, errorTypeToPrint(ErrorType::nameRedefinition)));
        }
        //读入符号表
        if (childToken(1) != "[") {
            curTable->addSymbol(curChild, Symbol::Type::VAR_CONST);
            cur = 2;
        } else if (childToken(4) != "[") {
            curTable->addSymbol(curChild, Symbol::Type::ONE_DIMENSION_CONST);
            //todo:读入数组信息
            analyzeConstExp(child(2));
            cur = 5;
        } else {
            curTable->addSymbol(curChild, Symbol::Type::TWO_DIMENSIONS_CONST);
            //todo:读入数组信息
            analyzeConstExp(child(2));
            analyzeConstExp(child(5));
            cur = 8;
        }

        //直接到ConstInitVal
        analyzeConstInitVal(curChild);
    }

    //    常量初值    ConstInitVal → ConstExp
    //    | '{' [ ConstInitVal { ',' ConstInitVal } ] '}'
    void analyzeConstInitVal(Node *parents) {
        int cur = 0;
        if (curChildToken == "{") {
            cur++;
            if (curChildToken != "}") {
                analyzeConstInitVal(curChild);
                cur++;
                while (curChildToken != "}") {
                    cur++;
                    analyzeConstInitVal(curChild);
                    cur++;
                }
            }
        } else {
            analyzeConstExp(child(0));
        }
    }

    //    变量声明    VarDecl → 'int' VarDef { ',' VarDef } ';' // i
    void analyzeVarDecl(Node *parents) {
        int cur = 1;
        analyzeVarDef(curChild);
        cur++;

        while (curChildToken != ";") {
            cur++;
            analyzeVarDef(curChild);
            cur++;
        }
    }

    //    变量定义    VarDef → Ident { '[' ConstExp ']' } // b
    //    | Ident { '[' ConstExp ']' } '=' InitVal // k
    void analyzeVarDef(Node *parents) {
        int cur = 0;
        //变量重定义
        if (curTable->isIdentIn(curChildToken)) {
            errorLog_.insert(make_pair(curChildLine, errorTypeToPrint(ErrorType::nameRedefinition)));
        }
        //读入符号表
        if (parents->getSize() == 1 || (1 < parents->getSize() && childToken(1) != "[")) {
            curTable->addSymbol(curChild, Symbol::Type::VAR);
            cur = 2;
        } else if (parents->getSize() == 4 || (4 < parents->getSize() && childToken(4) != "[")) {
            curTable->addSymbol(curChild, Symbol::Type::ONE_DIMENSION);
            //todo:读入数组信息
            analyzeConstExp(child(2));
            cur = 5;
        } else {
            curTable->addSymbol(curChild, Symbol::Type::TWO_DIMENSIONS);
            //todo:读入数组信息
            analyzeConstExp(child(2));
            analyzeConstExp(child(5));
            cur = 8;
        }
        if (parents->getSize() > cur) {
            analyzeInitVal(curChild);
        }
    }

    //    变量初值    InitVal → Exp | '{' [ InitVal { ',' InitVal } ] '}'
    void analyzeInitVal(Node *parents) {
        int cur = 0;
        if (curChildToken == "{") {
            cur++;
            if (curChildToken != "}") {
                analyzeInitVal(curChild);
                cur++;
                while (curChildToken != "}") {
                    cur++;
                    analyzeInitVal(curChild);
                    cur++;
                }
            }
        } else {
            analyzeExp(curChild, nullptr);
        }
    }

    //    函数定义    FuncDef → FuncType Ident '(' [FuncFParams] ')' Block // b g j
    //    函数类型    FuncType → 'void' | 'int'
    void analyzeFuncDef(Node *parents) {
        int cur = 0;
        string funcType = curChild->getChild(0)->getPureToken();
        cur++;
        //变量重定义
        if (curTable->isIdentIn(curChildToken)) {
            errorLog_.insert(make_pair(curChildLine, errorTypeToPrint(ErrorType::nameRedefinition)));
        }
        //读入符号表
        Symbol::Type symbolType;
        if (funcType == "void") {
            symbolType = Symbol::Type::FUNCTION_VOID;
        } else if (funcType == "int") {
            symbolType = Symbol::Type::FUNCTION_INT;
        }
        Symbol *symbol = new Symbol(child(1), symbolType);
        curTable->addSymbol(symbol);

        SymbolTable *child = new SymbolTable(curTable);
        curTable->addSymbolTable(child);
        curTable = child;

        //读函数参数
        if (childToken(3) != ")") {
            analyzeFuncFParams(child(3), symbol);
            cur = 5;
        } else {
            cur = 4;
        }


        //有返回值的函数没有return
        Node *returnStmtNode = curChild->getChild(curChild->getSize() - 2);//应该是block中的return的stmt节点
        Node *rbraceNode = curChild->getChild(curChild->getSize() - 1);//block中的}的节点
        if (
                funcType == "int"
                &&
                (returnStmtNode->getSize() == 0 || returnStmtNode->getChild(0)->getPureToken() != "return")
                ) { //倒数第二个的第一个子节点不是return
            errorLog_.insert(make_pair(rbraceNode->getLine(), //}的行数
                                       errorTypeToPrint(ErrorType::ReturnMissing)));
        }
        //读block
        analyzeBlock(curChild);

        curTable = curTable->getParents();
    }

    //    主函数定义   MainFuncDef → 'int' 'main' '(' ')' Block // g j
    void analyzeMainFuncDef(Node *parents) {
        int cur = 4;
        //有返回值的函数没有return
        Node *returnStmtNode = curChild->getChild(curChild->getSize() - 2);//应该是block中的return的stmt节点
        Node *rbraceNode = curChild->getChild(curChild->getSize() - 1);//block中的}的节点
        if (
                returnStmtNode->getSize() == 0 || returnStmtNode->getChild(0)->getPureToken() != "return"
                ) { //倒数第二个的第一个子节点不是return
            errorLog_.insert(make_pair(rbraceNode->getLine(), //}的行数
                                       errorTypeToPrint(ErrorType::ReturnMissing)));
        }
        //读Table
        SymbolTable *child = new SymbolTable(curTable);
        curTable->addSymbolTable(child);
        curTable = child;
        analyzeBlock(curChild);
        curTable = curTable->getParents();
    }

    //    函数形参表   FuncFParams → FuncFParam { ',' FuncFParam }
    void analyzeFuncFParams(Node *parents, Symbol *symbol) {
        int cur = 0;
        //参数类型和数量传入symbol
        analyzeFuncFParam(curChild, symbol);
        cur++;
        while (cur < parents->getSize() - 1) {
            cur++;
            analyzeFuncFParam(curChild, symbol);
            cur++;
        }
    }

    //    函数形参    FuncFParam → 'int' Ident ['[' ']' { '[' ConstExp ']' }]  //   b k
    void analyzeFuncFParam(Node *parents, Symbol *symbol) {
        int cur = 2;
        int flag = 0;

        if (cur < parents->getSize()) {
            cur++;
            cur++;
            flag = 1;
            while (cur < parents->getSize()) {
                flag = 2;
                cur++;
                //todo: 问题，这里是否会调转顺序
                analyzeConstExp(curChild);
                cur++;
                cur++;
            }
        }
        //todo 填入信息
        //变量重定义
        if (curTable->isIdentIn(child(1)->getPureToken())) {
            errorLog_.insert(make_pair(child(1)->getLine(), errorTypeToPrint(ErrorType::nameRedefinition)));
        }
        if (flag == 0) {
            symbol->addParam(Symbol::Type::VAR);
            curTable->addSymbol(child(1), Symbol::Type::VAR);
        } else if (flag == 1) {
            symbol->addParam(Symbol::Type::ONE_DIMENSION);
            curTable->addSymbol(child(1), Symbol::Type::ONE_DIMENSION);
        } else if (flag == 2) {
            symbol->addParam(Symbol::Type::TWO_DIMENSIONS);
            curTable->addSymbol(child(1), Symbol::Type::TWO_DIMENSIONS);
        }
    }

    //    语句块     Block → '{' { BlockItem } '}'   BlockItem → Decl | Stmt  Decl → ConstDecl | VarDecl
    void analyzeBlock(Node *parents) {
        //blockItem不变
        int cur = 1;
        while (curChildToken != "}") {
            if (curChildType == LexicalType::ConstDecl) {
                analyzeConstDecl(curChild);
            } else if (curChildType == LexicalType::VarDecl) {
                analyzeVarDecl(curChild);
            } else if (curChildType == LexicalType::Stmt) {
                analyzeStmt(curChild);
            } else {
                error();
            }
            cur++;
        }
    }

    //            语句  Stmt → |√| LVal '=' Exp ';' //h i
    //            |√| [Exp] ';' //i
    //            |√| Block
    //    |√| 'if' '(' Cond ')' Stmt [ 'else' Stmt ] // j
    //    |√| 'for' '('[ForStmt] ';' [Cond] ';' [ForStmt] ')' Stmt
    //    |√| 'break' ';' | 'continue' ';' // i m
    //    |√| 'return' [Exp] ';' // f i
    //    |√| LVal '=' 'getint''('')'';' // h i j
    //    | 'printf''('FormatString{,Exp}')'';' // i j l
    void analyzeStmt(Node *parents) {
        int cur = 0;
        if (curChildType == LexicalType::LVal) {
            Symbol *ident = nullptr;
            analyzeLVal(curChild, ident, nullptr);
            if (ident != nullptr) {
                if (ident->isNotCon()) { //不是常量
                    cur++;
                    cur++;
                    if (curChildToken != "getint") {
                        analyzeExp(curChild, nullptr);
                    }
                } else {
                    errorLog_.insert(
                            make_pair(child(cur)->getChild(0)->getLine(),
                                      errorTypeToPrint(ErrorType::ConstantModification)));
                }
            }
        } else if (curChildType == LexicalType::Exp) {
            analyzeExp(curChild, nullptr);
        } else if (curChildType == LexicalType::Block) {
            SymbolTable *child = new SymbolTable(curTable);
            curTable->addSymbolTable(child);
            curTable = child;
            analyzeBlock(curChild);
            curTable = curTable->getParents();
        } else if (curChildType == LexicalType::IFTK) {
            analyzeCond(child(2));
            analyzeStmt(child(4));
            if (parents->getSize() > 5) {
                analyzeStmt(child(6));
            }
        } else if (curChildType == LexicalType::FORTK) {
            //'for' '('[ForStmt] ';' [Cond] ';' [ForStmt] ')' Stmt
            cur++;
            cur++;
            if (curChildToken != ";") {
                analyzeForStmt(curChild);
                cur++;
            }
            cur++;
            if (curChildToken != ";") {
                analyzeCond(curChild);
                cur++;
            }
            cur++;
            if (curChildToken != ";" && curChildToken != ")") {
                analyzeForStmt(curChild);
                cur++;
            }
            cur++;
            analyzeStmt(curChild);
        } else if (curChildType == LexicalType::BREAKTK || curChildType == LexicalType::CONTINUETK) {
            checkRedundantBreakAndContinue(curChild);
        } else if (curChildType == LexicalType::RETURNTK && child(cur + 1)->getPureToken() != ";") {
            checkReturnRedundant(curChild);
        } else if (curChildType == LexicalType::PRINTFTK) {
//            'printf''('FormatString{,Exp}')'';' // i j l
            int numExp = (parents->getSize() - 5) / 2;
            checkFormatStrNumNotMatch(child(2), numExp);
            cur=3;
            for (int i = 0; i < numExp; i++) {
                cur++;
                analyzeExp(curChild,nullptr);
                cur++;
            }
        }
    }

    void checkRedundantBreakAndContinue(Node *breakOrContinueNode) {
        Node *tmp = breakOrContinueNode->getParent();
        while (tmp != ancestor) {
            //是for块中
            if (tmp->getChild(0)->getPureToken() == "for") {
                return;
            }
            tmp = tmp->getParent();
        }
        errorLog_.insert(
                make_pair(breakOrContinueNode->getLine(), errorTypeToPrint(ErrorType::RedundantBreakAndContinue)));
    }

    void checkReturnRedundant(Node *returnNode) {
        Node *tmp = returnNode->getParent();

        while (tmp != ancestor) {
            if (tmp->getType() == LexicalType::FuncDef) {
                string funcType = tmp->getChild(0)->getChild(0)->getPureToken();
                //是无返回值的函数
                if (funcType == "void" && returnNode->getParent()->getSize() > 2) {
                    errorLog_.insert(make_pair(returnNode->getLine(), errorTypeToPrint(ErrorType::ReturnRedundant)));
                }
                return;
            }
            tmp = tmp->getParent();
        }
    }

    void checkFormatStrNumNotMatch(Node *formatString, int numExp) {
        string str = formatString->getPureToken();
        int numVar = 0;
        for (char c: str) {
            if (c == '%') {
                numVar++;
            }
        }
        if (numVar != numExp) {
            errorLog_.insert(make_pair(formatString->getLine(), errorTypeToPrint(ErrorType::FormatStrNumNotMatch)));
        }

    }

    //    语句 ForStmt → LVal '=' Exp   //h
    void analyzeForStmt(Node *parents) {
        int cur = 0;
        Symbol *ident;
        analyzeLVal(curChild, ident, nullptr);
        if (ident->isNotCon()) { //不是常量
            cur++;
            cur++;
            analyzeExp(curChild, nullptr);
        } else {
            errorLog_.insert(make_pair(ident->getLine(), errorTypeToPrint(ErrorType::ConstantModification)));
        }
    }

    //    表达式 Exp → AddExp 注：SysY 表达式是int 型表达式
    void analyzeExp(Node *parents, Symbol::Type *type) {
        int cur = 0;
        analyzeAddExp(curChild, type);
    }

    //            条件表达式   Cond → LOrExp
    void analyzeCond(Node *parents) {
        int cur = 0;
        analyzeLOrExp(curChild);
    }

    //            左值表达式   LVal → Ident {'[' Exp ']'} // c k
    void analyzeLVal(Node *parents, Symbol *&ident, Symbol::Type *type) {
        int cur = 0;
        int flag = 0;
        if (curTable->isIdentInAllTables(curChildToken)) {
            ident = curTable->findIdentInAllTables(curChildToken);
            cur++;
            while (cur < parents->getSize()) {
                flag++;
                cur++;
                analyzeExp(curChild, nullptr);
                cur++;
                cur++;
            }
        } else {
            errorLog_.insert(make_pair(curChildLine, errorTypeToPrint(ErrorType::nameUndefined)));
        }
        if (type != nullptr) {
            if (flag == 0) {
                *type = Symbol::Type::VAR;
            } else if (flag == 1) {
                *type = Symbol::Type::ONE_DIMENSION;
            } else if (flag == 2) {
                *type = Symbol::Type::TWO_DIMENSIONS;
            } else {
                error();
            }
        }
    }

    //    基本表达式   PrimaryExp → '(' Exp ')' | LVal | Number
    void analyzePrimaryExp(Node *parents, Symbol::Type *type) {
        int cur = 0;
        if (curChildType == LexicalType::LVal) {
            Symbol *useIdent; //使用的时候
            analyzeLVal(curChild, useIdent, type);
            //处理符号
            Symbol *definedIdent = curTable->findIdentInAllTables(curChild->getChild(0)->getPureToken());
            if (type != nullptr && definedIdent != nullptr) {
                if (definedIdent->getType() == Symbol::Type::VAR) {
                    *type = Symbol::Type::VAR;
                } else if (definedIdent->getType() == Symbol::Type::ONE_DIMENSION) {
                    if (*type == Symbol::Type::ONE_DIMENSION) {
                        *type = Symbol::Type::VAR;
                    } else if (*type == Symbol::Type::VAR) {
                        *type = Symbol::Type::ONE_DIMENSION;
                    } else {
                        error();
                    }
                } else if (definedIdent->getType() == Symbol::Type::TWO_DIMENSIONS) {
                    if (*type == Symbol::Type::TWO_DIMENSIONS) {
                        *type = Symbol::Type::VAR;
                    } else if (*type == Symbol::Type::ONE_DIMENSION) {
                        *type = Symbol::Type::ONE_DIMENSION;
                    } else if (*type == Symbol::Type::VAR) {
                        *type = Symbol::Type::TWO_DIMENSIONS;
                    } else {
                        error();
                    }
                } else if (!definedIdent->isNotCon()) {
                    *type = definedIdent->getType();
                } else {
                    error();
                }
            }
            //fixme:这里会不会type！=0
        } else if (curChildType == LexicalType::Number) {
            analyzeNumber(curChild);
            if (type != nullptr)
                *type = Symbol::Type::VAR;
        } else {
            analyzeExp(child(1), type);
        }
    }

    //            数值  Number → IntConst
    void analyzeNumber(Node *parents) {
        int cur = 0;
        //todo:读入数据
        return;
    }

    //            一元表达式   UnaryExp → PrimaryExp
    //            | Ident '(' [FuncRParams] ')' // c d e j
    //            | UnaryOp UnaryExp
    void analyzeUnaryExp(Node *parents, Symbol::Type *type) {
        int cur = 0;
        if (curChildType == LexicalType::PrimaryExp) {
            analyzePrimaryExp(curChild, type);
        } else if (curChildType == LexicalType::UnaryOp) {
            analyzeUnaryOp(curChild);
            cur++;
            analyzeUnaryExp(curChild, type);
        } else {
            if (curTable->isIdentInAllTables(curChildToken)) {
                Symbol *definedIdent = curTable->findIdentInAllTables(curChildToken);

                Symbol *calledIdent = new Symbol(curChild, Symbol::Type::FuncR_Params);
                if (childToken(2) != ")") {
                    analyzeFuncRParams(child(2), calledIdent);
                }

                if (definedIdent->getSizeOfParams() != calledIdent->getSizeOfParams()) {
                    errorLog_.insert(
                            make_pair(curChildLine, errorTypeToPrint(ErrorType::NumOfFunctionParametersNotMatch)));
                } else {
                    for (int i = 0; i < definedIdent->getSizeOfParams(); i++) {
                        if (definedIdent->getTypeAt(i) != calledIdent->getTypeAt(i)) {
                            errorLog_.insert(make_pair(curChildLine,
                                                       errorTypeToPrint(ErrorType::TypeOfFunctionParametersNotMatch)));
                        }
                    }
                }
                if (type != nullptr){
                    if(definedIdent->getType()==Symbol::Type::FUNCTION_INT){
                        *type = Symbol::Type::VAR;
                    }
                    else{
                        *type=Symbol::Type::NOTHING;
                    }
                }

            } else {
                errorLog_.insert(make_pair(curChildLine, errorTypeToPrint(ErrorType::nameUndefined)));
            }
        }
    }

    //    单目运算符   UnaryOp → '+' | '−' | '!' 注：'!'仅出现在条件表达式中
    void analyzeUnaryOp(Node *parents) {
        int cur = 0;
        //todo:读入数据
    }

    //    函数实参表   FuncRParams → Exp { ',' Exp }
    void analyzeFuncRParams(Node *parents, Symbol *params) {
        int cur = 0;
        Symbol::Type *type = new Symbol::Type();
        analyzeExp(curChild, type);
        params->addParam(*type);
        cur++;
        while (cur < parents->getSize()) {
            cur++;
            type = new Symbol::Type();
            analyzeExp(curChild, type);
            params->addParam(*type);
            cur++;
        }
    }

    //    乘除模表达式  MulExp → UnaryExp | MulExp ('*' | '/' | '%') UnaryExp
    void analyzeMulExp(Node *parents, Symbol::Type *type) {
        int cur = 0;
        if (curChildType == LexicalType::UnaryExp) {
            analyzeUnaryExp(curChild, type);
        } else {
            while (curChildType == LexicalType::MulExp) {
                analyzeMulExp(curChild, nullptr);
                cur++;
                cur++;
            }
            analyzeUnaryExp(curChild, type);
        }
    }

    //            加减表达式   AddExp → MulExp | AddExp ('+' | '−') MulExp
    void analyzeAddExp(Node *parents, Symbol::Type *type) {
        int cur = 0;
        if (curChildType == LexicalType::MulExp) {
            analyzeMulExp(curChild, type);
        } else {
            while (curChildType == LexicalType::AddExp) {
                analyzeAddExp(curChild, nullptr);
                cur++;
                cur++;
            }
            analyzeMulExp(curChild, type);
        }
    }

    //            关系表达式   RelExp → AddExp | RelExp ('<' | '>' | '<=' | '>=') AddExp
    void analyzeRelExp(Node *parents) {
        int cur = 0;
        if (curChildType == LexicalType::AddExp) {
            analyzeAddExp(curChild, nullptr);
        } else {
            while (curChildType == LexicalType::RelExp) {
                analyzeRelExp(curChild);
                cur++;
                cur++;
            }
            analyzeAddExp(curChild, nullptr);
        }
    }

    //            相等性表达式  EqExp → RelExp | EqExp ('==' | '!=') RelExp
    void analyzeEqExp(Node *parents) {
        int cur = 0;
        if (curChildType == LexicalType::RelExp) {
            analyzeRelExp(curChild);
        } else {
            while (curChildType == LexicalType::EqExp) {
                analyzeEqExp(curChild);
                cur++;
                cur++;
            }
            analyzeRelExp(curChild);
        }
    }

    //            逻辑与表达式  LAndExp → EqExp | LAndExp '&&' EqExp
    void analyzeLAndExp(Node *parents) {
        int cur = 0;
        if (curChildType == LexicalType::EqExp) {
            analyzeEqExp(curChild);
        } else {
            while (curChildType == LexicalType::LAndExp) {
                analyzeLAndExp(curChild);
                cur++;
                cur++;
            }
            analyzeEqExp(curChild);
        }
    }

    //            逻辑或表达式  LOrExp → LAndExp | LOrExp '||' LAndExp
    void analyzeLOrExp(Node *parents) {
        int cur = 0;
        if (curChildType == LexicalType::LAndExp) {
            analyzeLAndExp(curChild);
        } else {
            while (curChildType == LexicalType::LOrExp) {
                analyzeLOrExp(curChild);
                cur++;
                cur++;
            }
            analyzeLAndExp(curChild);
        }
    }

    //            常量表达式   ConstExp → AddExp 注：使用的Ident 必须是常量
    void analyzeConstExp(Node *parents) {
        int cur = 0;
        analyzeAddExp(curChild, nullptr);
    }
};

#endif //SEMANTICANALYZER_SEMANTICANALYZER_H
