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
    // USES LOCAL VARIABLES currentscope,bracecounter, and scopenum to track current scope (currentscope tracks how deep we are in a function scopenum tracks how many functions/procedures there are)
    //please consider 0 to be global scope when making your functions
    void BuildTable(LCRSTree CST)
    {
        int currentscope =0,scopenum=0,bracecounter=0;//not finalized need to think about whether this 2 int system is good enough
        while(!CST.EOT())
        {
            Node* current=CST.getCurrentNode();
            if(current->data.getType()=="FUNCTION")
            {
                scopenum+=1;
                //addFunction( currentscope)
            }
            else if(current->data.getType()=="PROCEDURE")
            {
                scopenum+=1;
                //addProcedure(currentscope)
            }
            else if(current->data.getType()=="IDENTIFIER")
            {
                for(auto &keyword : typekeyword)
                {
                    if(keyword == current->data.getName()) {
                        //addVariable(currentscope);
                        break;
                    }
                }
            }
            else if(current->data.getType()=="L_BRACE" )
            {
                bracecounter+=1;
                currentscope=scopenum;
                CST.nextNode();
            }
            else if(current->data.getType()=="R_BRACE")
            {
                bracecounter -=1;
                if(bracecounter == 0)
                    currentscope=0;

                CST.nextNode();
            }
            else
                CST.nextChild(); //skips over the entire Sibling chain into the next Child
        }
    }

    // PA4: populateDeclaredFunction();
    // populates a node of the Symbol table that assumes the identifier we’ve hit is a Function.
    // NOTE: Requires the populateParameter() function when parenthesis is hit in the symbol tree.
    // Parameters: All of these parameters are for the existsInTable() function atm that checks if the current
    // node is the symbol table already.
    void populateDeclaredFunction(int currentScope, int scopeNum, const string& type, LCRSTree &CST) {
        // Setup variables
        Symbol* currentSymbol = Root;
        bool multipleDeclaredFunctions = true;
        string varname;
        string vartype = CST.getCurrentNode()->data.getName();

        // Check to see if the current node is a function
        if(CST.getCurrentNode()->data.getType()=="FUNCTION")
        {
            // Do another check, in here to verify this isn't already in the symbol table
            if (existsInTable(currentScope,varname)) {
                // Call the error statement function for this part.
                // errorStatement("DeclaredFunction", currentScope, current);
            }
                // Do thing
            else {
                // TODO: uncomment me later
                // Do something most-likely with populateParameter() function.
                // PopulateDeclaredFunctionParameter(Symbol* newSym)
            }
        }

        CST.nextNode(); // Go past function declaration into a L_PAREN
        

        while (multipleDecalredFunctions) {
            multipleDecalredFunctions = false;
            varname=CST.getCurrentNode()->data.getName();

            if(CST.getCurrentNode()->data.getType()=="L_PAREN") {
                // FIXME: unsure if contents of Symbol is correct.
                // This should be adding the function declaration itself to the table, I think.
                addSymbol(new Symbol(varname,"datatype",vartype,true,stoi(CST.getCurrentNode()->data.getName()),currentscope))
                CST.nextNode(); // move to either end of function declaration or comma.
            }

            else {
                // Adding the current node, something like a int something, to table.
                addSymbol(new Symbol(varname,"datatype",vartype,false,0,currentscope));
            }

            // Comma case.
            if(CST.getCurrentNode()->data.getType()=="COMMA")
            {
                multiplevariables=true;
                CST.nextNode(); // Go next
            }

            // Check for a closing parenthesis
            if(CST.getCurrentNode()->data.getType()=="R_PAREN")
            {
                CST.nextNode(); // This is a valid function declaration
            }

            // Error statement...
            else
            {
                cout<<"THERE IS SUPPOSED TO BE A PARENTHESIS HERE.  INSTEAD IT'S A "<<CST.getCurrentNode()->data.getName() <<" TOKEN ON LINE: "<<CST.getCurrentNode()->data.getLine();
                exit(-2);
            }

        }
    }

    void populateDeclaredvariable(int currentscope,LCRSTree &CST)
    {

        bool multiplevariables=true;
        string varname;
        string vartype=CST.getCurrentNode()->data.getName();
        CST.nextNode();
        while(multiplevariables)
        {
            multiplevariables=false;
            varname=CST.getCurrentNode()->data.getName();
            //if(existsInTable(varname,currentscope))
            //      errorStatement("DeclaredVariable alreadyexists", currentscope,CST.getcurrentNode());
            CST.nextNode();// move to the L_BRACKET OR COMMA OR SEMICOLON NODE
            if(CST.getCurrentNode()->data.getType()=="L_BRACKET")
            {
                CST.nextNode();     //moves to the arraysize node
                addSymbol(new Symbol(varname,"datatype",vartype,true,stoi(CST.getCurrentNode()->data.getName()),currentscope));
                CST.nextNode();     //move to R_BRACKET
                CST.nextNode();     //move to COMMA OR SEMICOLON NODE
            }
            else
            {
                addSymbol(new Symbol(varname,"datatype",vartype,false,0,currentscope));
            }
            if(CST.getCurrentNode()->data.getType()=="COMMA")
            {
                multiplevariables=true;
                CST.nextNode();
            }
        }
        if(CST.getCurrentNode()->data.getType()=="SEMICOLON")
        {
            CST.nextNode();
        }
        else
        {
            cout<<"THERE IS SUPPOSED TO BE A SEMICOLON HERE.  INSTEAD IT'S A "<<CST.getCurrentNode()->data.getName() <<" TOKEN ON LINE: "<<CST.getCurrentNode()->data.getLine();
            exit(-2);
        }
    }

    // TODO: Finish me! I'm currently a skeleton!
    // PA4: errorStatement()
    // Prints out and error statement to console and specifies what scope and type the node was.
    // Parameters copy existsInTable() function for now. fromwhere specifies what function it is from i.e. 
    // "populateDeclaredFunction". 
    void errorStatement(string fromwhere, int currentScope, Node* node) {
        cout << fromwhere << " error with " << " Scope was: "
        << currentScope << " variable was: " << node->data.getType()<<" "<<node->data.getName()<<" on line "<<node->data.getLine() << endl;
        exit(-1);
    }

    //checks if an identifier exists within the symbol table
    //assumes no duplicate function names, so this only works on variables
    //call this when the current CST node is the identifier's name
    bool existsInTable(int currentScope, const string& name) {
        Symbol* currentSymbol = Root;

        while (currentSymbol->next != nullptr) { //loop through the entire Symbol Table

            if (currentSymbol->type == "datatype") {
                //name matches an identifier that exists in global or same scope
                if (name == currentSymbol->name &&
                    (currentSymbol->scope == 0 || currentSymbol->scope == currentScope)) {
                    return true;
                }
            }
            else {
                while (!currentSymbol->parameterNames.empty()) { //checking current CST node against current Symbol's parameters, if any
                    for (const auto &i: currentSymbol->parameterNames) {
                        if (name == i &&
                            (currentSymbol->scope == 0 || currentSymbol->scope == currentScope)) {
                            return true;
                        }
                    }
                }
            }

            currentSymbol = currentSymbol->next;
        }

        return false;
    }
};

#endif //CS460_SYMBOLTABLE_H
