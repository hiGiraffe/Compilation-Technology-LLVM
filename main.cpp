//
// Created by 11863 on 2023/9/20.
//
using namespace std;

#include "lib.h"
#include "iostream"
#include "fstream"
#include "string"

using namespace std;

int main() {
    ifstream inFile("testfile.txt");
    if (!inFile.is_open()) {
        cout << "Can't open the input file" << endl;
    }
    ofstream outFile;
    outFile.open("output.txt");

    ofstream errFile;
    errFile.open("error.txt");

    ofstream llvmFile;
    llvmFile.open("llvm_ir.txt");

    Source source(inFile);
    map<int, string> errorLog;
    Lexer lexer(source, errorLog);

    Parser parser(lexer, errorLog);

    SemanticAnalyzer semanticAnalyzer(parser, errorLog);
    if (errorLog.size() == 0) {
        LLVMBuilder llvmBuilder(semanticAnalyzer, llvmFile);
    }
//    outfile << lexer;
    outFile << parser;
    errFile << semanticAnalyzer;

    inFile.close();
    outFile.close();
    errFile.close();
    llvmFile.close();
    return 0;
}