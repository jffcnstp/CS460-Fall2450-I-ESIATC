//
// Created by storm on 10/31/2024.
//

#ifndef INTERPRETER_ABSTRACTSYNTAXTREE_H
#define INTERPRETER_ABSTRACTSYNTAXTREE_H
#include "RecursiveDescentParser.h"
#include "string"
#include "stack"


class AbstractSyntaxTree {
    LCRSTree *AST;
    LCRSTree *CST;
public:
    //constructor
    AbstractSyntaxTree(LCRSTree *builtCST) {
        CST = builtCST;
        AST = new LCRSTree();
    }
    /* List of Functions inside AbstractSyntaxTree
     * -----------------------------------------------
     * LCRSTree *getAST()       //returns the *AST member
     * void buildAST()          //DFA function to build the Abstract Syntax Tree stored in *AST
     * int opPrecedence(const std::string &opType) //function that returns an integer marking the order of precedence for operators
     * void parseExpression()    //subfunction of buildAST() handles both mathematical and logical expressions
     * void parseFunctionParam() //subfunction of buildAST() handles the parameters of Function calls
     * void parseForStatement(int forExpression = 1)    //subfunction of buildAST() handles the For Statment and puts them in the AST as 3 separate For Expressions
     * void parseIfAndWhile()   //subfunction of buildAST() handles both If and While statments as they have the same syntax
     *
     */

    //returns the AST
    LCRSTree *getAST() { return AST; }
    void buildAST() {
        CST->resetCurrentNode();
        Node *currentCSTNode;
        string Nodetype;
        while (!CST->EOT()) {
            currentCSTNode = CST->getCurrentNode();
            Nodetype = currentCSTNode->data.getType();
            if (Nodetype == "FUNCTION" || Nodetype == "PROCEDURE" || (Nodetype == "IDENTIFIER" && find(begin(typekeyword), end(typekeyword),currentCSTNode->data.getName()) !=end(typekeyword)))
            {
                Node* dataextractor;
                if(Nodetype =="FUNCTION")
                    dataextractor=currentCSTNode->rightSibling->rightSibling;
                else if(Nodetype == "PROCEDURE")
                    dataextractor=currentCSTNode->rightSibling;
                else
                    dataextractor=currentCSTNode->rightSibling;
                AST->insertChild(new Node(Token("DECLARATION", dataextractor->data.getName(), currentCSTNode->data.getLine())));
                CST->nextChild();
            }
            else if (Nodetype == "IDENTIFIER")
            {
                if (currentCSTNode->rightSibling->data.getType() == "ASSIGNMENT_OPERATOR" || currentCSTNode->rightSibling->data.getType()=="L_BRACKET") {
                    AST->insertChild(new Node(Token("ASSIGNMENT", "ASSIGNMENT", currentCSTNode->data.getLine())));
                    parseExpression();
                } else if (currentCSTNode->rightSibling->data.getType() == "L_PAREN") {
                    AST->insertChild(new Node(currentCSTNode->data));
                    CST->nextNode();//should be on the L_PAREN of the function
                    parseFunctionParam();
                }
                else if(currentCSTNode->data.getName()=="return")
                {
                    AST->insertChild(new Node(Token("RETURN", "RETURN", currentCSTNode->data.getLine())));
                    CST->nextNode();
                    parseExpression();
                }

                CST->nextChild();
            } else if (Nodetype == "IF" || Nodetype == "WHILE") {
                parseIfAndWhile();

            }
            else if(Nodetype=="ELSE")
            {
                AST->insertChild(new Node(Token("ELSE","ELSE",currentCSTNode->data.getLine())));
                CST->nextChild();
            }
            else if (Nodetype =="FOR")
            {
                parseForStatement();
            }
            else if (Nodetype == "L_BRACE") {
                AST->insertChild(new Node(Token("BEGIN BLOCK", "BEGIN BLOCK", currentCSTNode->data.getLine())));
                CST->nextChild();
            } else if (Nodetype == "R_BRACE") {
                AST->insertChild(new Node(Token("END BLOCK", "END BLOCK", currentCSTNode->data.getLine())));
                CST->nextChild();
            }
        }
        if(CST->getCurrentNode()->data.getType()=="R_BRACE")
        {
            AST->insertChild(new Node(Token("END BLOCK", "END BLOCK", currentCSTNode->data.getLine())));
        }
    }

    int opPrecedence(const std::string &opType) {
        if (opType == "ASSIGNMENT_OPERATOR") return 1;
        if (opType == "BOOLEAN_OR") return 2;
        if (opType == "BOOLEAN_AND") return 3;
        if (opType == "BOOLEAN_EQUAL" || opType == "BOOLEAN_NOT_EQUAL" ||
            opType == "LT" || opType == "GT" ||
            opType == "LT_EQUAL" || opType == "GT_EQUAL")
            return 4;
        if (opType == "PLUS" || opType == "MINUS") return 5;
        if (opType == "ASTERISK" || opType == "DIVIDE" || opType == "MODULO") return 6;
        if (opType == "BOOLEAN_NOT") return 7;
        return 0;
    }

    //THIS SHOULD PARSE THE EXPRESSION IN A LOOP OF OPERAND->OPERATOR->etc.  IF THERE IS NO OPERATOR THE LOOP EXITS
    void parseExpression() {
        std::stack<Node *> operatorStack;

        bool expression = true;

        while (expression) {
            expression = false;
            //OPERAND
            if (CST->getCurrentNode()->data.getType() == "MINUS" || CST->getCurrentNode()->data.getType() == "INTEGER") //negative integer or normal integer
            {
                if (CST->getCurrentNode()->data.getType() == "MINUS") {
                    CST->nextNode();
                    AST->insertSibling(new Node(
                            Token(CST->getCurrentNode()->data.getType(), "-" + CST->getCurrentNode()->data.getName(),
                                  CST->getCurrentNode()->data.getLine())));
                } else {
                    AST->insertSibling(new Node(CST->getCurrentNode()->data));
                }
                CST->nextNode();
            }
            else if (CST->getCurrentNode()->data.getType() == "SINGLE_QUOTE" || CST->getCurrentNode()->data.getType() == "DOUBLE_QUOTE") { //operand is a string
                 //insert quote
                CST->nextNode();

                AST->insertSibling(new Node(CST->getCurrentNode()->data)); // insert string
                CST->nextNode();

                 //insert end quote
                CST->nextNode(); //

            }
            else if (CST->getCurrentNode()->data.getType() == "BOOLEAN_NOT" || CST->getCurrentNode()->data.getType() =="IDENTIFIER" // boolean not + identifier, identifier operand, true/false keyword
                    || CST->getCurrentNode()->data.getType() == "TRUE" || CST->getCurrentNode()->data.getType() == "FALSE")
            {
                if (CST->getCurrentNode()->data.getType() == "BOOLEAN_NOT") {
                    operatorStack.push(CST->getCurrentNode());
                    CST->nextNode();
                }

                AST->insertSibling(new Node(CST->getCurrentNode()->data));
                CST->nextNode();


                if (CST->getCurrentNode()->data.getType() == "L_BRACKET") {
                    AST->insertSibling(new Node(CST->getCurrentNode()->data));
                    CST->nextNode();
                    parseExpression();// should end on R_BRACKET

                    AST->insertSibling(new Node(CST->getCurrentNode()->data)); //insert R_BRACKET
                    CST->nextNode(); //should move to an end of expression token like ) ] , or ;
                }
                else if (CST->getCurrentNode()->data.getType() =="L_PAREN") //inside the identifier if statement means this parentheses is a function call
                {
                    parseFunctionParam();
                }

            }
            else if (CST->getCurrentNode()->data.getType() =="L_PAREN") //assumes this is operator precedence parentheses
            {
                CST->nextNode(); //skip L_PAREN insertion
                parseExpression(); // assumes this ends on R_PAREN
                CST->nextNode(); // skip R_PAREN insertion
            }
            //OPERATOR
            if (find(operatorlist.begin(), operatorlist.end(), CST->getCurrentNode()->data.getType()) !=
                operatorlist.end()) // checks if the operator is in the operator list
            {
                expression = true;
                int currentPrecedence = opPrecedence(CST->getCurrentNode()->data.getType());
                while (!operatorStack.empty() && opPrecedence(operatorStack.top()->data.getType()) >= currentPrecedence) {
                    AST->insertSibling(new Node(operatorStack.top()->data));
                    operatorStack.pop();
                }
                operatorStack.push(CST->getCurrentNode());
                CST->nextNode();
            }
        }
        while (!operatorStack.empty()) { //empty stack
            //cout << operatorStack.top()->data.getName() << " ";
            AST->insertSibling(new Node(operatorStack.top()->data));
            operatorStack.pop();
        }
        //By the end of parseExpression the CST should be at a ) ] , or ;
    }

    //Assumes the CST is on the L_PAREN of the function call parameters
    void parseFunctionParam() {
        bool multipleparams = true;
        AST->insertSibling(new Node(CST->getCurrentNode()->data));
        CST->nextNode();
        while (multipleparams) {
            multipleparams = false;
            parseExpression();
            if (CST->getCurrentNode()->data.getType() == "COMMA") {
                multipleparams = true;
                CST->nextNode();

            }
        }
        AST->insertSibling(new Node(CST->getCurrentNode()->data));//should insert R_PAREN
        CST->nextNode();
    }


    //ASSUME YOUR PARSE FUNCTION ALREADY IDENTIFIED ITSELF.  AKA THE FIRST NODE OF THE SIBLING CHAIN IS IDENTIFIED
    // PA5: parseForStatement();
    // Called on top of a for statement
    // convert this part of the for expression into post-fix maybe using external function
    // example: for (i=0;i<something;i++); == for expression 1 i 0 = for expression 2 i something < ...
    void parseForStatement(int forExpression = 1) {

        //modify given curr node to say for expression 'x'
        Node *currNode = CST->getCurrentNode();
        currNode->data.type = "FOR EXPRESSION " + to_string(forExpression);

        // This will be called on the for statement,
        // inserting for statement into AST
        AST->insertChild(new Node(Token("FOR EXPRESSION 1","FOR EXPRESSION 1",CST->getCurrentNode()->data.getLine())));
        CST->nextNode();
        // Continue to the right of CST

        // For AST we ignore the L_PAREN token.
        CST->nextNode(); // go past the L_PAREN
        parseExpression(); // Should be on top of an expression and exit on a semicolon

        AST->insertChild(new Node(Token("FOR EXPRESSION 2","FOR EXPRESSION 2",CST->getCurrentNode()->data.getLine())));
        CST->nextNode(); //go past 1st semicolon
        parseExpression(); //second expression exits parseExpression on semicolon

        AST->insertChild(new Node(Token("FOR EXPRESSION 3","FOR EXPRESSION 3",CST->getCurrentNode()->data.getLine())));
        CST->nextNode(); //go past 2nd semicolon
        parseExpression(); //last Expression should exit on R_PAREN

        CST->nextNode();
    }

    void parseIfAndWhile()
    {
        // Determine if it is an 'If' or 'While' statement
        if (CST->getCurrentNode()->data.getType() == "IF")
        {AST->insertChild(new Node(CST->getCurrentNode()->data));}
        if (CST->getCurrentNode()->data.getType() == "WHILE")
        {AST->insertChild(new Node(CST->getCurrentNode()->data));}

            CST->nextNode(); //moves to L_PAREN
            CST->nextNode(); //moves to start of expression
            parseExpression(); //should've moved to R_PAREN
            CST->nextNode(); //moves to L_BRACE


    }

    //PA6: evaluateBoolExpression()
    //called within an if, while, or for expression
    bool evaluateBoolExpression(SymbolTable* SymbolTable, int currentScope) {
        Node* currentNode = AST->getCurrentNode();
        std::stack<Node*> evaluateStack;
        while (currentNode != nullptr) {
            if (currentNode->data.getType() == "IDENTIFIER" ||
                currentNode->data.getType() == "INTEGER") {
                evaluateStack.push(currentNode);
                currentNode = currentNode->rightSibling;
            }
            else if (currentNode->data.getType() == "SINGLE_QUOTE") {
                currentNode = currentNode->rightSibling;
                evaluateStack.push(currentNode);
                currentNode = currentNode->rightSibling;
            }
            else if (find(operatorlist.begin(), operatorlist.end(), currentNode->data.getType()) !=
                     operatorlist.end()) {
                opHelperFunction(currentNode, evaluateStack, SymbolTable, currentScope);
                currentNode = currentNode->rightSibling;
            }
        }
        if (evaluateStack.size() != 1) {
            std::cerr << "evaluateBoolExpression: evaluateStack should contain a single node, but doesn't" << std::endl;
            exit(-1);
        }
        else if (evaluateStack.top()->data.getName() != "1" ||
                evaluateStack.top()->data.getName() != "0") {
            std::cerr << "evaluateBoolExpression: final result is not a boolean value" << std::endl;
            exit(-1);
        }
        else {
            return evaluateStack.top();
        }
    }

    //PA6: evaluateExpression()
    //called when reaching a postfix expression in the AST
    //assumptions:
    //  expression is a numerical expression
    //  the current AST node is the first operand
    //helper functions needed:
    //  getArrayValue
    //  evaluateFunction
    Value evaluateExpression(SymbolTable* SymbolTable, int currentScope) {
        Node* currentNode = AST->getCurrentNode();
        std::stack<Node*> evaluateStack;
        while (currentNode != nullptr) {
            //operands: identifiers. variables, functions, arrays
            if (currentNode->data.getType() == "IDENTIFIER") {
                if (currentNode->rightSibling->data.getType() == "L_PAREN") {
                    //evaluateStack.push(evaluateFunction);
                }
                else if (currentNode->rightSibling->data.getType() == "L_BRACE") {
                    //evaluateStack.push(getArrayValue);
                }
                else { //if not a function or array, push on to stack
                    evaluateStack.push(currentNode);
                }
            }
            //operands: surrounded by single quotes
            else if (currentNode->data.getType() == "SINGLE_QUOTE") {
                currentNode = currentNode->rightSibling;
                evaluateStack.push(currentNode);
                currentNode = currentNode->rightSibling;
            }
                //operands: integers
            else if (currentNode->data.getType() == "INTEGER") {
                evaluateStack.push(currentNode);
                currentNode = currentNode->rightSibling;
            }
                //operators
            else if (find(operatorlist.begin(), operatorlist.end(), currentNode->data.getType()) !=
                     operatorlist.end()) {
                opHelperFunction(currentNode, evaluateStack, SymbolTable, currentScope);
                currentNode = currentNode->rightSibling;
            }
            //assignment operator: end of expression
            else if (currentNode->data.getType() == "ASSIGNMENT_OPERATOR") {
                Symbol* op1Symbol;
                Symbol* op2Symbol;
                Value operand2, operand1;
                if (evaluateStack.top()->data.getType() == "IDENTIFIER" &&
                    SymbolTable->existsInTable(currentScope, evaluateStack.top()->data.getName())) {
                    op2Symbol = SymbolTable->searchSymbol(currentScope, evaluateStack.top()->data.getName());
                    operand2 = op2Symbol->value;
                    evaluateStack.pop();
                }
                else if (evaluateStack.top()->data.getType() == "INTEGER") {
                    operand2 = std::stoi(evaluateStack.top()->data.getName());
                } else {
                    std::cerr << "Error: operand2 is missing or something idk" << std::endl;
                    exit(-1);
                }
                if (evaluateStack.top()->data.getType() == "IDENTIFIER" &&
                    SymbolTable->existsInTable(currentScope, evaluateStack.top()->data.getName())) {
                    op1Symbol = SymbolTable->searchSymbol(currentScope, evaluateStack.top()->data.getName());
                    operand1 = op1Symbol->value;
                    evaluateStack.pop();
                }
                else {
                    std::cerr << "Error: attempting to assign value to non-variable" << std::endl;
                    exit(-1);
                }
                if (std::get_if<int>(&operand2) && std::get_if<int>(&operand1)) {
                    operand1 = operand2;
                    SymbolTable->setValue(currentScope, op1Symbol->name, operand1);
                    return operand1;
                } else {
                    std::cerr << "Assignment error: one or both operands are not valid types" << std::endl;
                    exit(-1);
                }
            }
        }
        std::cerr << "evaluateExpression: Reached end of expression without returning a final result" << std::endl;
        exit(-1);
    }

    void opHelperFunction(Node* currentNode, std::stack<Node*> &evaluateStack,
                          SymbolTable *SymbolTable, int currentScope) {
        string currentOperator = currentNode->data.getType();
        if (currentOperator == "PLUS") {
            evaluatePlus(evaluateStack, currentScope, SymbolTable);
        }
        else if (currentOperator == "MINUS") {
            evaluateMinus(evaluateStack, currentScope, SymbolTable);
        }
        else if (currentOperator == "ASTERISK") {
            evaluateMultiply(evaluateStack, currentScope, SymbolTable);
        }
        else if (currentOperator == "DIVIDE") {
            evaluateDivision(evaluateStack, currentScope, SymbolTable);
        }
        else if (currentOperator == "MODULO") {
            evaluateModulo(evaluateStack, currentScope, SymbolTable);
        }
        else if (currentOperator == "LT_EQUAL") {
            evaluateLessThanOrEqual(evaluateStack, currentScope, SymbolTable);
        }
        else if (currentOperator == "LT") {
            evaluateLessThan(evaluateStack, currentScope, SymbolTable);
        }
        else if (currentOperator == "GT_EQUAL") {
            evaluateGreaterThanOrEqual(evaluateStack, currentScope, SymbolTable);
        }
        else if (currentOperator == "GT") {
            evaluateGreaterThan(evaluateStack, currentScope, SymbolTable);
        }
        else if (currentOperator == "BOOLEAN_AND") {
            evaluateLogicalAnd(evaluateStack, currentScope, SymbolTable);
        }
        else if (currentOperator == "BOOLEAN_OR") {
            evaluateLogicalOr(evaluateStack, currentScope, SymbolTable);
        }
        else if (currentOperator == "BOOLEAN_NOT_EQUAL") {
            evaluateLogicalNot(evaluateStack, currentScope, SymbolTable);
        }
        else {
            std::cerr << "opHelperFunction: not an operator" << std::endl;
            exit(-1);
        }
    }



    // Helper function for the helper functions (resolves operand value in case of variables)
    int resolveOperandValue(std::stack<Node*>& operands, int currentScope, SymbolTable *symbolTable) {
        if (operands.empty()) throw std::runtime_error("Operand stack is empty");
        Node* top = operands.top();
        operands.pop();
        
        if (top->data.getType() == "INTEGER") {
            return std::stoi(top->data.getName());
        }

        // Otherwise, assume it's a variable
        Symbol* symbol = symbolTable->searchSymbol(currentScope, top->data.getName());
        if (!symbol || symbol->datatype != "int" || symbol->isArray) {
            throw std::runtime_error("Invalid variable: " + top->data.getName());
        }

        // Assuming variable value is stored in name as a string
        return std::stoi(symbol->name);
    }

    void evaluatePlus(std::stack<Node*>& operands, int currentScope, SymbolTable *symbolTable) {
        if (operands.size() < 2) throw std::runtime_error("Insufficient operands for addition");
        int b = resolveOperandValue(operands, currentScope, symbolTable);
        int a = resolveOperandValue(operands, currentScope, symbolTable);
        Node* result;
        result->data.name = std::to_string(a + b);
        result->data.type = "INTEGER";
        operands.push(result);
    }
    void evaluateMinus(std::stack<Node*>& operands, int currentScope, SymbolTable *symbolTable) {
        if (operands.size() < 2) throw std::runtime_error("Insufficient operands for subtraction");
        int b = resolveOperandValue(operands, currentScope, symbolTable);
        int a = resolveOperandValue(operands, currentScope, symbolTable);
        Node* result;
        result->data.name = std::to_string(a - b);
        result->data.type = "INTEGER";
        operands.push(result);
    }
    void evaluateMultiply(std::stack<Node*>& operands, int currentScope, SymbolTable *symbolTable) {
        if (operands.size() < 2) throw std::runtime_error("Insufficient operands for multiplication");
        int b = resolveOperandValue(operands, currentScope, symbolTable);
        int a = resolveOperandValue(operands, currentScope, symbolTable);
        Node* result;
        result->data.name = std::to_string(a * b);
        result->data.type = "INTEGER";
        operands.push(result);
    }
    void evaluateDivision(std::stack<Node*>& operands, int currentScope, SymbolTable *symbolTable) {
        if (operands.size() < 2) throw std::runtime_error("Insufficient operands for division");
        int b = resolveOperandValue(operands, currentScope, symbolTable);
        if (b == 0) throw std::runtime_error("Division by zero");
        int a = resolveOperandValue(operands, currentScope, symbolTable);
        Node* result;
        result->data.name = std::to_string(a / b);
        result->data.type = "INTEGER";
        operands.push(result);
    }
    void evaluateModulo(std::stack<Node*>& operands, int currentScope, SymbolTable *symbolTable) {
        if (operands.size() < 2) throw std::runtime_error("Insufficient operands for modulo");
        int b = resolveOperandValue(operands, currentScope, symbolTable);
        if (b == 0) throw std::runtime_error("Modulo by zero");
        int a = resolveOperandValue(operands, currentScope, symbolTable);
        Node* result;
        result->data.name = std::to_string(a % b);
        result->data.type = "INTEGER";
        operands.push(result);
    }
    void evaluateLessThan(std::stack<Node*>& operands, int currentScope, SymbolTable *symbolTable) {
        if (operands.size() < 2) throw std::runtime_error("Insufficient operands for comparison");
        int b = resolveOperandValue(operands, currentScope, symbolTable);
        int a = resolveOperandValue(operands, currentScope, symbolTable);
        Node* result;
        result->data.name = (a < b) ? "1" : "0";
        result->data.type = "INTEGER";
        operands.push(result);
    }
    void evaluateGreaterThan(std::stack<Node*>& operands, int currentScope, SymbolTable *symbolTable) {
        if (operands.size() < 2) throw std::runtime_error("Insufficient operands for comparison");
        int b = resolveOperandValue(operands, currentScope, symbolTable);
        int a = resolveOperandValue(operands, currentScope, symbolTable);
        Node* result;
        result->data.name = (a > b) ? "1" : "0";
        result->data.type = "INTEGER";
        operands.push(result);
    }
    void evaluateLessThanOrEqual(std::stack<Node*>& operands, int currentScope, SymbolTable *symbolTable) {
        if (operands.size() < 2) throw std::runtime_error("Insufficient operands for comparison");
        int b = resolveOperandValue(operands, currentScope, symbolTable);
        int a = resolveOperandValue(operands, currentScope, symbolTable);
        Node* result;
        result->data.name = (a <= b) ? "1" : "0";
        result->data.type = "INTEGER";
        operands.push(result);
    }
    void evaluateGreaterThanOrEqual(std::stack<Node*>& operands, int currentScope, SymbolTable *symbolTable) {
        if (operands.size() < 2) throw std::runtime_error("Insufficient operands for comparison");
        int b = resolveOperandValue(operands, currentScope, symbolTable);
        int a = resolveOperandValue(operands, currentScope, symbolTable);
        Node* result;
        result->data.name = (a >= b) ? "1" : "0";
        result->data.type = "INTEGER";
        operands.push(result);
    }
    void evaluateLogicalAnd(std::stack<Node*>& operands, int currentScope, SymbolTable *symbolTable) {
        if (operands.size() < 2) throw std::runtime_error("Insufficient operands for logical AND");
        int b = resolveOperandValue(operands, currentScope, symbolTable);
        int a = resolveOperandValue(operands, currentScope, symbolTable);
        Node* result;
        result->data.name = (a && b) ? "1" : "0";
        result->data.type = "INTEGER";
        operands.push(result);
    }
    void evaluateLogicalOr(std::stack<Node*>& operands, int currentScope, SymbolTable *symbolTable) {
        if (operands.size() < 2) throw std::runtime_error("Insufficient operands for logical OR");
        int b = resolveOperandValue(operands, currentScope, symbolTable);
        int a = resolveOperandValue(operands, currentScope, symbolTable);
        Node* result;
        result->data.name = (a || b) ? "1" : "0";
        result->data.type = "INTEGER";
        operands.push(result);
    }
    void evaluateLogicalNot(std::stack<Node*>& operands, int currentScope, SymbolTable *symbolTable) {
        if (operands.empty()) throw std::runtime_error("Insufficient operands for logical NOT");
        int a = resolveOperandValue(operands, currentScope, symbolTable);
        Node* result;
        result->data.name = !a ? "1" : "0";
        result->data.type = "INTEGER";
        operands.push(result);
    }



};

#endif //INTERPRETER_ABSTRACTSYNTAXTREE_H
