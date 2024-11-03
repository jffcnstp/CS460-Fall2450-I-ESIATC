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

std::unordered_map<std::string, int> opPrecedence {
        {"(", 6},
        {")", 6},

        {"*", 5},
        {"/", 5},
        {"+", 4},
        {"-", 4},

        {">=", 3},
        {"<=", 3},
        {">", 3},
        {"<", 3},
        {"==", 3},

        {"&&", 2},
        {"||", 1},

        {"=", 0},
};


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

    void parseExpression() {
        std::stack<Node*> operatorStack;
        Node *current = CST->getCurrentNode();

        bool loop;
        bool expression = true;
        while (expression) {
            loop = false;
            if (current->data.getType() == "SEMICOLON") {
                expression = false;
            }
            if (current->data.getType() == "IDENTIFIER" || current->data.getType() == "INTEGER" ||
                current->data.getType() == "SINGLE_QUOTE" || current->data.getType() == "DOUBLE_QUOTE" ||
                current->data.getType() == "STRING") {
                cout << current->data.getName() << " ";
                AST->insertSibling(new Node(current->data));
                current = current->rightSibling;
            }
            else
            {
                if (current->data.getType() == "L_PAREN") {
                    operatorStack.push(current);
                    current = current->rightSibling;
                }
                else
                {
                    if (current->data.getType() == "R_PAREN") {
                        while (operatorStack.top()->data.getType() != "L_PAREN") {
                            if (operatorStack.empty()) {
                                cout << "There's been a terrible accident!";
                                exit(-1);
                            }
                            cout << operatorStack.top()->data.getName() << " ";
                            AST->insertSibling(new Node(operatorStack.top()->data));
                            operatorStack.pop();
                        }
                        operatorStack.pop();
                        current = current->rightSibling;
                    }
                    else
                    {
                        if (current->data.getType() == "PLUS" || current->data.getType() == "MINUS" ||
                            current->data.getType() == "ASTERISK" || current->data.getType() == "DIVIDE" ||
                            current->data.getType() == "ASSIGNMENT_OPERATOR") {
                            if (operatorStack.empty()) {
                                operatorStack.push(current);
                                current = current->rightSibling;
                            }
                            else
                            {
                                if (current->data.getType() == "PLUS" || current->data.getType() == "MINUS") {
                                    loop = true;
                                    while (loop) {
                                        if (!operatorStack.empty()) {
                                            if ((operatorStack.top()->data.getType() == "PLUS") || (operatorStack.top()->data.getType() == "MINUS")
                                            || (operatorStack.top()->data.getType() == "ASTERISK") || (operatorStack.top()->data.getType() == "DIVIDE")) {
                                                cout << operatorStack.top()->data.getName() << " ";
                                                AST->insertSibling(operatorStack.top());
                                                operatorStack.pop();
                                            }
                                            else {
                                                operatorStack.push(current);
                                                current = current->rightSibling;
                                                loop = false;
                                            }
                                        }
                                        else {
                                            operatorStack.push(current);
                                            current = current->rightSibling;
                                            loop = false;
                                        }
                                    }
                                }
                                else {
                                    if (current->data.getType() == "ASTERISK" || current->data.getType() == "DIVIDE") {
                                        loop = true;
                                        while (loop) {
                                            if (!operatorStack.empty()) {
                                                if ((operatorStack.top()->data.getType() == "ASTERISK") || (operatorStack.top()->data.getType() == "DIVIDE")) {
                                                    cout << operatorStack.top()->data.getName() << " ";
                                                    AST->insertSibling(operatorStack.top());
                                                    operatorStack.pop();
                                                }
                                                else {
                                                    operatorStack.push(current);
                                                    current = current->rightSibling;
                                                    loop = false;
                                                }
                                            }
                                            else {
                                                operatorStack.push(current);
                                                current = current->rightSibling;
                                                loop = false;
                                            }
                                        }
                                    }
                                    else {
                                        if (current->data.getType() == "ASSIGNMENT_OPERATOR") {
                                            loop = true;
                                            while (loop) {
                                                if (!operatorStack.empty()) {
                                                    if (current->data.getType() == "PLUS" || current->data.getType() == "MINUS" ||
                                                            current->data.getType() == "ASTERISK" || current->data.getType() == "DIVIDE") {
                                                        cout << operatorStack.top()->data.getName() << " ";
                                                        AST->insertSibling(operatorStack.top());
                                                        operatorStack.pop();
                                                    }
                                                    else {
                                                        operatorStack.push(current);
                                                        current = current->rightSibling;
                                                        loop = false;
                                                    }
                                                }
                                                else {
                                                    operatorStack.push(current);
                                                    current = current->rightSibling;
                                                    loop = false;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
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
