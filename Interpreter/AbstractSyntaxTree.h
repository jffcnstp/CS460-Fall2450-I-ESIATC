//
// Created by storm on 10/31/2024.
//

#ifndef INTERPRETER_ABSTRACTSYNTAXTREE_H
#define INTERPRETER_ABSTRACTSYNTAXTREE_H
#include "map"
#include "unordered_map"
#include "string"
#include "stack"
#include "queue"


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
        if (opType == "ASTERISK" || opType == "DIVIDE") return 6;
        return 0;
    }

    void parseExpression() {
        std::stack<Node*> operatorStack;
        Node *current = CST->getCurrentNode();

        bool loop;
        bool expression = true;
        while (expression) {
            loop = false;
            if (current->rightSibling == nullptr) {
                expression = false;
            }
            if (current->data.getType() == "IDENTIFIER" || current->data.getType() == "INTEGER" ||
                current->data.getType() == "SINGLE_QUOTE" || current->data.getType() == "DOUBLE_QUOTE" ||
                current->data.getType() == "STRING") {
                cout << current->data.getName() << " ";
                AST->insertSibling(new Node(current->data));
                current = current->rightSibling;
            }
            else if (current->data.getType() == "L_PAREN") {
                operatorStack.push(current);
                current = current->rightSibling;
            }
            else if (current->data.getType() == "R_PAREN") {
                while (!operatorStack.empty() && operatorStack.top()->data.getType() == "L_PAREN") {
                    cout << operatorStack.top()->data.getName() << " ";
                    AST->insertSibling(new Node(operatorStack.top()->data));
                    operatorStack.pop();
                }
                if (!operatorStack.empty()) {
                    operatorStack.pop();
                }
                current = current->rightSibling;
            }
            else {
                int currentPrecedence = opPrecedence(current->data.getType());
                while (!operatorStack.empty() && opPrecedence(operatorStack.top()->data.getType()) >= currentPrecedence) {
                    cout << operatorStack.top()->data.getName() << " ";
                    AST->insertSibling(new Node(operatorStack.top()->data));
                    operatorStack.pop();
                }
                operatorStack.push(current);
                current = current->rightSibling;
            }
        }
        while (!operatorStack.empty()) {
            cout << operatorStack.top()->data.getName() << " ";
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
};




#endif //INTERPRETER_ABSTRACTSYNTAXTREE_H
