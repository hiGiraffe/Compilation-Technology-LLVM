//
// Created by 11863 on 2023/9/20.
//
#include "lib.h"

using namespace std;

int main() {
    ifstream infile("testfile.txt");
    if (!infile.is_open()) {
        cout << "Can't open the input file" << endl;
    }
    ofstream outfile;
    outfile.open("output.txt");

    ofstream errfile;
    errfile.open("error.txt");

    Source source(infile);
    map<int, string> errorLog;
    Lexer lexer(source, errorLog);

    Parser parser(lexer, errorLog);

    SemanticAnalyzer semanticAnalyzer(parser, errorLog);

//    outfile << lexer;
    outfile << parser;
    errfile << semanticAnalyzer;


    infile.close();
    outfile.close();
    errfile.close();
    return 0;
}