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

    void parseIfAndWhile() {
        CST->resetCurrentNode();
        Node* current = CST->getCurrentNode();
        if (!current) return;

        if (current->data.type == "If") {
            // Create the AST node for "If"
            Node* ifNode = new Node(Token("If"));
            AST->insertChild(ifNode);  // Add If node to the AST
            AST->resetCurrentNode();   // Move currentNode to the newly added If node

            // Move to the condition expression
            CST->nextNode();
            Node* conditionNode = CST->getCurrentNode();
            if (conditionNode) {
                Node* conditionAST = parseExpression(conditionNode);
                AST->insertChild(conditionAST);  // Attach condition as child of If node
            }

            // Move to the body of the If statement
            CST->nextNode();
            Node* ifBodyNode = CST->getCurrentNode();
            if (ifBodyNode) {
                Node* ifBodyAST = parseStatementList(ifBodyNode);
                AST->insertSibling(ifBodyAST);  // Attach body as sibling of the condition node
            }
        } 
        else if (current->data.type == "While") {
            // Create the AST node for "While"
            Node* whileNode = new Node(Token("While"));
            AST->insertChild(whileNode);  // Add While node to the AST
            AST->resetCurrentNode();      // Move currentNode to the newly added While node

            // Move to the condition expression
            CST->nextNode();
            Node* conditionNode = CST->getCurrentNode();
            if (conditionNode) {
                Node* conditionAST = parseExpression(conditionNode);
                AST->insertChild(conditionAST);  // Attach condition as child of While node
            }

            // Move to the body of the While loop
            CST->nextNode();
            Node* whileBodyNode = CST->getCurrentNode();
            if (whileBodyNode) {
                Node* whileBodyAST = parseStatementList(whileBodyNode);
                AST->insertSibling(whileBodyAST);  // Attach body as sibling of the condition node
            }
        }
    }





};




#endif //INTERPRETER_ABSTRACTSYNTAXTREE_H
