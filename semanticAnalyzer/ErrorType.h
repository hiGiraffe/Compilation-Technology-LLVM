//
// Created by 11863 on 2023/10/24.
//

#ifndef SEMANTICANALYZER_ERRORTYPE_H
#define SEMANTICANALYZER_ERRORTYPE_H
using namespace std;

#include "string"


enum class ErrorType{
    //非法符号 a
    IllegalSymbol,
    //名字重定义 b
    nameRedefinition,
    //未定义的名字 c
    nameUndefined,
    //函数参数个数不匹配 d
    NumOfFunctionParametersNotMatch,
    //函数参数类型不匹配 e
    TypeOfFunctionParametersNotMatch,
    //无返回值的函数存在不匹配的return语句 f
    ReturnRedundant,
    //有返回值的函数缺少return语句 g
    ReturnMissing,
    //不能改变常量的值 h
    ConstantModification,
    //缺少分号 i
    MissingSEMICN,
    //缺少右小括号’)’ j
    MissingRPARENT,
    //缺少右中括号’]’ k
    MissingRBRACK,
    //printf中格式字符与表达式个数不匹配 l
    FormatStrNumNotMatch,
    //在非循环块中使用break和continue语句 m
    RedundantBreakAndContinue,
};

static const string errorStrings[] = {
        "a",
        "b",
        "c",
        "d",
        "e",
        "f",
        "g",
        "h",
        "i",
        "j",
        "k",
        "l",
        "m",
};

const string errorTypeToPrint(ErrorType errorType) {
    return errorStrings[(int) errorType];
}

#endif //SEMANTICANALYZER_ERRORTYPE_H
