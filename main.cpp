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

    Source source(infile);

    Lexer lexer(source);

    Parser parser(lexer);

    outfile << parser;


    infile.close();
    outfile.close();
    return 0;
}