//
// Created by storm on 10/31/2024.
//

#ifndef INTERPRETER_ABSTRACTSYNTAXTREE_H
#define INTERPRETER_ABSTRACTSYNTAXTREE_H
#include "string"
#include "stack"


class AbstractSyntaxTree{
    LCRSTree *AST;
    LCRSTree *CST;
public:

    AbstractSyntaxTree(LCRSTree *builtCST){CST=builtCST;AST=new LCRSTree();}

    void buildAST()
    {
        CST->resetCurrentNode();
        parseExpression();
    };

    int opPrecedence(const std::string &opType) {
        if (opType == "ASSIGNMENT_OPERATOR") return 1;
        if (opType == "BOOLEAN_OR") return 2;
        if (opType == "BOOLEAN_AND") return 3;
        if (opType == "BOOLEAN_EQUAL" || opType == "BOOLEAN_NOT_EQUAL" ||
            opType == "LT" || opType == "GT" ||
            opType == "LT_EQUAL" || opType == "GT_EQUAL") return 4;
        if (opType == "PLUS" || opType == "MINUS") return 5;
        if (opType == "ASTERISK" || opType == "DIVIDE" || opType == "MODULO") return 6;
        if (opType == "BOOLEAN_NOT") return 7;
        return 0;
    }

    //after this runs, the current node should be the child of the last token
    void parseExpression() {
        std::stack<Node*> operatorStack;
        Node *current = CST->getCurrentNode();

        bool expression = true;
        while (expression) {
            if (current->rightSibling == nullptr) { //end of current expression
                expression = false;
                current = current->leftChild;
                continue;
            }
            if (current->data.getType() == "IDENTIFIER" || current->data.getType() == "INTEGER" ||
                current->data.getType() == "SINGLE_QUOTE" || current->data.getType() == "DOUBLE_QUOTE" ||
                current->data.getType() == "STRING") {
                //cout << current->data.getName() << " ";
                AST->insertSibling(new Node(current->data));
                current = current->rightSibling;
            }
            else if (current->data.getType() == "L_PAREN") {
                operatorStack.push(current);
                current = current->rightSibling;
            }
            else if (current->data.getType() == "R_PAREN") { //dump operator stack until L_PAREN
                while (!operatorStack.empty() && operatorStack.top()->data.getType() != "L_PAREN") {
                    //cout << operatorStack.top()->data.getName() << " ";
                    AST->insertSibling(new Node(operatorStack.top()->data));
                    operatorStack.pop();
                }
                if (!operatorStack.empty()) {
                    operatorStack.pop(); //pop L_PAREN
                }
                current = current->rightSibling;
            }
            else {
                int currentPrecedence = opPrecedence(current->data.getType());
                while (!operatorStack.empty() && opPrecedence(operatorStack.top()->data.getType()) >= currentPrecedence) {
                    //cout << operatorStack.top()->data.getName() << " ";
                    AST->insertSibling(new Node(operatorStack.top()->data));
                    operatorStack.pop();
                }
                operatorStack.push(current);
                current = current->rightSibling;
            }
        }
        while (!operatorStack.empty()) {
            //cout << operatorStack.top()->data.getName() << " ";
            AST->insertSibling(operatorStack.top());
            operatorStack.pop();
        }
    }

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

        if (current->data.getType() == "if") {
            // Create and insert the "If" node into the AST
            Node* ifNode = new Node(Token("IF", "if", 0)); 
            AST->insertChild(ifNode);  // Add If node to the AST

            // Move to the condition expression in the CST
            CST->nextNode();
            Node* conditionNode = CST->getCurrentNode();
            if (conditionNode) {
                parseExpression();  // Parse the condition, adding to AST under the If node
            }

            // Move to the body of the If statement
            CST->nextNode();
            Node* ifBodyNode = CST->getCurrentNode();
            if (ifBodyNode) {
                parseStatementList();  // Parse the body, attaching statements to the If node
            }
        } 
        else if (current->data.getType() == "while") {
            // Create and insert the "While" node into the AST
            Node* whileNode = new Node(Token("WHILE", "while", 0)); 
            AST->insertChild(whileNode);  // Add While node to the AST

            // Move to the condition expression in the CST
            CST->nextNode();
            Node* conditionNode = CST->getCurrentNode();
            if (conditionNode) {
                parseExpression();  // Parse the condition, adding to AST under the While node
            }

            // Move to the body of the While loop
            CST->nextNode();
            Node* whileBodyNode = CST->getCurrentNode();
            if (whileBodyNode) {
                parseStatementList();  // Parse the body, attaching statements to the While node
            }
        }
    }


};




#endif //INTERPRETER_ABSTRACTSYNTAXTREE_H
