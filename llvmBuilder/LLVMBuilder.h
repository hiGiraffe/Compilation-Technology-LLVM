//
// Created by 11863 on 2023/11/17.
//

#ifndef LLVMBUILDER_LLVMBUILDER_H
#define LLVMBUILDER_LLVMBUILDER_H
using namespace std;

#include "stdlib.h"

#define child(i) curNode->getChild(i)
#define childToken(i) curNode->getChild(i)->getPureToken()
#define childType(i) curNode->getChild(i)->getType()
#define childOp(i) curNode->getChild(i)->getOp()
#define childNum(i) curNode->getChild(i)->getNum()

class LLVMBuilder {
private:
    ofstream &llvmFile_;
    SymbolTable *ancestorTable;
    SymbolTable *curTable;
    Node *ancestor;
    int curRegNum = 0;
//    int labalNum = 0;
    bool stopPrintExp = false;
public:
    explicit LLVMBuilder(SemanticAnalyzer semanticAnalyzer, ofstream &llvmFile)
            : llvmFile_(llvmFile),
              ancestorTable(semanticAnalyzer.getAncestorTable()),
              curTable(ancestorTable),
              ancestor(semanticAnalyzer.getAncestorNode()) {
        visitCompUnit(ancestor);
    }

    //申请寄存器
    int allocReg() {
        return curRegNum++;
    }

//    //申请跳转label
//    string allocLabel() {
//        return "label" + to_string(labalNum++);
//    }

    //输出库函数
    void printDeclare() {
        llvmFile_ << "declare i32 @getint()" << endl;
        llvmFile_ << "declare void @putint(i32)" << endl;
        llvmFile_ << "declare void @putch(i32)" << endl;
        llvmFile_ << "declare void @putstr(i8*)" << endl;
    }

    //输出赋值
    void printExp(string result, string op, string op1, string op2) {
        if (curTable != ancestorTable && !stopPrintExp) {
            llvmFile_ << "    ";
            llvmFile_ << result << " = " << op << " i32 " << op1 << ", " << op2 << endl;
        }
    }

    //输出return int
    void printReturnInt(string value) {
        llvmFile_ << "    ";
        llvmFile_ << "ret i32 " << value << endl;
    }

    //输出return void
    void printReturnVoid() {
        llvmFile_ << "    ";
        llvmFile_ << "ret void" << endl;
    }

    //输出全局变量CONST
    //@a = dso_local constant i32 0
    void printGlobalVarConst(string token, int num) {
        llvmFile_ << "@" << token << " = dso_local constant i32 " << to_string(num) << endl;
    }

    //@b = dso_local constant [2 x i32] [i32 1, i32 2]
    void printGlobalOneDimensionConst(string token, vector<int> oneDimensionNum) {
        llvmFile_ << "@" << token << " = dso_local constant ";
        llvmFile_ << "[" << oneDimensionNum.size() << " x i32] [";
        for (int i = 0; i < oneDimensionNum.size(); i++) {
            if (i != 0)
                llvmFile_ << ", ";
            llvmFile_ << "i32 " << oneDimensionNum.at(i);
        }
        llvmFile_ << "]" << endl;
    }

    //@c = dso_local constant [2 x [3 x i32]] [[3 x i32] [i32 1, i32 2, i32 3], [3 x i32] [i32 4, i32 5, i32 6]]
    void printGlobalTwoDimensionConst(string token, vector<vector<int>> twoDimensionNum) {
        llvmFile_ << "@" << token << " = dso_local constant ";
        llvmFile_ << "[" << twoDimensionNum.size() << " x ["
                  << to_string(twoDimensionNum.at(0).size()) << " x i32]] [";
        for (int i = 0; i < twoDimensionNum.size(); i++) {
            if (i != 0)
                llvmFile_ << ", ";
            llvmFile_ << "[" << twoDimensionNum.at(0).size() << " x i32] [";
            for (int j = 0; j < twoDimensionNum.at(i).size(); j++) {
                if (j != 0)
                    llvmFile_ << ", ";
                llvmFile_ << "i32 " << twoDimensionNum.at(i).at(j);
            }
            llvmFile_ << "]";
        }
        llvmFile_ << "]" << endl;
    }

    //输出全局变量 VAR
    //@a = dso_local global i32 0
    void printGlobalVar(string token, int num) {
        llvmFile_ << "@" << token << " = dso_local global i32 " << to_string(num) << endl;
    }

    //@b = dso_local global [2 x i32] [i32 1, i32 2]
    void printGlobalOneDimension(string token, vector<int> oneDimensionNum) {
        llvmFile_ << "@" << token << " = dso_local global ";
        llvmFile_ << "[" << oneDimensionNum.size() << " x i32] [";
        for (int i = 0; i < oneDimensionNum.size(); i++) {
            if (i != 0)
                llvmFile_ << ", ";
            llvmFile_ << "i32 " << oneDimensionNum.at(i);
        }
        llvmFile_ << "]" << endl;
    }

    //@c = dso_local global [2 x [3 x i32]] [[3 x i32] [i32 1, i32 2, i32 3], [3 x i32] [i32 4, i32 5, i32 6]]
    void printGlobalTwoDimension(string token, vector<vector<int>> twoDimensionNum) {
        llvmFile_ << "@" << token << " = dso_local global ";
        llvmFile_ << "[" << twoDimensionNum.size() << " x ["
                  << to_string(twoDimensionNum.at(0).size()) << " x i32]] [";
        for (int i = 0; i < twoDimensionNum.size(); i++) {
            if (i != 0)
                llvmFile_ << ", ";
            llvmFile_ << "[" << twoDimensionNum.at(0).size() << " x i32] [";
            for (int j = 0; j < twoDimensionNum.at(i).size(); j++) {
                if (j != 0)
                    llvmFile_ << ", ";
                llvmFile_ << "i32 " << twoDimensionNum.at(i).at(j);
            }
            llvmFile_ << "]";
        }
        llvmFile_ << "]" << endl;
    }

    //输出局部变量 VAR或CONST
    void printAllocVar(string regOp) {
        llvmFile_ << "    ";
        llvmFile_ << regOp << " = alloca i32" << endl;
    }

    //  %3 = alloca [2 x i32], align 4
    void printAllocOneDimension(string regOp, int num) {
        llvmFile_ << "    ";
        llvmFile_ << regOp << " = alloca [" << num << " x i32]" << endl;
    }

    //  %4 = alloca [2 x [3 x i32]], align 16
    void printAllocTwoDimension(string regOp, int num1, int num2) {
        llvmFile_ << "    ";
        llvmFile_ << regOp << " = alloca [" << num1 << " x [" << num2 << " x i32]]" << endl;

    }

    //形参alloca
    //%3 = alloca i32*
    void printAllocOneDimensionParam(string regOp) {
        llvmFile_ << "    ";
        llvmFile_ << regOp << " = alloca i32*" << endl;
    }

    //%4 = alloca [3 x i32]*
    void printAllocTwoDimensionParam(string regOp, int num) {
        llvmFile_ << "    ";
        llvmFile_ << regOp << " = alloca [" << num << " x i32]*" << endl;

    }

    //输出load
    void printLoadVar(string lvalOp, string fromOp) {
        if (curTable != ancestorTable) {
            llvmFile_ << "    ";
            llvmFile_ << lvalOp << " = load i32, i32* " << fromOp << endl;
        }
    }

    //读出一维数组 %8 = load i32*, i32* * %5
    void printLoadOneDimensionParam(string lvalOp, string fromOp) {
        llvmFile_ << "    ";
        llvmFile_ << lvalOp << " = load i32*, i32* * " << fromOp << endl;
    }

    //读出二维数组 %12 = load [3 x i32] *, [3 x i32]* * %4
    void printLoadTwoDimensionParam(string lvalOp, string fromOp, int num) {
        llvmFile_ << "    ";
        llvmFile_ << lvalOp << " = load [" << num << " x i32] *, [" << num << " x i32]* * " << fromOp << endl;
    }

    //输出getelementptr 一维数组
    //%11 = getelementptr [6 x i32], [6 x i32]* @a, i32 0, i32 4
    void printGetelementptrOneDimension(string lvalOp, string fromOp, int num, string i) {
        llvmFile_ << "    ";
        llvmFile_ << lvalOp << " = getelementptr [" << num << " x i32], [" << num << " x i32]* " << fromOp
                  << ", i32 0, i32 " << i << endl;
    }

    //输出getelementptr 形参
    //%4 = getelementptr inbounds i32, i32* %3, i64 1
    void printGetelementptrOneDimensionParam(string lvalOp, string fromOp, string i) {
        llvmFile_ << "    ";
        llvmFile_ << lvalOp << " = getelementptr  i32, i32* " << fromOp
                  << ", i32 " << i << endl;
    }

    //输出getelementptr 二维数组
    //%8 = getelementptr [2 x [3 x i32]], [2 x [3 x i32]]*%1, i32 0, i32 0, i32 2
    void printGetelementptrTwoDimension(string lvalOp, string fromOp, int num1, int num2, string i, string j) {
        llvmFile_ << "    ";
        llvmFile_ << lvalOp << " = getelementptr [" << num1 << " x [" << num2
                  << " x i32]], [" << num1 << " x [" << num2 << " x i32]]* " << fromOp << ", i32 0, i32 " << i
                  << ", i32 " << j << endl;
    }

    //输出getelementptr 形参
    //  %4 = getelementptr inbounds [2 x i32], [2 x i32]* %3, i64 1
    //  %5 = getelementptr inbounds [2 x i32], [2 x i32]* %4, i64 0, i64 2
    void printGetelementptrTwoDimensionParam(string lvalOp, string fromOp, int num, string i, string j) {

    }

    //输出对LVal的修改
    void printStoreLVal(string regOp, string storeOp) {
        llvmFile_ << "    ";
        llvmFile_ << "store i32 " << storeOp << ", i32* " << regOp << endl;
    }

    //存进一维数组 store i32* %1, i32* * %5
    void printStoreOneDimensionParam(string regOp, string storeOp) {
        llvmFile_ << "    ";
        llvmFile_ << "store i32* " << storeOp << ", i32** " << regOp << endl;
    }

    //存进二维数组 store [3 x i32]* %2, [3 x i32]* * %4
    void printStoreTwoDimensionParam(string regOp, string storeOp, int num) {
        llvmFile_ << "    ";
        llvmFile_ << "store [" << num << " x i32]* " << storeOp << ", [" << num << " x i32]* * " << regOp << endl;
    }

    //输出getint()
    void printGetint(string regOp) {
        llvmFile_ << "    ";
        llvmFile_ << regOp << " = call i32 @getint()" << endl;
    }


    //编译单元 CompUnit → {Decl} {FuncDef} MainFuncDef
    //声明 Decl → ConstDecl | VarDecl
    void visitCompUnit(Node *curNode) {
        printDeclare();
        int cur = 0;
        while (childType(cur) == LexicalType::VarDecl || childType(cur) == LexicalType::ConstDecl) {
            curRegNum = 0;
            if (childType(cur) == LexicalType::VarDecl) {
                visitVarDecl(child(cur));
            } else {
                visitConstDecl(child(cur));
            }
            cur++;
        }
        while (childType(cur) == LexicalType::FuncDef) {
            curRegNum = 0;
            visitFuncDef(child(cur));
            cur++;
        }
        llvmFile_ << endl;
        curRegNum = 0;
        visitMainFuncDef(child(cur));
    }

    //常量声明 ConstDecl → 'const' BType ConstDef { ',' ConstDef } ';'
    void visitConstDecl(Node *curNode) {
        int cur = 2;
        visitConstDef(child(cur));
        cur++;
        while (childToken(cur) != ";") {
            cur++;
            visitConstDef(child(cur));
            cur++;
        }
    }

    //常数定义 ConstDef → Ident { '[' ConstExp ']' } '=' ConstInitVal
    void visitConstDef(Node *curNode) {
        Symbol *ident = curTable->findIdentInAllTables(child(0)->getPureToken());
        bool isGlobalDecl = curTable->isIdentGlobal(child(0)->getPureToken());
        if (isGlobalDecl)
            ident->setGlobalVaclTrue();

        int dimension; //维度
        switch (ident->getType()) {
            case Symbol::Type::VAR_CONST:
                dimension = 0;
                break;
            case Symbol::Type::ONE_DIMENSION_CONST:
                dimension = 1;
                break;
            case Symbol::Type::TWO_DIMENSIONS_CONST:
                dimension = 2;
                break;
        }

        int cur = 1;
        for (int i = 0; i < dimension; i++) {
            cur++;
            visitConstExp(child(cur));
            ident->addSizeOfArray(child(cur)->getNum());
            cur++;
            cur++;
        }
        cur++;
        //读入具体信息
        visitConstInitVal(child(cur), dimension);
        child(0)->setNum(child(cur)->getNum());

        if (isGlobalDecl) {
            if (dimension == 0)
                printGlobalVarConst(ident->getToken(), child(cur)->getNum());
            else if (dimension == 1)
                printGlobalOneDimensionConst(ident->getToken(), child(cur)->getOneDimensionNum());
            else if (dimension == 2)
                printGlobalTwoDimensionConst(ident->getToken(), child(cur)->getTwoDimensionNum());
        } else {
            int tmpRegNum = allocReg();
            child(0)->setRegNum(tmpRegNum);
            if (dimension == 0) {
                printAllocVar(child(0)->getOp());
                //如果有赋值
                if (cur < curNode->getSize())
                    printStoreLVal(child(0)->getOp(), child(cur)->getOp());
            } else if (dimension == 1) {
                printAllocOneDimension(child(0)->getOp(), ident->getSizeOfArray(0));
                //如果有赋值
                if (cur < curNode->getSize()) {
                    for (int i = 0; i < child(cur)->getOneDimensionNum().size(); i++) {
                        int num = child(cur)->getOneDimensionNum().at(i);
                        int regNum = child(cur)->getOneDimensionRegNum().at(i);

                        int tmpRegNum = allocReg();
                        printGetelementptrOneDimension("%" + to_string(tmpRegNum), child(0)->getOp(),
                                                       child(cur)->getOneDimensionNum().size(), to_string(i));
                        if (regNum == -1)
                            printStoreLVal("%" + to_string(tmpRegNum), to_string(num));
                        else
                            printStoreLVal("%" + to_string(tmpRegNum), "%" + to_string(regNum));
                    }
                }
            } else if (dimension == 2) {
                printAllocTwoDimension(child(0)->getOp(), ident->getSizeOfArray(0), ident->getSizeOfArray(1));
                //如果有赋值
                if (cur < curNode->getSize()) {
                    for (int i = 0; i < child(cur)->getTwoDimensionNum().size(); i++) {
                        for (int j = 0; j < child(cur)->getTwoDimensionNum().at(i).size(); j++) {
                            int num = child(cur)->getTwoDimensionNum().at(i).at(j);
                            int regNum = child(cur)->getTwoDimensionRegNum().at(i).at(j);

                            int tmpRegNum = allocReg();
                            printGetelementptrTwoDimension("%" + to_string(tmpRegNum), child(0)->getOp(),
                                                           child(cur)->getTwoDimensionNum().size(),
                                                           child(cur)->getTwoDimensionNum().at(i).size(), to_string(i),
                                                           to_string(j));
                            if (regNum == -1)
                                printStoreLVal("%" + to_string(tmpRegNum), to_string(num));
                            else
                                printStoreLVal("%" + to_string(tmpRegNum), "%" + to_string(regNum));
                        }
                    }
                }
            }
        }
    }

    //常量初值 ConstInitVal → ConstExp
    //| '{' [ ConstInitVal { ',' ConstInitVal } ] '}'
    void visitConstInitVal(Node *curNode, int dimension) {
        int cur = 0;
        switch (dimension) {
            case 0:
                visitConstExp(child(0));
                curNode->setNum(child(0)->getNum());
                curNode->setRegNum(child(0)->getRegNum());
                break;
            case 1:
                while (childToken(cur) != "}") {
                    cur++;
                    visitConstInitVal(child(cur), 0);
                    curNode->addNumInOneDimension(child(cur)->getNum());
                    curNode->addRegNumInOneDimension(child(cur)->getRegNum());
                    cur++;
                }
                break;
            case 2:
                while (childToken(cur) != "}") {
                    cur++;
                    visitConstInitVal(child(cur), 1);
                    curNode->addVectorInTwoDimension(child(cur)->getOneDimensionNum());
                    curNode->addRegVectorInTwoDimension(child(cur)->getOneDimensionRegNum());
                    cur++;
                }
                break;
        }
    }

    //变量声明 VarDecl → BType VarDef { ',' VarDef } ';'
    void visitVarDecl(Node *curNode) {
        int cur = 1;
        visitVarDef(child(cur));
        cur++;
        while (childToken(cur) != ";") {
            cur++;
            visitVarDef(child(cur));
            cur++;
        }
    }

    //变量定义 VarDef → Ident { '[' ConstExp ']' }
    //| Ident { '[' ConstExp ']' } '=' InitVal
    void visitVarDef(Node *curNode) {
        Symbol *ident = curTable->findIdentInAllTables(child(0)->getPureToken());
        bool isGlobalDecl = curTable->isIdentGlobal(child(0)->getPureToken());
        if (isGlobalDecl)
            ident->setGlobalVaclTrue();

        int dimension; //维度
        switch (ident->getType()) {
            case Symbol::Type::VAR:
                dimension = 0;
                break;
            case Symbol::Type::ONE_DIMENSION:
                dimension = 1;
                break;
            case Symbol::Type::TWO_DIMENSIONS:
                dimension = 2;
                break;
        }

        int cur = 1;
        for (int i = 0; i < dimension; i++) {
            cur++;
            visitConstExp(child(cur));
            ident->addSizeOfArray(child(cur)->getNum());
            cur++;
            cur++;
        }
        cur++;
        //如果有赋值
        if (cur < curNode->getSize()) {
            //读入具体信息
            visitInitVal(child(cur), dimension);
            child(0)->setNum(child(cur)->getNum());
        }


        if (isGlobalDecl) {
            if (cur < curNode->getSize()) { //如果有赋值部分
                if (dimension == 0)
                    printGlobalVar(ident->getToken(), child(cur)->getNum());
                else if (dimension == 1)
                    printGlobalOneDimension(ident->getToken(), child(cur)->getOneDimensionNum());
                else if (dimension == 2)
                    printGlobalTwoDimension(ident->getToken(), child(cur)->getTwoDimensionNum());
            } else { //如果没有赋值，全赋值为0
                if (dimension == 0) {
                    printGlobalVar(ident->getToken(), 0);
                } else if (dimension == 1) {
                    vector<int> oneDimensionNum = {};
                    for (int i = 0; i < ident->getSizeOfArray(0); i++) {
                        oneDimensionNum.push_back(0);
                    }
                    printGlobalOneDimension(ident->getToken(), oneDimensionNum);
                } else if (dimension == 2) {
                    vector<vector<int>> twoDimensionNum = {};
                    for (int i = 0; i < ident->getSizeOfArray(0); i++) {
                        vector<int> oneDimensionNum = {};
                        for (int j = 0; j < ident->getSizeOfArray(1); j++) {
                            oneDimensionNum.push_back(0);
                        }
                        twoDimensionNum.push_back(oneDimensionNum);
                    }
                    printGlobalTwoDimension(ident->getToken(), twoDimensionNum);
                }
            }

        } else {
            int tmpRegNum = allocReg();
            child(0)->setRegNum(tmpRegNum);
            if (dimension == 0) {
                printAllocVar(child(0)->getOp());
                //如果有赋值
                if (cur < curNode->getSize())
                    printStoreLVal(child(0)->getOp(), child(cur)->getOp());
            } else if (dimension == 1) {
                printAllocOneDimension(child(0)->getOp(), ident->getSizeOfArray(0));
                //如果有赋值
                if (cur < curNode->getSize()) {
                    for (int i = 0; i < child(cur)->getOneDimensionNum().size(); i++) {
                        int num = child(cur)->getOneDimensionNum().at(i);
                        int regNum = child(cur)->getOneDimensionRegNum().at(i);

                        int tmpRegNum = allocReg();
                        printGetelementptrOneDimension("%" + to_string(tmpRegNum), child(0)->getOp(),
                                                       child(cur)->getOneDimensionNum().size(), to_string(i));
                        if (regNum == -1)
                            printStoreLVal("%" + to_string(tmpRegNum), to_string(num));
                        else
                            printStoreLVal("%" + to_string(tmpRegNum), "%" + to_string(regNum));
                    }
                }
            } else if (dimension == 2) {
                printAllocTwoDimension(child(0)->getOp(), ident->getSizeOfArray(0), ident->getSizeOfArray(1));
                //如果有赋值
                if (cur < curNode->getSize()) {
                    for (int i = 0; i < child(cur)->getTwoDimensionNum().size(); i++) {
                        for (int j = 0; j < child(cur)->getTwoDimensionNum().at(i).size(); j++) {
                            int num = child(cur)->getTwoDimensionNum().at(i).at(j);
                            int regNum = child(cur)->getTwoDimensionRegNum().at(i).at(j);

                            int tmpRegNum = allocReg();
                            printGetelementptrTwoDimension("%" + to_string(tmpRegNum), child(0)->getOp(),
                                                           child(cur)->getTwoDimensionNum().size(),
                                                           child(cur)->getTwoDimensionNum().at(i).size(), to_string(i),
                                                           to_string(j));
                            if (regNum == -1)
                                printStoreLVal("%" + to_string(tmpRegNum), to_string(num));
                            else
                                printStoreLVal("%" + to_string(tmpRegNum), "%" + to_string(regNum));
                        }
                    }
                }
            }
        }
    }

    //变量初值 InitVal → Exp | '{' [ InitVal { ',' InitVal } ] '}'
    void visitInitVal(Node *curNode, int dimension) {
        int cur = 0;
        switch (dimension) {
            case 0:
                visitExp(child(0));
                curNode->setNum(child(0)->getNum());
                curNode->setRegNum(child(0)->getRegNum());
                break;
            case 1:
                while (childToken(cur) != "}") {
                    cur++;
                    visitInitVal(child(cur), 0);
                    curNode->addNumInOneDimension(child(cur)->getNum());
                    curNode->addRegNumInOneDimension(child(cur)->getRegNum());
                    cur++;
                }
                break;
            case 2:
                while (childToken(cur) != "}") {
                    cur++;
                    visitInitVal(child(cur), 1);
                    curNode->addVectorInTwoDimension(child(cur)->getOneDimensionNum());
                    curNode->addRegVectorInTwoDimension(child(cur)->getOneDimensionRegNum());
                    cur++;
                }
                break;
        }
    }

    //函数定义 FuncDef → FuncType Ident '(' [FuncFParams] ')' Block
    //函数类型 FuncType → 'void' | 'int'
    void visitFuncDef(Node *curNode) {
        int cur = 3;
        curTable = curTable->allocChild();
        //这一函数的符号
        Symbol *funcIdent = curTable->findIdentInAllTables(child(1)->getPureToken());
        llvmFile_ << "define dso_local ";
        if (child(0)->getChild(0)->getPureToken() == "void") {
            llvmFile_ << "void";
        } else {
            llvmFile_ << "i32";
        }
        llvmFile_ << " @" << child(1)->getPureToken() << "(";
        stopPrintExp = true;
        if (childType(cur) == LexicalType::FuncFParams) {
            visitFuncFParams(child(cur), funcIdent);
        }
        stopPrintExp = false;
        llvmFile_ << "){" << endl;
        curRegNum = 0;
        //分配形参寄存器
        Node *FuncFParams = child(cur);
        int curParam = 0;
        while (curParam < FuncFParams->getChildren().size()) {
            Node *FuncFParam = FuncFParams->getChild(curParam);
            FuncFParam->setRegNum(allocReg());
            curParam++;
            curParam++;
        }

        curRegNum++;//基本块

        //对于形参的处理：定义+赋值
        if (childType(cur) == LexicalType::FuncFParams) {
            //获取当前形参
            Node *funcFParams = child(cur);
            int curParam = 0;
            Node *funcFParam = funcFParams->getChild(curParam);
            Node *identNode = funcFParam->getChild(1);
            Symbol *paramIdent = curTable->findIdentInAllTables(identNode->getPureToken());
            //获取形参维度
            int dimension;
            switch (paramIdent->getType()) {
                case Symbol::Type::VAR:
                    dimension = 0;
                    break;
                case Symbol::Type::ONE_DIMENSION:
                    dimension = 1;
                    break;
                case Symbol::Type::TWO_DIMENSIONS:
                    dimension = 2;
                    break;
            }
            int tmpRegNum = allocReg();
            identNode->setRegNum(tmpRegNum);
            if (dimension == 0) {
                printAllocVar(identNode->getOp());
                //赋值
                printStoreLVal(identNode->getOp(), funcFParam->getOp());
            } else if (dimension == 1) {
                printAllocOneDimensionParam(identNode->getOp());
                printStoreOneDimensionParam(identNode->getOp(), funcFParam->getOp());
            } else if (dimension == 2) {
                printAllocTwoDimensionParam(identNode->getOp(), paramIdent->getSizeOfArray(0));
                printStoreTwoDimensionParam(identNode->getOp(), funcFParam->getOp(), paramIdent->getSizeOfArray(0));
            }


            curParam++;
            while (curParam < funcFParams->getSize()) {
                curParam++;
                funcFParam = funcFParams->getChild(curParam);
                identNode = funcFParam->getChild(1);
                paramIdent = curTable->findIdentInAllTables(identNode->getPureToken());
                switch (paramIdent->getType()) {
                    case Symbol::Type::VAR:
                        dimension = 0;
                        break;
                    case Symbol::Type::ONE_DIMENSION:
                        dimension = 1;
                        break;
                    case Symbol::Type::TWO_DIMENSIONS:
                        dimension = 2;
                        break;
                }
                //定义
                tmpRegNum = allocReg();
                identNode->setRegNum(tmpRegNum);
                if (dimension == 0) {
                    printAllocVar(identNode->getOp());
                    //赋值
                    printStoreLVal(identNode->getOp(), funcFParam->getOp());
                } else if (dimension == 1) {
                    printAllocOneDimensionParam(identNode->getOp());
                    printStoreOneDimensionParam(identNode->getOp(), funcFParam->getOp());
                } else if (dimension == 2) {
                    printAllocTwoDimensionParam(identNode->getOp(), paramIdent->getSizeOfArray(0));
                    printStoreTwoDimensionParam(identNode->getOp(), funcFParam->getOp(), paramIdent->getSizeOfArray(0));
                }


                curParam++;
            }
            //这里是外层的cur
            cur++;
        }


        visitBlock(child(cur + 1));
        curTable = curTable->getParents();
        Node *block = child(curNode->getSize() - 1);
        Node *stmt = block->getChild(block->getChildren().size() - 2);

        if (stmt->getType() == LexicalType::LBRACE || stmt->getChild(0)->getType() != LexicalType::RETURNTK)
            printReturnVoid();
        llvmFile_ << "}" << endl;
    }

    //主函数定义 MainFuncDef → 'int' 'main' '(' ')' Block
    void visitMainFuncDef(Node *curNode) {
        llvmFile_ << "define dso_local i32 @main() {" << endl;
        curRegNum++;//基本块
        curTable = curTable->allocChild();
        visitBlock(child(4));
        curTable = curTable->getParents();
        llvmFile_ << "}" << endl;
    }

    //函数形参表 FuncFParams → FuncFParam { ',' FuncFParam }
    void visitFuncFParams(Node *curNode, Symbol *ident) {
        int cur = 0;
        visitFuncFParam(child(cur), ident);
        cur++;
        while (cur < curNode->getSize()) {
            llvmFile_ << ", ";
            cur++;
            visitFuncFParam(child(cur), ident);
            cur++;
        }
    }

    //函数形参 FuncFParam → BType Ident ['[' ']' { '[' ConstExp ']' }]
    void visitFuncFParam(Node *curNode, Symbol *funcIdent) {
        //形参定义在FuncFParam，但是具体的局部变量是存在Ident上，这样每次使用可以访问
        //FIXME:定义在外面，避免在里面输出reg
//        int tmpRegNum = allocReg();
//        curNode->setRegNum(tmpRegNum);

        Symbol *paramIdent = curTable->findIdentInAllTables(child(1)->getPureToken());
        funcIdent->addSymbolOfParam(paramIdent);
        int dimension; //维度
        switch (paramIdent->getType()) {
            case Symbol::Type::VAR:
                dimension = 0;
                break;
            case Symbol::Type::ONE_DIMENSION:
                dimension = 1;
                break;
            case Symbol::Type::TWO_DIMENSIONS:
                dimension = 2;
                break;
        }
//        if (dimension == 0)
//            llvmFile_ << "i32 " << curNode->getOp();
//        else if (dimension == 1) {
//            llvmFile_ << "i32* " << curNode->getOp();
//        } else {
//            visitConstExp(child(5));
//            paramIdent->addSizeOfArray(child(5)->getNum());
//            llvmFile_ << "[" << child(5)->getNum() << " x i32]* " << curNode->getOp();
//        }
        if (dimension == 0)
            llvmFile_ << "i32";
        else if (dimension == 1) {
            llvmFile_ << "i32*";
        } else {
            visitConstExp(child(5));
            paramIdent->addSizeOfArray(child(5)->getNum());
            llvmFile_ << "[" << child(5)->getNum() << " x i32]*";
        }
    }

    //语句块 Block → '{' { BlockItem } '}'
    //语句块项 BlockItem → Decl | Stmt
    void visitBlock(Node *curNode) {
        int cur = 1;
        while (childToken(cur) != "}") {
            switch (childType(cur)) {
                case LexicalType::ConstDecl:
                    visitConstDecl(child(cur));
                    break;
                case LexicalType::VarDecl:
                    visitVarDecl(child(cur));
                    break;
                case LexicalType::Stmt:
                    visitStmt(child(cur));
                    break;
            }
            cur++;
        }
    }


    //语句 Stmt → 1 LVal '=' Exp ';' // 每种类型的语句都要覆盖
    //|1  [Exp] ';' //有无Exp两种情况
    //|1 Block
    //|1 'if' '(' Cond ')' Stmt [ 'else' Stmt ] // 1.有else 2.无else
    //| 'for' '(' [ForStmt] ';' [Cond] ';' [forStmt] ')' Stmt
    //| 'break' ';' | 'continue' ';'
    //|1 'return' [Exp] ';' // 1.有Exp 2.无Exp
    //|1 LVal '=' 'getint''('')'';'
    //|1 'printf''('FormatString{','Exp}')'';' // 1.有Exp 2.无Exp
    void visitStmt(Node *curNode) {
        if (childType(0) == LexicalType::RETURNTK) {
            if (childToken(1) != ";") {
                visitExp(child(1));
            }
            Node *parent = curNode->getParent();
            while (parent->getType() != LexicalType::FuncDef && parent->getType() != LexicalType::MainFuncDef) {
                parent = parent->getParent();
            }
            //todo:函数和变量重名
            Symbol *ident = curTable->findIdentInAllTables(
                    parent->getChild(1)->getPureToken());
            if (ident->getLine() > parent->getChild(1)->getLine()) {
                ident = curTable->findIdentInParentTable(parent->getChild(1)->getPureToken());
            }
            string value = childOp(1);
            if (ident->getType() == Symbol::Type::FUNCTION_INT) {
                printReturnInt(value);
            } else if (ident->getType() == Symbol::Type::FUNCTION_VOID) {
                printReturnVoid();
            }
            allocReg();
        } else if (childType(0) == LexicalType::Block) {
            //分配block
            curTable = curTable->allocChild();
            visitBlock(child(0));
            curTable = curTable->getParents();
        } else if (childType(0) == LexicalType::LVal) {
            //fixme:这里是赋值LVal
            Symbol *ident = curTable->findIdentInAllTables(child(0)->getChild(0)->getPureToken());
            if (ident->getLine() > child(0)->getChild(0)->getLine()) {
                ident = curTable->findIdentInParentTable(child(0)->getChild(0)->getPureToken());
            }
            int dimension; //维度
            switch (ident->getType()) {
                case Symbol::Type::VAR:
                    dimension = 0;
                    break;
                case Symbol::Type::ONE_DIMENSION:
                    dimension = 1;
                    break;
                case Symbol::Type::TWO_DIMENSIONS:
                    dimension = 2;
                    break;
            }
            if (childType(2) == LexicalType::Exp) {
                if (dimension == 0) {
                    visitExp(child(2));
                    printStoreLVal(ident->getOp(), child(2)->getOp());
                    ident->setNum(child(2)->getNum());
                } else if (dimension == 1) {
                    visitExp(child(2));
                    visitExp(child(0)->getChild(2));
                    int tmpRegNum = allocReg();
                    //如果不是形参
                    //  %5 = getelementptr [7 x i32], [7 x i32]* %2, i64 0, i64 1
                    //  store i32 3, i32* %5, align 4
                    if (ident->getNode()->getParent()->getType() != LexicalType::FuncFParam) {
                        printGetelementptrOneDimension("%" + to_string(tmpRegNum), ident->getOp(),
                                                       ident->getSizeOfArray(0), child(0)->getChild(2)->getOp());

                        printStoreLVal("%" + to_string(tmpRegNum), child(2)->getOp());
                    }
                        //如果是形参
                    else {
                        printLoadOneDimensionParam("%" + to_string(tmpRegNum), ident->getOp());
                        int regNum = allocReg();
                        printGetelementptrOneDimensionParam("%" + to_string(regNum), "%" + to_string(tmpRegNum),
                                                            child(0)->getChild(2)->getOp());
                        printStoreLVal("%" + to_string(regNum), child(2)->getOp());
                    }
                } else if (dimension == 2) {
                    visitExp(child(2));
                    visitExp(child(0)->getChild(2));
                    visitExp(child(0)->getChild(5));
                    int tmpRegNum = allocReg();
                    //如果`不是形参`
                    //  %7 = getelementptr [2 x [3 x i32]], [2 x [3 x i32]]* %3, i64 0, i64 1
                    //  %8 = getelementptr [3 x i32], [3 x i32]* %7, i64 0, i64 2
                    //  store i32 1, i32* %8, align 4
                    if (ident->getNode()->getParent()->getType() != LexicalType::FuncFParam) {
                        printGetelementptrTwoDimension("%" + to_string(tmpRegNum), ident->getOp(),
                                                       ident->getSizeOfArray(0), ident->getSizeOfArray(1),
                                                       child(0)->getChild(2)->getOp(),
                                                       child(0)->getChild(5)->getOp());
                        printStoreLVal("%" + to_string(tmpRegNum), child(2)->getOp());
                    }
                        //如果是形参
                        //  %6 = load [2 x i32]*, [2 x i32]** %3, align 8
                        //  %7 = getelementptr inbounds [2 x i32], [2 x i32]* %6, i64 1
                        //  %8 = getelementptr inbounds [2 x i32], [2 x i32]* %7, i64 0, i64 1
                        //  store i32 %5, i32* %8, align 4
                    else {
                        printLoadTwoDimensionParam("%" + to_string(tmpRegNum), ident->getOp(),
                                                   ident->getSizeOfArray(0));
                        int regNum1 = allocReg();
                        llvmFile_ << "    ";
                        llvmFile_ << "%" + to_string(regNum1) << " = getelementptr [" << ident->getSizeOfArray(0)
                                  << " x i32], [" << ident->getSizeOfArray(0) << " x i32]* "
                                  << "%" + to_string(tmpRegNum)
                                  << ", i32 " << child(0)->getChild(2)->getOp() << endl;
                        int regNum2 = allocReg();
                        llvmFile_ << "    ";
                        llvmFile_ << "%" + to_string(regNum2) << " = getelementptr [" << ident->getSizeOfArray(0)
                                  << " x i32], [" << ident->getSizeOfArray(0) << " x i32]* "
                                  << "%" + to_string(regNum1)
                                  << ", i32 0, i32 " << child(0)->getChild(5)->getOp() << endl;
                        printStoreLVal("%" + to_string(regNum2), child(2)->getOp());
                    }
                }
            } else { //getint
                //右侧的结果
                int tmpRegNum = allocReg();
                child(2)->setRegNum(tmpRegNum);
                printGetint(child(2)->getOp());

                if (dimension == 0) {
                    printStoreLVal(ident->getOp(), child(2)->getOp());
                    ident->setNum(child(2)->getNum());
                } else if (dimension == 1) {

                    visitExp(child(0)->getChild(2));
                    int tmpRegNum = allocReg();
                    //如果不是形参
                    //  %5 = getelementptr [7 x i32], [7 x i32]* %2, i64 0, i64 1
                    //  store i32 3, i32* %5, align 4
                    if (ident->getNode()->getParent()->getType() != LexicalType::FuncFParam) {
                        printGetelementptrOneDimension("%" + to_string(tmpRegNum), ident->getOp(),
                                                       ident->getSizeOfArray(0), child(0)->getChild(2)->getOp());

                        printStoreLVal("%" + to_string(tmpRegNum), child(2)->getOp());
                    }
                        //如果是形参
                    else {
                        printLoadOneDimensionParam("%" + to_string(tmpRegNum), ident->getOp());
                        int regNum = allocReg();
                        printGetelementptrOneDimensionParam("%" + to_string(regNum), "%" + to_string(tmpRegNum),
                                                            child(0)->getChild(2)->getOp());
                        printStoreLVal("%" + to_string(regNum), child(2)->getOp());
                    }
                } else if (dimension == 2) {

                    visitExp(child(0)->getChild(2));
                    visitExp(child(0)->getChild(5));
                    int tmpRegNum = allocReg();
                    //如果`不是形参`
                    //  %7 = getelementptr [2 x [3 x i32]], [2 x [3 x i32]]* %3, i64 0, i64 1
                    //  %8 = getelementptr [3 x i32], [3 x i32]* %7, i64 0, i64 2
                    //  store i32 1, i32* %8, align 4
                    if (ident->getNode()->getParent()->getType() != LexicalType::FuncFParam) {
                        printGetelementptrTwoDimension("%" + to_string(tmpRegNum), ident->getOp(),
                                                       ident->getSizeOfArray(0), ident->getSizeOfArray(1),
                                                       child(0)->getChild(2)->getOp(),
                                                       child(0)->getChild(5)->getOp());
                        printStoreLVal("%" + to_string(tmpRegNum), child(2)->getOp());
                    }
                        //如果是形参
                        //  %6 = load [2 x i32]*, [2 x i32]** %3, align 8
                        //  %7 = getelementptr inbounds [2 x i32], [2 x i32]* %6, i64 1
                        //  %8 = getelementptr inbounds [2 x i32], [2 x i32]* %7, i64 0, i64 1
                        //  store i32 %5, i32* %8, align 4
                    else {
                        printLoadTwoDimensionParam("%" + to_string(tmpRegNum), ident->getOp(),
                                                   ident->getSizeOfArray(0));
                        int regNum1 = allocReg();
                        llvmFile_ << "    ";
                        llvmFile_ << "%" + to_string(regNum1) << " = getelementptr [" << ident->getSizeOfArray(0)
                                  << " x i32], [" << ident->getSizeOfArray(0) << " x i32]* "
                                  << "%" + to_string(tmpRegNum)
                                  << ", i32 " << child(0)->getChild(2)->getOp() << endl;
                        int regNum2 = allocReg();
                        llvmFile_ << "    ";
                        llvmFile_ << "%" + to_string(regNum2) << " = getelementptr [" << ident->getSizeOfArray(0)
                                  << " x i32], [" << ident->getSizeOfArray(0) << " x i32]* "
                                  << "%" + to_string(regNum1)
                                  << ", i32 0, i32 " << child(0)->getChild(5)->getOp() << endl;
                        printStoreLVal("%" + to_string(regNum2), child(2)->getOp());
                    }
                }
            }
        } else if (childType(0) == LexicalType::Exp) {
            visitExp(child(0));
        } else if (childToken(0) == ";") {

        } else if (childType(0) == LexicalType::PRINTFTK) {
            string str = child(2)->getPureToken();
            int cur = 4;
            for (int i = 1; i < str.size() - 1; i++) {
                if (str.at(i) == '%') {
                    visitExp(child(cur));
                    llvmFile_ << "    call void @putint(i32 " << child(cur)->getOp() << ")" << endl;
                    cur += 2;
                    i++;
                } else if (str.at(i) == '\\') {
                    llvmFile_ << "    call void @putch(i32 " << 10 << ")" << endl;
                    i++;
                } else {
                    llvmFile_ << "    call void @putch(i32 " << static_cast<int>( str.at(i)) << ")" << endl;
                }
            }
        } else if (childType(0) == LexicalType::IFTK) {
            //如果有else
            if (curNode->getSize() == 7) {

                string endLabel = child(0)->getLabel();
                //成立去if 不成立去else
                visitCond(child(2), child(4)->getLabel(), child(6)->getLabel());
                //if语句块+跳转
                llvmFile_ << child(4)->getLabel() << ":" << endl;
                visitStmt(child(4));
                llvmFile_ << "    br label %" << endLabel << endl;
                //else语句块+跳转
                llvmFile_ << child(6)->getLabel() << ":" << endl;
                visitStmt(child(6));
                llvmFile_ << "    br label %" << endLabel << endl;
                //if结束
                llvmFile_ << endLabel << ":" << endl;
            } else {
                string endLabel = child(0)->getLabel();
                //成立去if 不成立退出
                visitCond(child(2), child(4)->getLabel(), endLabel);
                //if语句块+跳转
                llvmFile_ << child(4)->getLabel() << ":" << endl;
                visitStmt(child(4));
                llvmFile_ << "    br label %" << endLabel << endl;
                //if结束
                llvmFile_ << endLabel << ":" << endl;
            }
        } else if (childType(0) == LexicalType::FORTK) {
            //'for' '(' [ForStmt1] ';' [Cond] ';' [forStmt2] ')' Stmt
            string condLabel = "", forstmt2Label = "", stmtLabel = "", endLabel = child(0)->getLabel();
            //查询可用基本块
            int cur = 2;
            if (childToken(cur) != ";")
                cur++;
            cur++;
            if (childToken(cur) != ";") {
                condLabel = child(cur)->getLabel();
                cur++;
            }
            cur++;
            if (childToken(cur) != ")") {
                forstmt2Label = child(cur)->getLabel();
                cur++;
            }
            cur++;
            stmtLabel = child(cur)->getLabel();

            //往上传
            if (condLabel == "")
                condLabel = stmtLabel;
            if (forstmt2Label == "")
                forstmt2Label = condLabel;

            //第二次循环
            cur = 2;
            if (childToken(cur) != ";") {
                visitForStmt(child(cur));
                cur++;
            }
            llvmFile_ << "    br label %" << condLabel << endl;
            cur++;

            if (childToken(cur) != ";") {
                //Cond开始
                llvmFile_ << child(cur)->getLabel() << ":" << endl;
                visitCond(child(cur), stmtLabel, endLabel);
                cur++;
            }
            cur++;
            if (childToken(cur) != ")") {
                //forstmt2开始
                llvmFile_ << child(cur)->getLabel() << ":" << endl;
                visitForStmt(child(cur));
                llvmFile_ << "    br label %" << condLabel << endl;
                cur++;
            }
            cur++;

            //Stmt开始
            llvmFile_ << child(cur)->getLabel() << ":" << endl;
            visitStmt(child(cur));
            llvmFile_ << "    br label %" << forstmt2Label << endl;

            //结束for
            llvmFile_ << child(0)->getLabel() << ":" << endl;

        } else if (childType(0) == LexicalType::BREAKTK) {
            Node *tmp = curNode->getParent();
            while (tmp != ancestor) {
                //是for块中
                if (tmp->getChild(0)->getPureToken() == "for") {
                    tmp = tmp->getChild(0);
                    break;
                }
                tmp = tmp->getParent();
            }
            allocReg();
            llvmFile_ << "    br label %" << tmp->getLabel() << endl;
        } else if (childType(0) == LexicalType::CONTINUETK) {
            Node *tmp = curNode->getParent();
            string condLabel = "", forstmt2Label = "", stmtLabel = "", endLabel = child(0)->getLabel();

            while (tmp != ancestor) {
                //是for块中
                if (tmp->getChild(0)->getPureToken() == "for") {
                    //查询可用基本块
                    int cur = 2;
                    if (tmp->getChild(cur)->getPureToken() != ";")
                        cur++;
                    cur++;
                    if (tmp->getChild(cur)->getPureToken() != ";") {
                        condLabel = tmp->getChild(cur)->getLabel();
                        cur++;
                    }
                    cur++;
                    if (tmp->getChild(cur)->getPureToken() != ")") {
                        forstmt2Label = tmp->getChild(cur)->getLabel();
                        cur++;
                    }
                    cur++;
                    stmtLabel = tmp->getChild(cur)->getLabel();

                    //往上传
                    if (condLabel == "")
                        condLabel = stmtLabel;
                    if (forstmt2Label == "")
                        forstmt2Label = condLabel;

                    break;
                }
                tmp = tmp->getParent();
            }
            allocReg();
            llvmFile_ << "    br label %" << forstmt2Label << endl;
        } else {

        }


    }

    //语句 ForStmt → LVal '=' Exp
    void visitForStmt(Node *curNode) {
        Symbol *ident = curTable->findIdentInAllTables(child(0)->getChild(0)->getPureToken());
        if (ident->getLine() > child(0)->getChild(0)->getLine()) {
            ident = curTable->findIdentInParentTable(child(0)->getChild(0)->getPureToken());
        }
        visitExp(child(2));
        printStoreLVal(ident->getOp(), child(2)->getOp());
        ident->setNum(child(2)->getNum());
    }

    //表达式 Exp → AddExp
    void visitExp(Node *curNode) {
        visitAddExp(child(0));
        curNode->setRegNum(child(0)->getRegNum());
        curNode->setNum(child(0)->getNum());
    }

    //条件表达式 Cond → LOrExp
    void visitCond(Node *curNode, string trueLabel, string falseLabel) {
        //这里直接跳过一个LOrExp，进入||
        visitLOrExp(child(0), trueLabel, falseLabel);
    }

    //使用左值表达式 LVal → Ident {'[' Exp ']'}
    void visitLVal(Node *curNode) {
        Symbol *ident = curTable->findIdentInAllTables(child(0)->getPureToken());
        if (ident->getLine() > child(0)->getLine()) {
            ident = curTable->findIdentInParentTable(child(0)->getPureToken());
        }
        int dimension; //维度
        switch (ident->getType()) {
            case Symbol::Type::VAR_CONST:
            case Symbol::Type::VAR:
            case Symbol::Type::FUNCTION_INT:
                dimension = 0;
                break;
            case Symbol::Type::ONE_DIMENSION_CONST:
            case Symbol::Type::ONE_DIMENSION:
                dimension = 1;
                break;
            case Symbol::Type::TWO_DIMENSIONS_CONST:
            case Symbol::Type::TWO_DIMENSIONS:
                dimension = 2;
                break;
        }

        if (dimension == 0) {
            //使用：从定义的地方load过来
            int tmpRegNum = allocReg();
            curNode->setRegNum(tmpRegNum);
            curNode->setNum(ident->getNum());
            printLoadVar("%" + to_string(tmpRegNum), ident->getOp());
        } else if (dimension == 1) {
            if (ident->getNode()->getParent()->getType() != LexicalType::FuncFParam) {//不是形参
                //没有[]
                if (curNode->getChildren().size() == 1) {
                    int tmpRegNum = allocReg();
                    curNode->setRegNum(tmpRegNum);
                    printGetelementptrOneDimension("%" + to_string(tmpRegNum), ident->getOp(),
                                                   ident->getSizeOfArray(0), to_string(0));
                }
                    //一个[]
                else if (curNode->getChildren().size() == 4) {

                    visitExp(child(2));
                    if (!ident->isGlobal() && ident->getNode()->getParent()->getChild(
                            ident->getNode()->getParent()->getChildren().size() - 1)->getOneDimensionNum().size() != 0)
                        curNode->setNum(ident->getNode()->getParent()->getChild(
                                ident->getNode()->getParent()->getChildren().size() - 1)->getOneDimensionNum().at(
                                child(2)->getNum()));
                    int tmpRegNum = allocReg();
                    printGetelementptrOneDimension("%" + to_string(tmpRegNum), ident->getOp(),
                                                   ident->getSizeOfArray(0), child(2)->getOp());

                    int regNum = allocReg();
                    curNode->setRegNum(regNum);
                    printLoadVar("%" + to_string(regNum), "%" + to_string(tmpRegNum));

                }
            } else { //是形参
                //没有[]
                //%8 = load i32*, i32** %5, align 8
                if (curNode->getChildren().size() == 1) {
                    int tmpRegNum = allocReg();
                    curNode->setRegNum(tmpRegNum);
                    printLoadOneDimensionParam("%" + to_string(tmpRegNum), ident->getOp());
                }
                    //一个[]
                    //    %6 = load i32*, i32* * %3		y
                    //    %7 = getelementptr i32, i32* %6, i32 2	y[2]地址
                    //    %8 = load i32, i32* %7			y[2]
                else if (curNode->getChildren().size() == 4) {
                    visitExp(child(2));
                    int tmpRegNum1 = allocReg();
                    printLoadOneDimensionParam("%" + to_string(tmpRegNum1), ident->getOp());
                    int tmpRegNum2 = allocReg();
                    llvmFile_ << "    " << "%" + to_string(tmpRegNum2) << " = getelementptr i32, i32* "
                              << "%" + to_string(tmpRegNum1) << ", i32 " << child(2)->getOp() << endl;
                    int regNum = allocReg();
                    curNode->setRegNum(regNum);
                    printLoadVar("%" + to_string(regNum), "%" + to_string(tmpRegNum2));
                }
            }

        } else if (dimension == 2) {
            if (ident->getNode()->getParent()->getType() != LexicalType::FuncFParam) { //不是形参
                //没有[]
                if (curNode->getChildren().size() == 1) {
                    int tmpRegNum = allocReg();
                    curNode->setRegNum(tmpRegNum);
                    llvmFile_ << "    ";
                    llvmFile_ << "%" + to_string(tmpRegNum) << " = getelementptr [" << ident->getSizeOfArray(0)
                              << " x [" << ident->getSizeOfArray(1)
                              << " x i32]], [" << ident->getSizeOfArray(0) << " x [" << ident->getSizeOfArray(1)
                              << " x i32]]* " << ident->getOp() << ", i32 0, i32 " << to_string(0)
                              << endl;
                }
                    //一个[]
                else if (curNode->getChildren().size() == 4) {

                    visitExp(child(2));
                    int tmpRegNum = allocReg();
                    curNode->setRegNum(tmpRegNum);
                    printGetelementptrTwoDimension("%" + to_string(tmpRegNum), ident->getOp(),
                                                   ident->getSizeOfArray(0), ident->getSizeOfArray(1),
                                                   child(2)->getOp(),
                                                   to_string(0));
                }
                    //两个[]
                else if (curNode->getChildren().size() == 7) {

                    visitExp(child(2));
                    visitExp(child(5));
                    int tmpRegNum = allocReg();
                    if (!ident->isGlobal() && ident->getNode()->getParent()->getChild(
                            ident->getNode()->getParent()->getChildren().size() - 1)->getTwoDimensionNum().size() != 0)
                        curNode->setNum(ident->getNode()->getParent()->getChild(
                                ident->getNode()->getParent()->getChildren().size() - 1)->getTwoDimensionNum().at(
                                child(2)->getNum()).at(child(5)->getNum()));

                    printGetelementptrTwoDimension("%" + to_string(tmpRegNum), ident->getOp(),
                                                   ident->getSizeOfArray(0), ident->getSizeOfArray(1),
                                                   child(2)->getOp(),
                                                   child(5)->getOp());
                    int regNum = allocReg();
                    curNode->setRegNum(regNum);
                    printLoadVar("%" + to_string(regNum), "%" + to_string(tmpRegNum));
                }
            } else { //是形参
                //没有[]
                //%10 = load [3 x i32]*, [3 x i32]** %2, align 8
                if (curNode->getChildren().size() == 1) {
                    int tmpRegNum = allocReg();
                    curNode->setRegNum(tmpRegNum);
                    printLoadTwoDimensionParam("%" + to_string(tmpRegNum), ident->getOp(), ident->getSizeOfArray(0));
                }
                    //一个[]
                    //  %7 = load [3 x i32]*, [3 x i32]** %2, align 8
                    //  %8 = getelementptr inbounds [3 x i32], [3 x i32]* %7, i64 2
                    //  %9 = getelementptr inbounds [3 x i32], [3 x i32]* %8, i32 0, i32 0
                else if (curNode->getChildren().size() == 4) {

                    visitExp(child(2));
                    int tmpRegNum1 = allocReg();
                    printLoadTwoDimensionParam("%" + to_string(tmpRegNum1), ident->getOp(),
                                               ident->getSizeOfArray(0));
                    int tmpRegNum2 = allocReg();
                    llvmFile_ << "    " << "%" + to_string(tmpRegNum2) << " = getelementptr ["
                              << ident->getSizeOfArray(0) << " x i32], [" << ident->getSizeOfArray(0) << " x i32]* "
                              << "%" + to_string(tmpRegNum1) << ", i32 " << child(2)->getOp() << endl;
                    int tmpRegNum3 = allocReg();
                    llvmFile_ << "    " << "%" + to_string(tmpRegNum3) << " = getelementptr ["
                              << ident->getSizeOfArray(0) << " x i32], [" << ident->getSizeOfArray(0) << " x i32]* "
                              << "%" + to_string(tmpRegNum2) << ", i32 0, i32 0" << endl;

                    curNode->setRegNum(tmpRegNum3);
                }
                    //两个[]
                    //  %3 = load [3 x i32]*, [3 x i32]** %2, align 8
                    //  %4 = getelementptr inbounds [3 x i32], [3 x i32]* %3, i64 0
                    //  %5 = getelementptr inbounds [3 x i32], [3 x i32]* %4, i64 0, i64 3
                    //  %6 = load i32, i32* %5, align 4
                else if (curNode->getChildren().size() == 7) {

                    visitExp(child(2));
                    visitExp(child(5));
                    int tmpRegNum1 = allocReg();
                    printLoadTwoDimensionParam("%" + to_string(tmpRegNum1), ident->getOp(),
                                               ident->getSizeOfArray(0));
                    int tmpRegNum2 = allocReg();
                    llvmFile_ << "    " << "%" + to_string(tmpRegNum2) << " = getelementptr ["
                              << ident->getSizeOfArray(0) << " x i32], [" << ident->getSizeOfArray(0) << " x i32]* "
                              << "%" + to_string(tmpRegNum1) << ", i32 " << child(2)->getOp() << endl;
                    int tmpRegNum3 = allocReg();
                    llvmFile_ << "    " << "%" + to_string(tmpRegNum3) << " = getelementptr ["
                              << ident->getSizeOfArray(0) << " x i32], [" << ident->getSizeOfArray(0) << " x i32]* "
                              << "%" + to_string(tmpRegNum2) << ", i32 0, i32 " << child(5)->getOp() << endl;

                    int regNum = allocReg();
                    curNode->setRegNum(regNum);
                    printLoadVar("%" + to_string(regNum), "%" + to_string(tmpRegNum3));
                }
            }
        }
    }

    //基本表达式 PrimaryExp → '(' Exp ')' | LVal | Number
    void visitPrimaryExp(Node *curNode) {
        if (childType(0) == LexicalType::Number) {
            visitNumber(child(0));
            curNode->setRegNum(child(0)->getRegNum());
            curNode->setNum(child(0)->getNum());
        } else if (childType(0) == LexicalType::LVal) {
            visitLVal(child(0));
            curNode->setRegNum(child(0)->getRegNum());
            curNode->setNum(child(0)->getNum());
        } else {
            visitExp(child(1));
            curNode->setRegNum(child(1)->getRegNum());
            curNode->setNum(child(1)->getNum());
        }
    }

    //数值 Number → IntConst
    void visitNumber(Node *curNode) {
        int num = stoi(childToken(0));
        curNode->setNum(num);
    }

    //一元表达式 UnaryExp → PrimaryExp | Ident '(' [FuncRParams] ')'
    //| UnaryOp UnaryExp
    //单目运算符 UnaryOp → '+' | '−' | '!'
    void visitUnaryExp(Node *curNode) {
        if (childType(0) == LexicalType::PrimaryExp) {
            visitPrimaryExp(child(0));
            curNode->setRegNum(child(0)->getRegNum());
            curNode->setNum(child(0)->getNum());
        } else if (childType(0) == LexicalType::UnaryOp) {
            visitUnaryExp(child(1));
            if (child(0)->getChild(0)->getPureToken() == "+") {
                curNode->setRegNum(child(1)->getRegNum());
                curNode->setNum(child(1)->getNum());
            } else if (child(0)->getChild(0)->getPureToken() == "-") {
                int tmpRegNum = allocReg();
                string tmpResult = "%" + to_string(tmpRegNum);
                printExp(tmpResult, "sub", "0", childOp(1));
                curNode->setRegNum(tmpRegNum);
                curNode->setNum(-child(1)->getNum());
            } else if (child(0)->getChild(0)->getPureToken() == "!") {
                string tmpResult1 = "%" + to_string(allocReg());
                int tmpRegNum = allocReg();
                if (curTable != ancestorTable) { //如果不是全局
                    //判断跟0有没有区别
                    llvmFile_ << "    " << tmpResult1 << " = icmp eq i32 0, " << childOp(1) << endl;
                    //化为32位
                    string tmpResult32 = "%" + to_string(tmpRegNum);
                    llvmFile_ << "    " << tmpResult32 << " = zext i1 " << tmpResult1 << " to i32" << endl;
                }
                curNode->setRegNum(tmpRegNum);
                curNode->setNum(child(1)->getNum() == 0);

            }

        } else if (childType(0) == LexicalType::IDENFR) {
            Symbol *ident = curTable->findIdentInAllTables(child(0)->getPureToken());
            if (ident->getLine() > child(0)->getLine()) {
                ident = curTable->findIdentInParentTable(child(0)->getPureToken());
            }
            //先计算参数
            if (childType(2) == LexicalType::FuncRParams) {
                for (int i = 0; i < ident->getSizeOfParams(); i++) {
                    visitExp(child(2)->getChild(2 * i));
                }
            }
            //再输出
            if (ident->getType() == Symbol::Type::FUNCTION_INT) { //如果是int函数
                int tmpRegNum = allocReg();
                curNode->setRegNum(tmpRegNum);
                //输出引用
                llvmFile_ << "    ";
                llvmFile_ << curNode->getOp() << " = call i32 @" << ident->getToken() << "(";
                if (childType(2) == LexicalType::FuncRParams) {
                    visitFuncRParams(child(2), ident);
                }
                llvmFile_ << ")" << endl;
            } else { //如果是void函数
                //输出引用
                llvmFile_ << "    ";
                llvmFile_ << "call ";
                if (ident->getType() == Symbol::Type::FUNCTION_INT)
                    llvmFile_ << "i32";
                else
                    llvmFile_ << "void";

                llvmFile_ << " @" << ident->getToken() << "(";
                if (childType(2) == LexicalType::FuncRParams) {
                    visitFuncRParams(child(2), ident);
                }
                llvmFile_ << ")" << endl;
            }

        }
    }

    //函数实参表 FuncRParams → Exp { ',' Exp }
    void visitFuncRParams(Node *curNode, Symbol *ident) {
        int getSizeOfParams = ident->getSizeOfParams();
        for (int i = 0; i < getSizeOfParams; i++) {
            Symbol *symbol = ident->getSymbol(i);
            if (i != 0)
                llvmFile_ << ", ";

            if (symbol->getType() == Symbol::Type::VAR)
                llvmFile_ << "i32 ";
            else if (symbol->getType() == Symbol::Type::ONE_DIMENSION)
                llvmFile_ << "i32* ";
            else if (symbol->getType() == Symbol::Type::TWO_DIMENSIONS)
                llvmFile_ << "[" << symbol->getSizeOfArray(0) << " x i32]* ";

            llvmFile_ << child(2 * i)->getOp();
        }
    }

    //乘除模表达式 MulExp → UnaryExp | MulExp ('*' | '/' | '%') UnaryExp
    void visitMulExp(Node *curNode) {
        if (childType(0) == LexicalType::UnaryExp) {
            visitUnaryExp(child(0));

            curNode->setRegNum(child(0)->getRegNum());
            curNode->setNum(child(0)->getNum());
        } else {
            int cur = 0;
            int tmpNum = 0;
            int tmpNum1 = 0;
            string tmpOp1;
            string tmpResult;
            string op;

            visitMulExp(child(cur));
            tmpOp1 = childOp(cur);
            tmpNum1 = childNum(cur);
            cur++;
            if (childToken(cur) == "*") {
                op = "mul";
            } else if (childToken(cur) == "/") {
                op = "sdiv";
            } else {
                op = "srem";
            }
            cur++;

            while (childType(cur) == LexicalType::MulExp) {
                visitMulExp(child(cur));
                tmpResult = "%" + to_string(allocReg());
                printExp(tmpResult, op, tmpOp1, childOp(cur));
                if (op == "mul") {
                    tmpNum = tmpNum1 * childNum(cur);
                } else if (op == "sdiv") {
                    if (childNum(cur) != 0)
                        tmpNum = tmpNum1 / childNum(cur);
                } else {
                    if (childNum(cur) != 0)
                        tmpNum = tmpNum1 % childNum(cur);
                }
                tmpOp1 = tmpResult;
                tmpNum1 = tmpNum;
                cur++;
                if (childToken(cur) == "*") {
                    op = "mul";
                } else if (childToken(cur) == "/") {
                    op = "sdiv";
                } else {
                    op = "srem";
                }
                cur++;
            }
            visitUnaryExp(child(cur));
            int tmpRegNum = allocReg();
            tmpResult = "%" + to_string(tmpRegNum);
            printExp(tmpResult, op, tmpOp1, childOp(cur));
            if (op == "mul") {
                tmpNum = tmpNum1 * childNum(cur);
            } else if (op == "sdiv") {
                if (childNum(cur) != 0)
                    tmpNum = tmpNum1 / childNum(cur);
            } else {
                if (childNum(cur) != 0)
                    tmpNum = tmpNum1 % childNum(cur);
            }

            curNode->setRegNum(tmpRegNum);
            curNode->setNum(tmpNum);
        }
    }

    //加减表达式 AddExp → MulExp | AddExp ('+' | '−') MulExp
    void visitAddExp(Node *curNode) {
        if (childType(0) == LexicalType::MulExp) {
            visitMulExp(child(0));

            curNode->setRegNum(child(0)->getRegNum());
            curNode->setNum(child(0)->getNum());
        } else {
            int cur = 0;
            int tmpNum = 0;
            int tmpNum1 = 0;
            string tmpOp1;
            string tmpResult;
            string op;
            //第一个addExp
            visitAddExp(child(cur));
            tmpOp1 = childOp(cur);
            tmpNum1 = childNum(cur);
            cur++;
            op = childToken(cur) == "+" ? "add" : "sub";
            cur++;

            //假如还有addExp
            while (childType(cur) == LexicalType::AddExp) {
                visitAddExp(child(cur));
                tmpResult = "%" + to_string(allocReg());
                printExp(tmpResult, op, tmpOp1, childOp(cur));
                if (op == "add") {
                    tmpNum = tmpNum1 + childNum(cur);
                } else {
                    tmpNum = tmpNum1 - childNum(cur);
                }
                tmpOp1 = tmpResult;
                tmpNum1 = tmpNum;
                cur++;
                op = childToken(cur) == "+" ? "add" : "sub";
                cur++;
            }

            visitMulExp(child(cur));
            int tmpRegNum = allocReg();
            tmpResult = "%" + to_string(tmpRegNum);
            printExp(tmpResult, op, tmpOp1, childOp(cur));
            if (op == "add") {
                tmpNum = tmpNum1 + childNum(cur);
            } else {
                tmpNum = tmpNum1 - childNum(cur);
            }

            curNode->setRegNum(tmpRegNum);
            curNode->setNum(tmpNum);
        }
    }

    //关系表达式 RelExp → AddExp | RelExp ('<' | '>' | '<=' | '>=') AddExp 存32位
    void visitRelExp(Node *curNode) {
        if (curNode->getSize() == 1) {
            visitAddExp(child(0));
            curNode->setRegNum(child(0)->getRegNum());
            curNode->setNum(child(0)->getNum());
        } else {
            int cur = 0;
            int tmpNum = 0;
//            string tmpOp_1;//比较对象1寄存器或数值
            string tmpOp32_1;
//            string tmpOp_2;//比较对象2寄存器或数值
            string tmpOp32_2;
            string tmpResult1;//比较结果
            string tmpResult32;//化为32位的比较结果
            string op;//符号
            //第一个RelExp
            visitRelExp(child(cur));
            tmpOp32_1 = childOp(cur);
            //化为32位
//            tmpOp32_1 = "%" + to_string(allocReg());
//            llvmFile_ << "    " << tmpOp32_1 << " = zext i1 " << tmpOp_1 << " to i32" << endl;

            cur++;
            //slt: 有符号小于
            if (childToken(cur) == "<") op = "slt";
                //sgt: 有符号大于
            else if (childToken(cur) == ">") op = "sgt";
                //sle: 有符号小于等于
            else if (childToken(cur) == "<=") op = "sle";
                //sge: 有符号大于等于
            else op = "sge";
            cur++;

            //假如还有RelExp
            while (childType(cur) == LexicalType::RelExp) {
                visitRelExp(child(cur));
                tmpOp32_2 = childOp(cur);
                //化为32位
//                tmpOp32_2 = "%" + to_string(allocReg());
//                llvmFile_ << "    " << tmpOp32_2 << " = zext i1 " << tmpOp_2 << " to i32" << endl;

                //比较
                tmpResult1 = "%" + to_string(allocReg());
                llvmFile_ << "    " << tmpResult1 << " = icmp " << op << " i32 " << tmpOp32_1 << ", " << tmpOp32_2
                          << endl;

                //化为32位
                tmpResult32 = "%" + to_string(allocReg());
                llvmFile_ << "    " << tmpResult32 << " = zext i1 " << tmpResult1 << " to i32" << endl;

                tmpOp32_1 = tmpResult32;
                cur++;
                //slt: 有符号小于
                if (childToken(cur) == "<") op = "slt";
                    //sgt: 有符号大于
                else if (childToken(cur) == ">") op = "sgt";
                    //sle: 有符号小于等于
                else if (childToken(cur) == "<=") op = "sle";
                    //sge: 有符号大于等于
                else op = "sge";
                cur++;
            }

            visitAddExp(child(cur));
            tmpOp32_2 = childOp(cur);


            tmpResult1 = "%" + to_string(allocReg());
            llvmFile_ << "    " << tmpResult1 << " = icmp " << op << " i32 " << tmpOp32_1 << ", " << tmpOp32_2 << endl;

            //化为32位
            int tmpRegNum = allocReg();
            tmpResult32 = "%" + to_string(tmpRegNum);
            llvmFile_ << "    " << tmpResult32 << " = zext i1 " << tmpResult1 << " to i32" << endl;

            curNode->setRegNum(tmpRegNum);
        }
    }

//            //化为1位
//            int tmpRegNum = allocReg();
//            string tmpResult1;
//            tmpResult1 = "%" + to_string(tmpRegNum);
//            llvmFile_ << "    " << tmpResult1 << " = trunc i32 " << child(0)->getOp() << " to i1" << endl;
//            curNode->setRegNum(tmpRegNum);
    //相等性表达式 EqExp → RelExp | EqExp ('==' | '!=') RelExp 存32位
    void visitEqExp(Node *curNode, string trueLabel, string falseLabel) {
        //如果现在是连并的一部分 并且不是if的开头
        if (isLabel(curNode) &&
            curNode != curNode->getParent()->getParent()->getParent()->getChild(0)->getChild(0)->getChild(0)) {
            llvmFile_ << curNode->getLabel() << ":" << endl;
        }
        if (curNode->getSize() == 1) {
            visitRelExp(child(0));
            curNode->setRegNum(child(0)->getRegNum());
            curNode->setNum(child(0)->getNum());

        } else {
            int cur = 0;
            int tmpNum = 0;
//            string tmpOp_1;//比较对象1寄存器或数值
            string tmpOp32_1;
//            string tmpOp_2;//比较对象2寄存器或数值
            string tmpOp32_2;
            string tmpResult1;//比较结果
            string tmpResult32;//化为32位的比较结果
            string op;//符号
            //第一个EqExp
            visitEqExp(child(cur), child(cur + 2)->getLabel(), falseLabel);
            tmpOp32_1 = childOp(cur);
            //化为32位
//            tmpOp32_1 = "%" + to_string(allocReg());
//            llvmFile_ << "    " << tmpOp32_1 << " = zext i1 " << tmpOp_1 << " to i32" << endl;

            cur++;
            op = childToken(cur) == "==" ? "eq" : "ne";
            cur++;

            //假如还有EqExp
            while (childType(cur) == LexicalType::EqExp) {
                visitEqExp(child(cur), child(cur + 2)->getLabel(), falseLabel);
                tmpOp32_2 = childOp(cur);
                //化为32位
//                tmpOp32_2 = "%" + to_string(allocReg());
//                llvmFile_ << "    " << tmpOp32_2 << " = zext i1 " << tmpOp_2 << " to i32" << endl;

                //比较
                tmpResult1 = "%" + to_string(allocReg());
                llvmFile_ << "    " << tmpResult1 << " = icmp " << op << " i32 " << tmpOp32_1 << ", " << tmpOp32_2
                          << endl;

                //化为32位
                tmpResult32 = "%" + to_string(allocReg());
                llvmFile_ << "    " << tmpResult32 << " = zext i1 " << tmpResult1 << " to i32" << endl;

                tmpOp32_1 = tmpResult32;
                cur++;
                op = childToken(cur) == "==" ? "eq" : "ne";
                cur++;
            }

            visitRelExp(child(cur));
            tmpOp32_2 = childOp(cur);
            //化为32位
//            tmpOp32_2 = "%" + to_string(allocReg());
//            llvmFile_ << "    " << tmpOp32_2 << " = zext i1 " << tmpOp_2 << " to i32" << endl;

            tmpResult1 = "%" + to_string(allocReg());
            llvmFile_ << "    " << tmpResult1 << " = icmp " << op << " i32 " << tmpOp32_1 << ", " << tmpOp32_2 << endl;

            //化为32位
            int tmpRegNum = allocReg();
            tmpResult32 = "%" + to_string(tmpRegNum);
            llvmFile_ << "    " << tmpResult32 << " = zext i1 " << tmpResult1 << " to i32" << endl;

            curNode->setRegNum(tmpRegNum);
        }

        //如果现在是连并的一部分
        if (isLabel(curNode)) {
            int tmpRegNum2 = allocReg();
            string tmpResult1;
            tmpResult1 = "%" + to_string(tmpRegNum2);
            llvmFile_ << "    " << tmpResult1 << " = icmp ne i32 0, " << curNode->getOp() << endl;
            llvmFile_ << "    br i1 " << tmpResult1 << ", label %" << trueLabel << ", label %" << falseLabel
                      << endl;
        }
    }

    //逻辑与表达式 LAndExp → EqExp | LAndExp '&&' EqExp
    void visitLAndExp(Node *curNode, string trueLabel, string falseLabel) {
        //如果现在是连并的一部分 并且不是if的开头
        if (isLabel(curNode) &&
            curNode != curNode->getParent()->getParent()->getParent()->getChild(0)->getChild(0)->getChild(0)) {
            llvmFile_ << curNode->getLabel() << ":" << endl;
        }
        if (curNode->getSize() == 1) {
            visitEqExp(child(0), trueLabel, falseLabel);
            curNode->setRegNum(child(0)->getRegNum());
        } else {
            int cur = 0;
            while (childType(cur) == LexicalType::LAndExp) {
                //成立就去看下一个，不成立就返回false
                visitLAndExp(child(cur), child(cur + 2)->getLabel(), falseLabel);
                cur += 2;
            }
            //最后一个 成立就返回true，不成立就返回else
            visitEqExp(child(cur), trueLabel, falseLabel);
        }
        //如果现在是连并的一部分
        if (isLabel(curNode)) {
            int tmpRegNum = allocReg();
            string tmpResult1;
            tmpResult1 = "%" + to_string(tmpRegNum);
            llvmFile_ << "    " << tmpResult1 << " = icmp ne i32 0, " << child(0)->getOp() << endl;
            llvmFile_ << "    br i1 " << tmpResult1 << ", label %" << trueLabel << ", label %" << falseLabel
                      << endl;
        }
    }

    //逻辑或表达式 LOrExp → LAndExp | LOrExp '||' LAndExp
    void visitLOrExp(Node *curNode, string trueLabel, string falseLabel) {
        if (curNode->getSize() == 1) {
            visitLAndExp(child(0), trueLabel, falseLabel);
        } else {
            int cur = 0;
            while (childType(cur) == LexicalType::LOrExp) {
                //成立就返回true，不成立就去看下一个
                visitLOrExp(child(cur), trueLabel, child(cur + 2)->getLabel());
                cur += 2;
            }
            //最后一个 成立就返回true，不成立就返回else
            visitLAndExp(child(cur), trueLabel, falseLabel);
        }
    }

    //常量表达式 ConstExp → AddExp
    void visitConstExp(Node *curNode) {
        visitAddExp(child(0));
        curNode->setNum(child(0)->getNum());
        curNode->setRegNum(child(0)->getRegNum());
    }


    bool isLabel(Node *curNode) {
        return curNode->getParent()->getType() == LexicalType::LAndExp
               && curNode->getParent()->getParent()->getType() == LexicalType::LOrExp;
    }
};

#endif //LLVMBUILDER_LLVMBUILDER_H
