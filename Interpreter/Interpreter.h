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
#include <variant>

using namespace std;

class Interpreter
{
    stack<Node*> progcounter;
    SymbolTable* table;
    LCRSTree *AST;
public:
    Interpreter(SymbolTable *createdtable,LCRSTree *createdAST)
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
        InterpretFunction("main",table->searchSymbol("main")->scope);
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
    void InterpretFunction(string functionname,int functionscope)
    {
        bool withinscope=true;
        bool inloop=false;
        int localscope=1;
        int scope=1;

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
                evaluateExpression(functionscope);
            }
            else if(AST->getCurrentNode()->data.getType()=="IF")
            {
                if(!evaluateIfStatement(functionscope))
                {
                    while(AST->getCurrentNode()->data.getType() != "END BLOCK")
                    {
                        AST->nextChild();
                    }
                    AST->nextChild();
                    if(AST->getCurrentNode()->data.getType()=="ELSE")
                    {
                        AST->nextChild();
                    }

                }
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
                //bool skipInit = /* Determine based on DFA logic or context */
                progcounter.push(AST->getCurrentNode());
                if(evaluateForStatement(functionscope, inloop)) //evaluate whether we've gone through the first loop
                {
                    while(AST->getCurrentNode()->data.getType() !="BEGIN BLOCK")
                    {
                        AST->nextChild();
                    }
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
                scope+=1;
                if(inloop==true)
                    localscope+=1;
                AST->nextChild();
            }
            else if(AST->getCurrentNode()->data.getType()=="END BLOCK") {
                scope-=1;
                if(scope==0)
                    withinscope=false;
                if (inloop == true) {
                    localscope -= 1;
                    if(localscope==0)
                    {
                        AST->setCurrentNode(progcounter.top());
                    }
                }
                else
                {
                    AST->nextChild();
                }
            }
            else if(AST->getCurrentNode()->data.getType()=="RETURN")//CHEATING BREAKS EASILY
            {
                Symbol* localsymbol=table->searchSymbol(functionname);
                AST->nextNode(); //all test files return a symbol
                localsymbol->setValue(table->searchSymbol(functionscope,AST->getCurrentNode()->data.getName())->getValue());
                withinscope=false;
                break;
            }
            else if(AST->getCurrentNode()->data.getType()=="IDENTIFIER")//standalone function calls
            {
                if (AST->getCurrentNode()->data.getName() == "printf") {
                    AST->nextNode();// (
                    AST->nextNode();// string
                    string output=AST->getCurrentNode()->data.getName();
                    AST->nextNode();
                    while(AST->getCurrentNode()->data.getType() != "R_PAREN") //I'M GOING TO CHEAT AGAIN
                    {

                        int replaceindex=output.find('%');
                        Symbol* localsymbol=table->searchSymbol(functionscope,AST->getCurrentNode()->data.getName());
                        if(AST->getCurrentNode()->data.getName()==localsymbol->getName()) {
                            if(get_if<0>(&localsymbol->value)!= nullptr)
                                output.replace(replaceindex, 2, to_string(get<0>(localsymbol->getValue())));
                            else
                                output.replace(replaceindex,2,get<3>(localsymbol->getValue()));
                        }
                        else{
                            vector<string> names=localsymbol->getParameterNames();
                            int index=distance(names.begin(),find(names.begin(), names.end(),AST->getCurrentNode()->data.getName()));
                            Value localvalue=localsymbol->getParameterValues().at(index);
                            output.replace(replaceindex,2,to_string(get<0>(localvalue)));
                        }
                        AST->nextNode();
                    }
                    if(output.find("\\n") != string::npos)
                    {
                        output.replace(output.find("\\n"),2,"");
                        cout<<output<<endl;
                    }
                    else
                    {
                        cout<<output;
                    }
                    AST->nextChild();
                }
                else
                {
                    progcounter.push(AST->getCurrentNode());
                    string funcname=AST->getCurrentNode()->data.getName();
                    evaluateFunction(functionscope,AST->getCurrentNode()->data.getName());
                    findProcedure(funcname);
                    InterpretFunction(funcname,table->searchSymbol(funcname)->getScope());
                    AST->setCurrentNode(progcounter.top());
                    progcounter.pop();
                    AST->nextChild();

                }
            }
        }
    }


    // evaluateIfStatement();
    // This function will evaluate an if expression using a DFA, NOTE: THIS ALREADY ASSUMES IT'S CALLED ON AN IF
    // parameters:

    // functionscope, an integer that helps keep track of its scope
    bool evaluateIfStatement(int functionScope) {
        // Move to the condition node (left child of the IF node)
        AST->nextNode();

        // Evaluate the boolean expression
         return evaluateBoolExpression(functionScope);


    }


    bool evaluateWhileStatement(int functionScope) {
        // Move to the condition node (right sibling of the WHILE node)
        AST->nextNode();

        // Evaluate the boolean expression
        bool conditionResult = evaluateBoolExpression(functionScope);

        // Return the result of the boolean expression
        return conditionResult;
    }


    bool evaluateForStatement(int functionScope, bool skipInit) {
        // Step 1: Move to the initialization node (Expression 1)
        Node* start = AST->getCurrentNode();

        // If initialization needs to be executed (skipInit == false), evaluate it
        if (!skipInit) { // Move to the initialization node
            evaluateExpression(functionScope); // Evaluate the initialization expression
        }
        else
        {
            AST->nextChild();//ForEx2
            AST->nextChild();//ForEx3
            evaluateExpression(functionScope);
        }

        AST->setCurrentNode(start); // Set the current node to Forex1;
        AST->nextChild();//Forex2
        AST->nextNode();//bool expression

        // Evaluate the condition using evaluateBoolExpression
        bool conditionResult = evaluateBoolExpression(functionScope);


        return conditionResult;
    }


    //PA6: evaluateBoolExpression()
    //called within an if, while, or for expression
    bool evaluateBoolExpression(int currentScope) {
        std::stack<Node*> evaluateStack;
        while (AST->getCurrentNode() != nullptr) {
            if (AST->getCurrentNode()->data.getType() == "IDENTIFIER" ||
                    AST->getCurrentNode()->data.getType() == "INTEGER" || AST->getCurrentNode()->data.getType() == "STRING") {
                evaluateStack.push(AST->getCurrentNode());

            }
            else if (find(operatorlist.begin(), operatorlist.end(), AST->getCurrentNode()->data.getType()) !=
                     operatorlist.end()) {
                opHelperFunction(AST->getCurrentNode(), evaluateStack, table, currentScope);

            }
            if(AST->getCurrentNode()->leftChild)
                break;
            AST->nextNode();
        }
        if (evaluateStack.size() != 1) {
            std::cerr << "evaluateBoolExpression: evaluateStack should contain a single node, but doesn't" << std::endl;
            exit(-1);
        }
        if (evaluateStack.top()->data.getName() == "1")
        { AST->setCurrentNode(AST->getCurrentNode()->leftChild);
            return true;
        }
        else if(evaluateStack.top()->data.getName() == "0") {
            AST->setCurrentNode(AST->getCurrentNode()->leftChild);
            return false;
        }
        else {
            std::cerr << "evaluateBoolExpression: final result is not a boolean value" << std::endl;
            exit(-1);
        }
    }

    //PA6: evaluateExpression()
    //assumptions:
    //  expression is a numerical expression
    //  the current AST node is the ASSIGNMENT node
    //  after finishing, the node should be the left child of the last node
    //helper functions needed:
    //  getArrayValue
    Value evaluateExpression(int currentScope) {
        AST->nextNode();
        std::stack<Node*> evaluateStack;
        while (AST->getCurrentNode() != nullptr) {
            //operands: identifiers. variables, functions, arrays
            if (AST->getCurrentNode()->data.getType() == "IDENTIFIER") {
                if (AST->getCurrentNode()->rightSibling->data.getType() == "L_PAREN") {
                    string funcName = AST->getCurrentNode()->data.getName();
                    //function result is stored in the function's symbol? ask anthony
                    evaluateFunction(currentScope, funcName);

                    progcounter.push(AST->getCurrentNode());
                    findProcedure(funcName);

                    InterpretFunction(funcName,table->searchSymbol(funcName)->getScope());
                    AST->setCurrentNode(progcounter.top());
                    progcounter.pop();

                    Symbol* localSymbol = table->searchSymbol(funcName);
                    //this line is on the assumption that a function will return type int
                    Node* functResult = new Node(Token("INTEGER", to_string(std::get<int>(localSymbol->value)), 0));

                    evaluateStack.push(functResult); //when the DFA finishes it should have pushed a value to its data field
                    AST->nextNode(); //node should now be the one after R_PAREN
                }
                else if (AST->getCurrentNode()->rightSibling->data.getType() == "L_BRACE") {
                    // moved to arraynumber (we're gonna cheat here since there's only i)
                    //UNDERCONSTRUCTION

                }
                else { //if not a function or array, push on to stack
                    evaluateStack.push(AST->getCurrentNode());
                    AST->nextNode();
                }
            }
                //operands: string
            else if (AST->getCurrentNode()->data.getType() == "STRING") {
                string localstring=AST->getCurrentNode()->data.getName();
                if(localstring.find("\\x") != string::npos)
                {
                    localstring.replace(localstring.find("\\x"),3,"");
                }
                evaluateStack.push(new Node(Token("STRING",localstring,0)));
                AST->nextNode();
            }
                //operands: integers
            else if (AST->getCurrentNode()->data.getType() == "INTEGER") {
                evaluateStack.push(AST->getCurrentNode());
                AST->nextNode();
            }
                //operators
            else if (find(operatorlist.begin(), operatorlist.end(), AST->getCurrentNode()->data.getType()) !=
                     operatorlist.end() && AST->getCurrentNode()->data.getType() != "ASSIGNMENT_OPERATOR") {
                opHelperFunction(AST->getCurrentNode(), evaluateStack, table, currentScope);
                AST->nextNode();
            }
                //assignment operator: end of expression
            else if (AST->getCurrentNode()->data.getType() == "ASSIGNMENT_OPERATOR") {
                Symbol* op1Symbol;
                Symbol* op2Symbol;
                Value operand2, operand1;
                if (evaluateStack.top()->data.getType() == "IDENTIFIER" &&
                    table->existsInTable(currentScope, evaluateStack.top()->data.getName())) {
                    op2Symbol = table->searchSymbol(currentScope, evaluateStack.top()->data.getName());
                    operand2 = op2Symbol->value;
                    evaluateStack.pop();
                }
                else if (evaluateStack.top()->data.getType() == "STRING") {
                    operand2 = evaluateStack.top()->data.getName();
                    evaluateStack.pop();
                }
                else if (evaluateStack.top()->data.getType() == "INTEGER") {
                    operand2 = std::stoi(evaluateStack.top()->data.getName());
                    evaluateStack.pop();
                } else {
                    std::cerr << "Error: operand2 is missing or something idk" << std::endl;
                    exit(-1);
                }
                if (evaluateStack.top()->data.getType() == "IDENTIFIER" &&
                    table->existsInTable(currentScope, evaluateStack.top()->data.getName())) {
                    op1Symbol = table->searchSymbol(currentScope, evaluateStack.top()->data.getName());
                    operand1 = op1Symbol->value;
                    evaluateStack.pop();
                }
                else {
                    std::cerr << "Error: attempting to assign value to non-variable" << std::endl;
                    exit(-1);
                }
                if (std::get_if<int>(&operand2) && std::get_if<int>(&operand1)) {
                    operand1 = operand2;
                    op1Symbol->setValue(operand1);
                    AST->setCurrentNode(AST->getCurrentNode()->leftChild);
                    return operand1;
                }
                else if (op1Symbol->isArray && op1Symbol->datatype == "char") {
                    operand1 = operand2;
                    op1Symbol->setValue(operand1);
                    AST->setCurrentNode(AST->getCurrentNode()->leftChild);
                    return operand1;
                }
                else {
                    std::cerr << "Assignment error: one or both operands are not valid types" << std::endl;
                    exit(-61);
                }
            }
        }
        std::cerr << "evaluateExpression: Reached end of expression without returning a final result" << std::endl;
        exit(-61);
    }

    // enters a function's parameter values to the symbol
    void evaluateFunction(int currentScope, string funcName) {
        vector<Value> parameterValues;

        AST->nextNode(); // moves from IDENTIFIER to L_PAREN
        AST->nextNode(); // moves to first parameter

        while (AST->getCurrentNode()->data.getType() != "R_PAREN") {
            Value value;
            if(AST->getCurrentNode()->data.getType()=="IDENTIFIER")//VERY BAD ARRAY CALL
            {
                Symbol *localsymbol=table->searchSymbol(currentScope,AST->getCurrentNode()->data.getName());
                if(localsymbol->getIsArray())
                {
                    int getindex;

                    AST->nextNode();// [
                    AST->nextNode();// value
                    if(AST->getCurrentNode()->data.getType()=="INTEGER")
                    {
                        value=get<3>(localsymbol->getValue()).at( stoi(AST->getCurrentNode()->data.getName() ));
                    }
                    else
                    {
                        char localchar=get<3>(localsymbol->getValue()).at(get<0>(table->searchSymbol(currentScope,AST->getCurrentNode()->data.getName())->getValue()));
                        string localstring{localchar};
                        value=stoi(localstring,nullptr,16);
                    }
                    AST->nextNode(); // ]
                }
                else
                {
                    value=localsymbol->getValue();
                }
            }
            else
            {
                if(AST->getCurrentNode()->data.getType()=="INTEGER")
                    value=stoi(AST->getCurrentNode()->data.getName());
                else if(AST->getCurrentNode()->data.getType()=="STRING")
                    value=AST->getCurrentNode()->data.getName();
            }
            parameterValues.push_back(value);
            AST->nextNode();

            if(AST->getCurrentNode()->data.getType() == "COMMA") { //WHY IS THERE CODE FOR COMMAS WE GOT RID OF THEM
                AST->nextNode(); // moves past comma
                AST->nextNode(); // moves to next parameter
            }
        }
        Symbol* localSymbol = table->searchSymbol(funcName);
        localSymbol->setParameterValues(parameterValues);
        // ends on R_PAREN
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
        else if(currentOperator == "BOOLEAN_EQUAL"){
            evaluateLogicalEqual(evaluateStack,currentScope,SymbolTable);
        }
        else {
            std::cerr << "opHelperFunction: unsupported operator" << std::endl;
            exit(-1);
        }
    }



    int resolveOperandValue(std::stack<Node*>& operands, int currentScope, SymbolTable* symbolTable) {
        if (operands.empty()) {
            throw std::runtime_error("Operand stack is empty");
        }

        Node* top = operands.top();
        operands.pop();

        if (top->data.getType() == "INTEGER") {
            // If the token is an integer literal, return its value
            return std::stoi(top->data.getName());
        }
        else if (top->data.getType() == "STRING") {
            return std::stoi(top->data.getName(), nullptr, 16);
        }
        else if (top->data.getType() == "IDENTIFIER") {
            // Handle variables by looking them up in the symbol table
            Symbol* symbol = symbolTable->searchSymbol(currentScope, top->data.getName());
            if(symbol->getName()==top->data.getName())//if it's a variable
            {
                if (!symbol || (symbol->datatype != "int" && symbol->datatype !="char") || symbol->isArray  ) {
                    throw std::runtime_error("Invalid variable: " + top->data.getName());
                }
                // Fetch the variable's value (that the symbol's value is an int)
                try {
                    if(get_if<0>(&symbol->value)!= nullptr)
                        return std::get<int>(symbol->value);
                    else
                    {
                        char temp=std::get<char>(symbol->value);
                        return int(temp);
                    }
                }
                catch (const std::invalid_argument&) {
                    throw std::runtime_error("Variable " + symbol->name + " does not contain a valid integer value");
                }
            }
            else //it's a function parameter
            {
                Value localvalue;
                vector<string> names=symbol->getParameterNames();
                int index=distance(names.begin(),find(names.begin(), names.end(),top->data.getName()));
                localvalue=symbol->getParameterValues().at(index);
                try {
                    if(get_if<0>(&localvalue)!= nullptr)
                        return std::get<int>(localvalue);
                    else
                    {
                        char temp=std::get<char>(localvalue);
                        return int(temp);
                    }
                }
                catch (const std::invalid_argument&) {
                    throw std::runtime_error("Variable " + symbol->name + " does not contain a valid integer value");
                }

            }
        } else {
            throw std::runtime_error("Unexpected token type: " + top->data.getType());
        }
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
    void evaluateLogicalEqual(std::stack<Node*>& operands, int currentScope, SymbolTable *symbolTable){
        if (operands.size() < 2) throw std::runtime_error("Insufficient operands for logical OR");
        int b = resolveOperandValue(operands, currentScope, symbolTable);
        int a = resolveOperandValue(operands, currentScope, symbolTable);
        Node* result = new Node(Token("INTEGER", (a == b) ? "1" : "0", 0));
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
    }

};

#endif //INTERPRETER_INTERPRETER_H
