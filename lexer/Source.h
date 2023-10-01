//
// Created by 11863 on 2023/9/22.
//

#ifndef LEXER_SOURCE_H
#define LEXER_SOURCE_H

#include <string>
#include <fstream>
#include "iostream"
#include "vector"

using namespace std;

class Source{
    vector<string> sourceFile={};
public:
    explicit Source(ifstream &infile){
        if(!infile.is_open()){
            cout<<"Can't open the input file"<<endl;
        }
        else{
            string line;
            while(getline(infile,line)){
                sourceFile.push_back(line);
            }
        }
    }
    vector<string> getSourceFile(){
        return this->sourceFile;
    }
};
#endif //LEXER_SOURCE_H
