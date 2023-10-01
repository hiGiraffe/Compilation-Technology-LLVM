//
// Created by 11863 on 2023/9/22.
//

#ifndef COMPILER_LEXER_LEXICALTYPE_H
#define COMPILER_LEXER_LEXICALTYPE_H
using namespace std;

#include "string"

enum class LexicalType {
//  Ident
    IDENFR,
//  IntConst
    INTCON,
//  FormatString
    STRCON,

//  main
    MAINTK,
//  const
    CONSTTK,
//  int
    INTTK,
//  break
    BREAKTK,
//  continue
    CONTINUETK,
//  if
    IFTK,
//  else

    ELSETK,
//    !
    NOT,
//    &&
    AND,
//    ||
    OR,

//    for
    FORTK,
//    getint
    GETINTTK,
//    printf
    PRINTFTK,
//    return
    RETURNTK,
//    void
    VOIDTK,

//    +
    PLUS,
//    -
    MINU,
//    *
    MULT,
//    /
    DIV,
//    %
    MOD,
//    <
    LSS,
//    <=
    LEQ,
//    >
    GRE,
//    >=
    GEQ,
//    ==
    EQL,
//    !=
    NEQ,
//    =
    ASSIGN,
//    ;
    SEMICN,
//    ,
    COMMA,
//    (
    LPARENT,
//    )
    RPARENT,
//    [
    LBRACK,
//    ]
    RBRACK,
//    {
    LBRACE,
//    }
    RBRACE,

    CompUnit,
    ConstDecl,
    ConstDef,
    ConstInitVal,
    VarDecl,
    VarDef,
    InitVal,
    FuncDef,
    MainFuncDef,
    FuncType,
    FuncFParams,
    FuncFParam,
    Block,
    Stmt,
    ForStmt,
    Exp,
    Cond,
    LVal,
    PrimaryExp,
    Number,
    UnaryExp,
    UnaryOp,
    FuncRParams,
    MulExp,
    AddExp,
    RelExp,
    EqExp,
    LAndExp,
    LOrExp,
    ConstExp,
};

static const string lexicalStrings[] = {
        "IDENFR",
        "INTCON",
        "STRCON",
        "MAINTK",
        "CONSTTK",
        "INTTK",
        "BREAKTK",
        "CONTINUETK",
        "IFTK",
        "ELSETK",
        "NOT",
        "AND",
        "OR",
        "FORTK",
        "GETINTTK",
        "PRINTFTK",
        "RETURNTK",
        "VOIDTK",
        "PLUS",
        "MINU",
        "MULT",
        "DIV",
        "MOD",
        "LSS",
        "LEQ",
        "GRE",
        "GEQ",
        "EQL",
        "NEQ",
        "ASSIGN",
        "SEMICN",
        "COMMA",
        "LPARENT",
        "RPARENT",
        "LBRACK",
        "RBRACK",
        "LBRACE",
        "RBRACE",

        "CompUnit",
        "ConstDecl",
        "ConstDef",
        "ConstInitVal",
        "VarDecl",
        "VarDef",
        "InitVal",
        "FuncDef",
        "MainFuncDef",
        "FuncType",
        "FuncFParams",
        "FuncFParam",
        "Block",
        "Stmt",
        "ForStmt",
        "Exp",
        "Cond",
        "LVal",
        "PrimaryExp",
        "Number",
        "UnaryExp",
        "UnaryOp",
        "FuncRParams",
        "MulExp",
        "AddExp",
        "RelExp",
        "EqExp",
        "LAndExp",
        "LOrExp",
        "ConstExp",
};

const string lexicalTypeToPrint(LexicalType lexicalType) {
    return lexicalStrings[(int) lexicalType];
}

#endif //COMPILER_LEXER_LEXICALTYPE_H
