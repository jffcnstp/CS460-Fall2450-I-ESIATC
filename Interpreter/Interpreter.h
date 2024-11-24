//
// Created by storm on 11/17/2024.
//

#ifndef INTERPRETER_INTERPRETER_H
#define INTERPRETER_INTERPRETER_H
#include "AbstractSyntaxTree.h"
#include "SymbolTable.h"
#include <stack>
class Interpreter
{
    std::stack<Node*> programCounter;
    SymbolTable* table;
    LCRSTree *AST;
public:
    Interpreter(SymbolTable* createdtable,LCRSTree * createdAST)
    {
        table=createdtable;
        AST=createdAST;
    }

    void Interpret()
    {
        findProcedure("main");
    }
    void findProcedure(string procedurename)
    {
        Node* TraversingNode=AST->root;
        Symbol* Symbolstorage;
        while(TraversingNode->data.getName()!=procedurename)
        {
            traverseToChild(TraversingNode);
        }
        if(TraversingNode->data.getName()==procedurename)
        {
            //Symbolstorage=table->searchSymbol(procedurename)
        }

    }
    void traverseToChild(Node* TraversingNode)
    {
        while(TraversingNode->rightSibling)
        {
            TraversingNode=TraversingNode->rightSibling;
        }
        TraversingNode=TraversingNode->leftChild;
    }
    void traverseNext(Node* TraversingNode)
    {
        if(TraversingNode->rightSibling)
            TraversingNode=TraversingNode->rightSibling;
        else
            TraversingNode=TraversingNode->leftChild;
    }

};

#endif //INTERPRETER_INTERPRETER_H
