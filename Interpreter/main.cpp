#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "CommentRemove.h"
#include "Token.h"
#include "RecursiveDescentParser.h"
#include "SymbolTable.h"
#include "AbstractSyntaxTree.h"
#include "Interpreter.h"

using namespace std;


// Main function
int main() {
    vector<Token> tokenlist;
    string fileName ="programming_assignment_6-test_file_";
    string tokenizefile="test_file";
    Parser *CST;
    SymbolTable *Table;
    AbstractSyntaxTree *AST;
    Interpreter *finalmodule;

    for(int i=1; i < 4; i++) {
        cout<<endl;
        ignoreComments(fileName + std::to_string(i) + ".c", tokenizefile + std::to_string(i) + ".c");
        tokenlist = Tokenize(tokenizefile + std::to_string(i) + ".c");
        CST= new Parser(tokenlist);
        CST->buildCST();
        cout<<"CST built successfully"<<endl;
        //CST->tree->breadthFirstTraversal();
        CST->tree->resetCurrentNode();
        Table=new SymbolTable(CST->getCST());
        Table->BuildTable();
        AST=new AbstractSyntaxTree(CST->getCST());
        AST->buildAST();
        cout<<"AST built Successfully"<<endl<<endl;
       // AST->getAST()->breadthFirstTraversal();

        finalmodule=new Interpreter(Table,AST->getAST());
        finalmodule->startInterpreter();
        delete finalmodule;
        delete AST;
        delete Table;
        delete CST;
    }
    return 0;
}
