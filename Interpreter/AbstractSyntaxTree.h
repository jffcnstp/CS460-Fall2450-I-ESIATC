//
// Created by storm on 10/31/2024.
//

#ifndef INTERPRETER_ABSTRACTSYNTAXTREE_H
#define INTERPRETER_ABSTRACTSYNTAXTREE_H
#include "RecursiveDescentParser.h"


class AbstractSyntaxTree{
    LCRSTree *AST;
    LCRSTree *CST;
public:

    AbstractSyntaxTree(LCRSTree *builtCST){CST=builtCST;AST=new LCRSTree();}

    void buildAST()
    {
        CST->resetCurrentNode();

    };

    //ASSUME YOUR PARSE FUNCTION ALREADY IDENTIFIED ITSELF.  AKA THE FIRST NODE OF THE SIBLING CHAIN IS IDENTIFIED
    //parseIfandWhile
    //parseFor
    //parseExpression
    //
    //
};




#endif //INTERPRETER_ABSTRACTSYNTAXTREE_H
