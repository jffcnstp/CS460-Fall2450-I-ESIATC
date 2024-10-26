#ifndef CS460_SYMBOLTABLE_H
#define CS460_SYMBOLTABLE_H

#include<string>
#include<vector>

using namespace std;

struct Symbol {
    string name;
    string type;
    string datatype;
    bool isArray;
    int arraySize;
    int scope;

    //Parameter Information
    vector<string> parameterNames;
    vector<string> parameterDatatypes;
    vector<bool> isParameterArray;
    vector<int> parametersArraySizes;

    Symbol *next;

    Symbol () : isArray(false), arraySize(0), scope (0), next(nullptr) {};

    Symbol(string name, string type, string datatype, bool isArray = false, int arraySize = 0, int scope = 0)
            : name(name), type(type), datatype(datatype), isArray(isArray), arraySize(arraySize), scope(scope), next(nullptr) {}
};

class SymbolTable{
    Symbol* Root;
    Symbol* Tail;
    Symbol* Traversal;

public:
    SymbolTable(){Root= nullptr;Tail= nullptr;Traversal= nullptr;}

    void addSymbol(Symbol *entry)
    {
        if(Root== nullptr)
        {
            Root=entry;
            Tail=entry;
            Traversal=Root;
            return;
        }
        Tail->next=entry;
        Tail=entry;


    }

    // DFA FOR BUILDING THE SYMBOL TABLE
    // USES LOCAL VARIABLES currentscope and scopenum to track current scope (currentscope tracks how deep we are in a function scopenum tracks which function we are in)
    //
    void BuildTable(LCRSTree CST)
    {
        int currentscope =0;//not finalized need to think about whether this 2 int system is good enough
        int scopenum=0;
        while(!CST.EOT())
        {
            Node* current=CST.getCurrentNode();
            if(current->data.getType()=="FUNCTION")
            {
                scopenum+=1;
                //addFunction( scopenum)
            }
            else if(current->data.getType()=="PROCEDURE")
            {
                scopenum+=1;
                //addProcedure(scopenum)
            }
            else if(current->data.getType()=="IDENTIFIER")
            {
                for(auto &keyword : typekeyword)
                {
                    if(keyword == current->data.getName()) {
                        //addVariable(scope);
                        break;
                    }
                }
            }
            else if(current->data.getType()=="L_BRACE" )
            {
                currentscope+=1;
                CST.nextNode();
            }
            else if(current->data.getType()=="R_BRACE")
            {

                currentscope-=1;
                CST.nextNode();
            }
            else
                CST.nextChild(); //skips over the entire Sibling chain into the next Child
        }
    }

    //checks if an identifier exists within the symbol table
    bool existsInTable(int currentScope, int scopeNum, const string& type, LCRSTree CST) {
        Symbol* currentSymbol = Root;
        Node* currentNode = CST.getCurrentNode();
        currentNode = currentNode->rightSibling;

        if (type == "IDENTIFIER" || type == "PROCEDURE") {
            while (currentSymbol->next != nullptr) {
                if (currentNode->data.getName() == currentSymbol->name /*scope stuff goes here*/) {
                    return true;
                }
                currentSymbol = currentSymbol->next;
            }
        }
        else { //type == function
            currentNode = currentNode->rightSibling;
            while (currentSymbol->next != nullptr) {
                if (currentNode->data.getName() == currentSymbol->name /*scope stuff goes here*/) {
                    return true;
                }
                currentSymbol = currentSymbol->next;
            }
        }
        return false;
    }


    void populateDeclaredProcedure(SymbolTable &table, LCRSTree &CST, int currentScope, int scopeNum) {
        Node* currentNode = CST.getCurrentNode();

        // Verify that current node is indeed an identifier for a procedure
        if (currentNode->data.getType() != "PROCEDURE") {
            error("Expected PROCEDURE type but found: " + currentNode->data.getType());
            return;
        }

        // Extract procedure name and data type
        string procedureName = currentNode->data.getName();
        string procedureType = currentNode->data.getType(); // Should be "PROCEDURE" for type consistency
        string procedureDataType = "void"; // Default to void if no return type is expected

        // Check if procedure already exists in the symbol table
        if (table.existsInTable(currentScope, scopeNum, procedureType, CST)) {
            error("Procedure already declared in the current scope: " + procedureName);
            return;
        }

        // Create a new Symbol entry for the procedure
        Symbol *newProcedure = new Symbol(procedureName, procedureType, procedureDataType, false, 0, currentScope);

        // Move to the next node to check for parameter list (i.e., an open parenthesis)
        CST.nextNode();
        currentNode = CST.getCurrentNode();
        if (currentNode->data.getType() != "L_PAREN") {
            error("Expected '(' after procedure name for parameter list.");
            delete newProcedure; // Clean up in case of error
            return;
        }

        // Process parameters between parentheses
        CST.nextNode(); // Move inside the parentheses
        currentNode = CST.getCurrentNode();
        while (currentNode->data.getType() != "R_PAREN" && !CST.EOT()) {
            populateParameter(*newProcedure, CST); // Add parameter details to newProcedure
            CST.nextNode();
            currentNode = CST.getCurrentNode();
        }

        // Check if the right parenthesis was reached correctly
        if (currentNode->data.getType() != "R_PAREN") {
            error("Expected ')' at the end of the parameter list.");
            delete newProcedure;
            return;
        }

        // Add the newly populated procedure entry to the symbol table
        table.addSymbol(newProcedure);
    }

    void error(const std::string& message) const {
        std::cerr << "Symbol Table Error: " << message << std::endl;
        
        // Possible additions could include logging to a file, halting certain operations, etc.
    }


};

#endif //CS460_SYMBOLTABLE_H
