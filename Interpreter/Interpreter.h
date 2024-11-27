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
                if(evaluateWhile(functionscope))
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
                if(evaluateFor(functionscope,inloop)) //evaluate whether we've gone through the first loop
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

    void evaluateIfStatement(Node* Traversal, int functionscope) {

        // Move to the condition node
        Node* conditionNode = Traversal->leftChild;

        // Grabbing sibling
        Node* bodyBlock = conditionNode->rightSibling;

        // Evaluates the expression past IF
        evaluateExpression(table,functionscope,conditionNode);

        bool conditionResult = conditionNode->data.getName() == "true";

        // if false, do else statement and stop evaluating if.
        if (!conditionResult) {
            evaluateElseStatement(Traversal,functionscope);
            traverseNext(Traversal);
            return;
        }

        // interpret the bodyBlock
        InterpretFunction(bodyBlock, functionscope);

        // Go next
        traverseNext(Traversal);
    }

    // evaluateElseStatement();
    // This function will evaluate an else expression using a DFA, NOTE: THIS ALREADY ASSUMES IT'S CALLED ON AN ELSE
    // parameters:
    // Traversal, the node that is being traversed
    // functionscope, an integer that helps keep track of its scope

    void evaluateElseStatement(Node* Traversal, int functionscope) {

        // Move to the condition node
        Node* conditionNode = Traversal->leftChild;

        // Grabbing sibling
        Node* bodyBlock = conditionNode->rightSibling;

        // Evaluates the expression past ELSE
        evaluateExpression(table,functionscope,conditionNode);

        bool conditionResult = conditionNode->data.getName() == "false";

        // if true, do else statement and stop evaluating if.
        if (!conditionResult) {
            evaluateIfStatement(Traversal,functionscope);
            traverseNext(Traversal);
            return;
        }

        // interpret the bodyBlock
        InterpretFunction(bodyBlock, functionscope);

        // Go next
        traverseNext(Traversal);

    }


    void evaluateWhile(Node* Traversal, int functionScope) {
        // Move to the condition node (child of the WHILE node)
        Node* conditionNode = Traversal->leftChild;

        // Get the body block node (sibling of the condition node)
        Node* bodyBlock = conditionNode->rightSibling;

        // While the condition evaluates to true
        while (true) {
            // Evaluate the condition
            evaluateExpression(table, functionScope, conditionNode);

            // Assume condition result is stored as a string in the Token data
            bool conditionResult = conditionNode->data.getName() == "true";

            // Break if the condition is false
            if (!conditionResult) {
                break;
            }

            // Interpret the body block
            InterpretFunction(bodyBlock, functionScope);
        }

        // Move Traversal to the next sibling node after the "WHILE" node
        traverseNext(Traversal);
    }






    //PA6: evaluateExpression()
    //called when reaching a postfix expression in the AST
    //assumptions:
    //  expression is part of an assignment or a boolean operation
    //  the current AST node is the first operand
    //helper functions needed:
    //  operator shenanigans
    //  getArrayValue
    //  evaluateFunction
    void evaluateExpression(SymbolTable* SymbolTable, int currentScope, Node* currentNode) {
        bool expression = true;
        //stack data type is tentative. must retrieve int value or ref to symbol table from string
        std::stack<string> evaluateStack;
        while (currentNode != nullptr) {
            //operands: identifiers. variables, functions, arrays
            if (currentNode->data.getType() == "IDENTIFIER") {
                if (currentNode->rightSibling->data.getType() == "L_PAREN") {
                    //string funcname=AST->getCurrentNode()->data.getName(); save the name of the function somewhere.
                    //vector<string?> data = evaluatefunction() -> should return the parameter data in a vector.  The Node position should either be on R_PAREN or next operand

                    //progcounter.push(AST->getCurrentNode());  push the position onto the progcounter stack
                    //findprocedure(funcname) moves the AST to where the function is in the AST

                    //Symbol* localsymbol = searchSymbol(funcname) Calling the symbol to a local place because we need to set the parameter values before actually entering the function DFA
                    //Set the symbols parameter data with the local vector we have

                    //Interpretfunction(funcname);  //DFA goes through the function
                    //AST->setCurrentNode(progcounter.top()) //set the CurrentNode back to the middle of the expression
                    //progcounter.pop()  // pop the stack since we don't need that node position anymore

                    //evaluateStack.push(searchSymbol("functionname")->data); //when the DFA finishes it should have pushed a value to its data field
                }
                else if (currentNode->rightSibling->data.getType() == "L_BRACE") {
                    //evaluateStack.push(getArrayValue);
                }
                else { //if not a function or array, push on to stack
                    evaluateStack.push(currentNode->data.getName());
                }
            }
                //operands: integers
            else if (currentNode->data.getType() == "INTEGER") {
                evaluateStack.push(currentNode->data.getName());
                currentNode = currentNode->rightSibling;
            }
                //operators
            else if (find(operatorlist.begin(), operatorlist.end(), currentNode->data.getType()) !=
                     operatorlist.end()) {
                opHelperFunction(currentNode, evaluateStack);
                currentNode = currentNode->rightSibling;
            }
            else if (currentNode->data.getType() == "ASSIGNMENT_OPERATOR") {
                if (SymbolTable->existsInTable(evaluateStack.top())) {

                }
            }
        }
    }

    void opHelperFunction(Node* currentNode, std::stack<string> &evaluateStack) {
        string currentOperator = currentNode->data.getType();
        if (currentOperator == "PLUS") {
            evaluatePlus(evaluateStack);
        }
        else if (currentOperator == "MINUS") {
            evaluateMinus(evaluateStack);
        }
        else if (currentOperator == "MODULO") {
            evaluateModulo(evaluateStack);
        }
        else if (currentOperator == "GT") {
            evaluateGreaterThan(evaluateStack);
        }
        else if (currentOperator == "BOOLEAN_OR") {
            evaluateLogicalOr(evaluateStack);
        }
    }



    // Helper function for the helper functions (resolves operand value in case of variables)
    int resolveOperandValue(std::stack<std::string>& operands, int currentScope, SymbolTable& symbolTable) {
        if (operands.empty()) throw std::runtime_error("Operand stack is empty");
        std::string top = operands.top();
        operands.pop();

        // Check if the top is a numeric literal
        if (std::isdigit(top[0]) || (top[0] == '-' && top.size() > 1 && std::isdigit(top[1]))) {
            return std::stoi(top);
        }

        // Otherwise, assume it's a variable
        Symbol* symbol = symbolTable.searchSymbol(currentScope, top);
        if (!symbol || symbol->datatype != "int" || symbol->isArray) {
            throw std::runtime_error("Invalid variable: " + top);
        }

        // Assuming variable value is stored in name as a string
        return std::stoi(symbol->name);
    }

    void evaluatePlus(std::stack<std::string>& operands, int currentScope, SymbolTable& symbolTable) {
        if (operands.size() < 2) throw std::runtime_error("Insufficient operands for addition");
        int b = resolveOperandValue(operands, currentScope, symbolTable);
        int a = resolveOperandValue(operands, currentScope, symbolTable);
        operands.push(std::to_string(a + b));
    }
    void evaluateMinus(std::stack<std::string>& operands, int currentScope, SymbolTable& symbolTable) {
        if (operands.size() < 2) throw std::runtime_error("Insufficient operands for subtraction");
        int b = resolveOperandValue(operands, currentScope, symbolTable);
        int a = resolveOperandValue(operands, currentScope, symbolTable);
        operands.push(std::to_string(a - b));
    }
    void evaluateMultiply(std::stack<std::string>& operands, int currentScope, SymbolTable& symbolTable) {
        if (operands.size() < 2) throw std::runtime_error("Insufficient operands for multiplication");
        int b = resolveOperandValue(operands, currentScope, symbolTable);
        int a = resolveOperandValue(operands, currentScope, symbolTable);
        operands.push(std::to_string(a * b));
    }
    void evaluateDivision(std::stack<std::string>& operands, int currentScope, SymbolTable& symbolTable) {
        if (operands.size() < 2) throw std::runtime_error("Insufficient operands for division");
        int b = resolveOperandValue(operands, currentScope, symbolTable);
        if (b == 0) throw std::runtime_error("Division by zero");
        int a = resolveOperandValue(operands, currentScope, symbolTable);
        operands.push(std::to_string(a / b));
    }
    void evaluateModulo(std::stack<std::string>& operands, int currentScope, SymbolTable& symbolTable) {
        if (operands.size() < 2) throw std::runtime_error("Insufficient operands for modulo");
        int b = resolveOperandValue(operands, currentScope, symbolTable);
        if (b == 0) throw std::runtime_error("Modulo by zero");
        int a = resolveOperandValue(operands, currentScope, symbolTable);
        operands.push(std::to_string(a % b));
    }
    void evaluateLessThan(std::stack<std::string>& operands, int currentScope, SymbolTable& symbolTable) {
        if (operands.size() < 2) throw std::runtime_error("Insufficient operands for comparison");
        int b = resolveOperandValue(operands, currentScope, symbolTable);
        int a = resolveOperandValue(operands, currentScope, symbolTable);
        operands.push(a < b ? "1" : "0");
    }
    void evaluateGreaterThan(std::stack<std::string>& operands, int currentScope, SymbolTable& symbolTable) {
        if (operands.size() < 2) throw std::runtime_error("Insufficient operands for comparison");
        int b = resolveOperandValue(operands, currentScope, symbolTable);
        int a = resolveOperandValue(operands, currentScope, symbolTable);
        operands.push(a > b ? "1" : "0");
    }
    void evaluateLessThanOrEqual(std::stack<std::string>& operands, int currentScope, SymbolTable& symbolTable) {
        if (operands.size() < 2) throw std::runtime_error("Insufficient operands for comparison");
        int b = resolveOperandValue(operands, currentScope, symbolTable);
        int a = resolveOperandValue(operands, currentScope, symbolTable);
        operands.push(a <= b ? "1" : "0");
    }
    void evaluateGreaterThanOrEqual(std::stack<std::string>& operands, int currentScope, SymbolTable& symbolTable) {
        if (operands.size() < 2) throw std::runtime_error("Insufficient operands for comparison");
        int b = resolveOperandValue(operands, currentScope, symbolTable);
        int a = resolveOperandValue(operands, currentScope, symbolTable);
        operands.push(a >= b ? "1" : "0");
    }
    void evaluateLogicalAnd(std::stack<std::string>& operands, int currentScope, SymbolTable& symbolTable) {
        if (operands.size() < 2) throw std::runtime_error("Insufficient operands for logical AND");
        int b = resolveOperandValue(operands, currentScope, symbolTable);
        int a = resolveOperandValue(operands, currentScope, symbolTable);
        operands.push((a && b) ? "1" : "0");
    }
    void evaluateLogicalOr(std::stack<std::string>& operands, int currentScope, SymbolTable& symbolTable) {
        if (operands.size() < 2) throw std::runtime_error("Insufficient operands for logical OR");
        int b = resolveOperandValue(operands, currentScope, symbolTable);
        int a = resolveOperandValue(operands, currentScope, symbolTable);
        operands.push((a || b) ? "1" : "0");
    }
    void evaluateLogicalNot(std::stack<std::string>& operands, int currentScope, SymbolTable& symbolTable) {
        if (operands.empty()) throw std::runtime_error("Insufficient operands for logical NOT");
        int a = resolveOperandValue(operands, currentScope, symbolTable);
        operands.push(!a ? "1" : "0");
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
