//
// Created by storm on 11/17/2024.
//

#ifndef INTERPRETER_INTERPRETER_H
#define INTERPRETER_INTERPRETER_H
#include "AbstractSyntaxTree.h"
#include "SymbolTable.h"
#include <stack>
#include <iostream>

using namespace std;

class Interpreter
{
    SymbolTable* table;
    LCRSTree *AST;
public:
    Interpreter(SymbolTable* createdtable,LCRSTree * createdAST)
    {
        table=createdtable;
        AST=createdAST;
    }
    /* LIST OF INTERPRETER FUNCTIONS
     * -------------------------------
     * startInterpreter() //starts Interpreting main function
     * findProcedure(string procedurename,vector<string> datavalues) //looks for a procedure in the Symboltable and makes a node Walk to its place in AST
     * traverseToChild(Node* TraversingNode) //Makes an independent Node walk to next Child
     * traverseNext(Node* TraversingNode) //Makes an independent Node move to the next Node
     * InterpretFunction(Node* Traversal,int functionscope) //DFA for each function traversal
     *
     *
     *
     */

    void startInterpreter()
    {
        findProcedure("main",vector<string>(0));
    }
    void findProcedure(string procedurename,vector<string> datavalues)
    {
        Node* TraversingNode=AST->root;
        Symbol* Symbolstorage;
        while(TraversingNode->data.getName()!=procedurename)
        {
            traverseToChild(TraversingNode);
        }
        if(TraversingNode->data.getName()==procedurename)
        {
            //Symbolstorage=table->searchSymbol(procedurename) Find the function in symboltable and retrieve it
            // InterpretFunction(TraversingNode,Symbolstorage->scope); Start interpreting the Function since it is at the correct node and we have the scope for its variables
        }
        else
        {
            cout<<"SOMETHING HAS GONE WRONG WE CAN'T FIND THE FUNCTION YOU'RE LOOKING FOR : "<<procedurename;
            exit(62);
        }

    }

    //Moves the Parameter Node* to the next Child
    void traverseToChild(Node* TraversingNode)
    {
        while(TraversingNode->rightSibling)
        {
            TraversingNode=TraversingNode->rightSibling;
        }
        TraversingNode=TraversingNode->leftChild;
    }
    // Moves the Parameter Node* to the next Node
    void traverseNext(Node* TraversingNode)
    {
        if(TraversingNode->rightSibling)
            TraversingNode=TraversingNode->rightSibling;
        else
            TraversingNode=TraversingNode->leftChild;
    }

    //These are individual DFAs that will appear PER function
    void InterpretFunction(Node* Traversal,int functionscope)
    {
        int scopeblock=0;
        traverseToChild(Traversal); //currently on function declaration.  Should traverse twice to go inside the block
        if(Traversal->data.getType() != "BEGIN BLOCK") {
            cout << "Function misalignment error instead it's: "<<Traversal->data.getType();
            exit(62);
        }
        traverseToChild(Traversal);
        //Begin DFA
        while(scopeblock !=-1)
        {
            if(Traversal->data.getType()=="DECLARATION")
            {
                traverseToChild(Traversal);
            }
            else if(Traversal->data.getType()=="ASSIGNMENT")
            {
                //evaluatePostfix(Traversal,functionscope)
            }
            else if(Traversal->data.getType()=="IF")
            {
                //evaluateIf(Traversal);
            }
            else if(Traversal->data.getType()=="WHILE")
            {
                //evaluateWhile(Traversal);
            }
            else if(Traversal->data.getType()=="FOR EXPRESSION 1")
            {
                //evaluateFor(Traversal);
            }
            else if(Traversal->data.getType()=="END BLOCK")
            {
                scopeblock-=1;
                traverseToChild(Traversal);
            }
        }
    }
};

#endif //INTERPRETER_INTERPRETER_H
