//
// Created by storm on 11/17/2024.
//

#ifndef INTERPRETER_INTERPRETER_H
#define INTERPRETER_INTERPRETER_H
#include "AbstractSyntaxTree.h"
#include "SymbolTable.h"
#include "Token.h"
#include <stack>
#include <iostream>

using namespace std;

class Interpreter
{
    stack<Node*> progcounter;
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
        AST->resetCurrentNode();
        findProcedure("main"); //traverses AST until it finds main
        InterpretFunction(table->searchSymbol("main")->scope);
    }
    void findProcedure(string procedurename)
    {
        AST->resetCurrentNode();
        Symbol* Symbolstorage;
        while(AST->getCurrentNode()->data.getName()!=procedurename)
        {
           AST->nextChild();
        }
        if(AST->getCurrentNode()->data.getName()==procedurename)
        {
            return;
        }
        else
        {
            cout<<"SOMETHING HAS GONE WRONG WE CAN'T FIND THE FUNCTION YOU'RE LOOKING FOR : "<<procedurename;
            exit(62);
        }

    }
    

    //These are individual DFAs that will appear PER function
    void InterpretFunction(int functionscope)
    {
        bool withinscope=true;
        bool inloop=false;
        int localscope=0;

        AST->nextChild(); //currently on function declaration.  Should traverse twice to go inside the block
        if(AST->getCurrentNode()->data.getType() != "BEGIN BLOCK") {
            cout << "Function misalignment error instead it's: "<<AST->getCurrentNode()->data.getType();
            exit(62);
        }
        AST->nextChild();
        //Begin DFA
        while(withinscope)
        {
            if(AST->getCurrentNode()->data.getType()=="DECLARATION")
            {
                AST->nextChild();
            }
            else if(AST->getCurrentNode()->data.getType()=="ASSIGNMENT")
            {
                //evaluateExpression()
            }
            else if(AST->getCurrentNode()->data.getType()=="IF")
            {
                /*if(!evaluateIf(functionscope))
                 * AST->nextChild(); //go past Beginblock
                 * AST->nextChild();
                 *  int withinscope=1
                 *  while(withinscope !=0)
                 *  {
                 *      if(AST->getCurrentNode()->data.getTYpe()=="BEGIN BLOCK"
                 *          withinscope+=1;
                 *      if(AST->getCurrentNode()->data.getType()=="END BLOCK"
                 *          withinscope-=1;
                 *      AST->nextchild();
                   }
                 AST->nextChild()
                 if(AST->getCurrentNode()->data.getType()=="ELSE")
                    AST->nextChild();
                    */
            }
            else if(AST->getCurrentNode()->data.getType()=="ELSE") // we hit an else but we did the IF
            {
                traverseConditionalBlock();
            }
            else if(AST->getCurrentNode()->data.getType()=="WHILE")
            {
                progcounter.push(AST->getCurrentNode());
                if(evaluateWhileStatement(functionscope))
                {
                    inloop=true;
                    localscope=0;
                }
                else
                {
                    inloop=false;
                    traverseConditionalBlock();
                    progcounter.pop();
                }
            }
            else if(AST->getCurrentNode()->data.getType()=="FOR EXPRESSION 1")
            {
                progcounter.push(AST->getCurrentNode());
                if(evaluateForStatement(functionscope)) //evaluate whether we've gone through the first loop
                {
                    inloop=true;
                    localscope=0;
                }
                else
                {
                    inloop=false;
                    traverseConditionalBlock();  //is this necessary for FOR?
                    progcounter.pop();
                }
            }
            else if(AST->getCurrentNode()->data.getType()=="BEGIN BLOCK")
            {
                if(inloop==true)
                    localscope+=1;
            }
            else if(AST->getCurrentNode()->data.getType()=="END BLOCK") {

                if (inloop == true) {
                    localscope -= 1;
                    if(localscope==0)
                    {
                        AST->setCurrentNode(progcounter.top());
                    }
                }
            }
            else if(AST->getCurrentNode()->data.getType()=="RETURN")
            {
               // evaluateExpression()
            }
        }
    }


    // evaluateIfStatement();
    // This function will evaluate an if expression using a DFA, NOTE: THIS ALREADY ASSUMES IT'S CALLED ON AN IF
    // parameters:
    // Traversal, the node that is being traversed
    // functionscope, an integer that helps keep track of its scope
    bool evaluateIfStatement(Node* Traversal, int functionScope) {
        // Move to the condition node
        Node* conditionNode = Traversal->leftChild;

        // Grabbing sibling (body block of the IF statement)
        Node* bodyBlock = conditionNode->rightSibling;

        // Evaluate the condition
        evaluateExpression(table, functionScope, conditionNode);

        // Check the condition result
        bool conditionResult = conditionNode->data.getName() == "true";

        if (conditionResult) {
            // If the condition is true, interpret the body block
            InterpretFunction(functionScope); // Assume InterpretFunction automatically interprets the subtree
            traverseNext(Traversal); // Move past the IF block
            return true; // IF block was executed
        } else {
            // If the condition is false, check for an ELSE block
            if (bodyBlock->rightSibling && bodyBlock->rightSibling->data.getType() == "ELSE") {
                return evaluateElseStatement(bodyBlock->rightSibling, functionScope); // Evaluate the ELSE block
            }
            traverseNext(Traversal); // Skip the IF block and any non-existent ELSE block
            return false; // Neither IF nor ELSE blocks were executed
        }
    }

    // evaluateElseStatement();
    // This function will evaluate an else expression using a DFA, NOTE: THIS ALREADY ASSUMES IT'S CALLED ON AN ELSE
    // parameters:
    // Traversal, the node that is being traversed
    // functionscope, an integer that helps keep track of its scope
    bool evaluateElseStatement(Node* Traversal, int functionScope) {
        // Grabbing the body block (child of ELSE)
        Node* bodyBlock = Traversal->leftChild;

        // Check if the ELSE block has a body to execute
        if (bodyBlock != nullptr) {
            // Interpret the ELSE block
            InterpretFunction(functionScope); // Assume InterpretFunction interprets the subtree
            traverseNext(Traversal); // Move past the ELSE block
            return true; // ELSE block was executed
        }

        traverseNext(Traversal); // Move past the ELSE block
        return false; // ELSE block was skipped (no body present)
    }


    bool evaluateWhileStatement(int functionScope) {
        bool loopExecuted = false;

        // Move to the condition node of the WHILE
        AST->nextChild(); 
        Node* conditionNode = AST->getCurrentNode(); // Save the current node as the condition

        // Move to the body block (sibling of the condition)
        AST->nextNode();
        Node* bodyBlock = AST->getCurrentNode();

        while (true) {
            // Evaluate the condition
            evaluateExpression(table, functionScope, conditionNode);

            // Check the condition result (stored in the token's name)
            bool conditionResult = conditionNode->data.getName() == "true";

            // If the condition is false, break the loop
            if (!conditionResult) {
                break;
            }

            // Set loopExecuted to true since the loop ran
            loopExecuted = true;

            // Interpret the body block
            InterpretFunction(functionScope);
            
            // Reset to the condition node for reevaluation
            AST->setCurrentNode(conditionNode);
        }

        // After exiting the loop, ensure the AST points to the correct node (end of WHILE)
        AST->setCurrentNode(bodyBlock->rightSibling);

        return loopExecuted;
    }


    bool evaluateForStatement(int functionScope) {
        // Step 1: Move to the initialization (Expression 1)
        Node* initNode = AST->getCurrentNode()->leftChild;

        // Evaluate initialization (e.g., variable assignment or declaration)
        evaluateExpression(table, functionScope, initNode);

        // Step 2: Move to the condition (Expression 2)
        Node* conditionNode = initNode->rightSibling;

        // Evaluate the condition
        evaluateExpression(table, functionScope, conditionNode);
        bool conditionResult = conditionNode->data.getName() == "true";

        // Step 3: Move to the increment (Expression 3)
        Node* incrementNode = conditionNode->rightSibling;

        // Step 4: Move to the body block
        Node* bodyBlock = incrementNode->rightSibling;

        // Step 5: Loop execution
        while (conditionResult) {
            // Execute the body block
            InterpretFunction(functionScope);

            // Evaluate the increment (e.g., updating a loop variable)
            evaluateExpression(table, functionScope, incrementNode);

            // Re-evaluate the condition
            evaluateExpression(table, functionScope, conditionNode);
            conditionResult = conditionNode->data.getName() == "true";
        }

        // If the loop condition is false initially, skip the loop body
        if (!conditionResult) {
            // Traverse past the FOR block to continue interpretation
            traverseConditionalBlock();
            return false; // FOR loop was skipped
        }

        // Loop executed at least once
        return true;
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
                opHelperFunction(currentNode, evaluateStack, table, currentScope);
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
                    string funcName = currentNode->data.getName();
                    //vector<string?> data = evaluatefunction() -> should return the parameter data in a vector.  The Node position should either be on R_PAREN or next operand
                    vector<std::string> data = evaluateFunction();

                    progcounter.push(AST->getCurrentNode());
                    findProcedure(funcName);

                    Symbol* localSymbol = table->searchSymbol(funcName);
//                    localSymbol->name = data[0]; i think searchSymbol does all this already?
//                    localSymbol->type = data[1];
//                    localSymbol->datatype = data[2];

                    interpretFunction(funcName);
                    AST->setCurrentNode(progcounter.top());
                    progcounter.pop();

                    //evaluateStack.push(searchSymbol("functionname")->data); //when the DFA finishes it should have pushed a value to its data field
                }
                else if (currentNode->rightSibling->data.getType() == "L_BRACE") {
                    //evaluateStack.push(getArrayValue);
                }
                else { //if not a function or array, push on to stack
                    evaluateStack.push(currentNode);
                }
            }
                //operands: surrounded by quotes
            else if (currentNode->data.getType() == "SINGLE_QUOTE" ||
                     currentNode->data.getType() == "DOUBLE_QUOTE") {
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
                opHelperFunction(currentNode, evaluateStack, table, currentScope);
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
            std::cerr << "opHelperFunction: unsupported operator" << std::endl;
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
        Node* result = new Node(Token("INTEGER", std::to_string(a + b), 0));
        operands.push(result);
    }
    void evaluateMinus(std::stack<Node*>& operands, int currentScope, SymbolTable *symbolTable) {
        if (operands.size() < 2) throw std::runtime_error("Insufficient operands for subtraction");
        int b = resolveOperandValue(operands, currentScope, symbolTable);
        int a = resolveOperandValue(operands, currentScope, symbolTable);
        Node* result = new Node(Token("INTEGER", std::to_string(a - b), 0));
        operands.push(result);
    }
    void evaluateMultiply(std::stack<Node*>& operands, int currentScope, SymbolTable *symbolTable) {
        if (operands.size() < 2) throw std::runtime_error("Insufficient operands for multiplication");
        int b = resolveOperandValue(operands, currentScope, symbolTable);
        int a = resolveOperandValue(operands, currentScope, symbolTable);
        Node* result = new Node(Token("INTEGER", std::to_string(a * b), 0));
        operands.push(result);
    }
    void evaluateDivision(std::stack<Node*>& operands, int currentScope, SymbolTable *symbolTable) {
        if (operands.size() < 2) throw std::runtime_error("Insufficient operands for division");
        int b = resolveOperandValue(operands, currentScope, symbolTable);
        if (b == 0) throw std::runtime_error("Division by zero");
        int a = resolveOperandValue(operands, currentScope, symbolTable);
        Node* result = new Node(Token("INTEGER", std::to_string(a / b), 0));
        operands.push(result);
    }
    void evaluateModulo(std::stack<Node*>& operands, int currentScope, SymbolTable *symbolTable) {
        if (operands.size() < 2) throw std::runtime_error("Insufficient operands for modulo");
        int b = resolveOperandValue(operands, currentScope, symbolTable);
        if (b == 0) throw std::runtime_error("Modulo by zero");
        int a = resolveOperandValue(operands, currentScope, symbolTable);
        Node* result = new Node(Token("INTEGER", std::to_string(a % b), 0));
        operands.push(result);
    }
    void evaluateLessThan(std::stack<Node*>& operands, int currentScope, SymbolTable *symbolTable) {
        if (operands.size() < 2) throw std::runtime_error("Insufficient operands for comparison");
        int b = resolveOperandValue(operands, currentScope, symbolTable);
        int a = resolveOperandValue(operands, currentScope, symbolTable);
        Node* result = new Node(Token("INTEGER", (a < b) ? "1" : "0", 0));
        operands.push(result);
    }
    void evaluateGreaterThan(std::stack<Node*>& operands, int currentScope, SymbolTable *symbolTable) {
        if (operands.size() < 2) throw std::runtime_error("Insufficient operands for comparison");
        int b = resolveOperandValue(operands, currentScope, symbolTable);
        int a = resolveOperandValue(operands, currentScope, symbolTable);
        Node* result = new Node(Token("INTEGER", (a > b) ? "1" : "0", 0));
        operands.push(result);
    }
    void evaluateLessThanOrEqual(std::stack<Node*>& operands, int currentScope, SymbolTable *symbolTable) {
        if (operands.size() < 2) throw std::runtime_error("Insufficient operands for comparison");
        int b = resolveOperandValue(operands, currentScope, symbolTable);
        int a = resolveOperandValue(operands, currentScope, symbolTable);
        Node* result = new Node(Token("INTEGER", (a <= b) ? "1" : "0", 0));
        operands.push(result);
    }
    void evaluateGreaterThanOrEqual(std::stack<Node*>& operands, int currentScope, SymbolTable *symbolTable) {
        if (operands.size() < 2) throw std::runtime_error("Insufficient operands for comparison");
        int b = resolveOperandValue(operands, currentScope, symbolTable);
        int a = resolveOperandValue(operands, currentScope, symbolTable);
        Node* result = new Node(Token("INTEGER", (a >= b) ? "1" : "0", 0));
        operands.push(result);
    }
    void evaluateLogicalAnd(std::stack<Node*>& operands, int currentScope, SymbolTable *symbolTable) {
        if (operands.size() < 2) throw std::runtime_error("Insufficient operands for logical AND");
        int b = resolveOperandValue(operands, currentScope, symbolTable);
        int a = resolveOperandValue(operands, currentScope, symbolTable);
        Node* result = new Node(Token("INTEGER", (a && b) ? "1" : "0", 0));
        operands.push(result);
    }
    void evaluateLogicalOr(std::stack<Node*>& operands, int currentScope, SymbolTable *symbolTable) {
        if (operands.size() < 2) throw std::runtime_error("Insufficient operands for logical OR");
        int b = resolveOperandValue(operands, currentScope, symbolTable);
        int a = resolveOperandValue(operands, currentScope, symbolTable);
        Node* result = new Node(Token("INTEGER", (a || b) ? "1" : "0", 0));
        operands.push(result);
    }
    void evaluateLogicalNot(std::stack<Node*>& operands, int currentScope, SymbolTable *symbolTable) {
        if (operands.empty()) throw std::runtime_error("Insufficient operands for logical NOT");
        int a = resolveOperandValue(operands, currentScope, symbolTable);
        Node* result = new Node(Token("INTEGER", !a ? "1" : "0", 0));
        operands.push(result);
    }

    void traverseConditionalBlock()
    {
        int localscope=1;
        AST->nextChild();
        AST->nextChild();//move past BEGIN BLOCK
        while(localscope != 0)
        {
            if(AST->getCurrentNode()->data.getType()=="BEGIN BLOCK")
                localscope+=1;
            if(AST->getCurrentNode()->data.getType()=="END BLOCK")
                localscope-=1;
            AST->nextChild();
        }
        AST->nextChild();//should move from END BLOCK to next child
    }

};

#endif //INTERPRETER_INTERPRETER_H
