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

    // TODO: Finish writing me! I'm just a skeleton!
    // PA4: populateDeclaredFunction();
    // populates a node of the Symbol table that assumes the identifier we’ve hit is a Function.
    // NOTE: Requires the populateParameter() function when parenthesis is hit in the symbol tree.
    // Parameters: All of these parameters are for the existsInTable() function atm that checks if the current
    // node is the symbol table already.
    void populateDeclaredFunction(int currentScope, int scopeNum, const string& type, LCRSTree CST) {
        // Grab current node
        Node* current=CST.getCurrentNode();

        // Check to see if the current node is a function
        if(current->data.getType()=="FUNCTION")
        {

            // Do another check, in here to verify this isn't already in the symbol table
            if (existsInTable(currentScope,scopeNum,type,CST)) {
                // Call the error statement function for this part.
            }
            // Do thing
            else {
                // Do something most-likely with populateParameter() function.
            }
        }

        // Otherwise...
        else {
            // Call the errorStatement() function for this part
        }
    }

    // TODO: Finish me! I'm currently a skeleton!
    // PA4: errorStatement()
    // Prints out and error statement to console and specifies what scope and type the node was.
    // Parameters copy existsInTable() function for now. fromwhere specifies what function it is from i.e. 
    // "populateDeclaredFunction". 
    void errorStatement(string fromwhere, int currentScope, int scopeNum, const string& type, LCRSTree CST) {
        cout << fromwhere << " error with either scope or already existing variable on symbol table." << " Scope was: " 
        << currentScope << " variable was: " << type << endl;
        exit(-1);
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
};

#endif //CS460_SYMBOLTABLE_H
