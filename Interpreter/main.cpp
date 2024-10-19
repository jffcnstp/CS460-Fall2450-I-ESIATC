#include <iostream>
#include <queue>
#include <string>
#include <vector>
#include <fstream>
#include "CommentRemove.h"
#include "Token.h"

using namespace std;

std::string typekeyword[]={"int","bool","char","void"};
std::string conditionalkeyword[]={"if","for","while","else"};


enum keywords{Identifier=0,Type=1,Conditional=2,RETURN=3};
// Node of the LCRS Tree
struct Node {
    Token data;
    Node* leftChild;
    Node* rightSibling;

    Node(Token token) : data(token), leftChild(nullptr), rightSibling(nullptr) {}
};



class LCRSTree {
private:
    Node* currentNode;

    // Recursive helper function to delete nodes
    void deleteNode(Node* node) {
        if (!node) return;
        deleteNode(node->leftChild);
        deleteNode(node->rightSibling);
        delete node;
    }

public:
    Node* root;

    LCRSTree() : root(nullptr), currentNode(nullptr) {}

    ~LCRSTree() { deleteNode(root); }

    // Insert child under the given parent
    void insertChild(Node* child) {
       currentNode->leftChild=child;
       currentNode=child;
    }

    //Purpose: inserts a sibling into the LCRS tree
    //input:  a Node
    //output:  the node is attached to the current node's sibling pointer.  Current node is changed to the sibling node
    void insertSibling(Node* sibling)
    {
        currentNode->rightSibling=sibling;
        currentNode=sibling;
    }

    // Get the current node (most recently visited in traversal)
    Node* getCurrentNode() {
        return currentNode;
    }


    // Breadth-First Traversal
    void breadthFirstTraversal() {
        if (!root) return;

        queue<Node*> q;
        q.push(root);

        while (!q.empty()) {
            currentNode = q.front();  // Update current node
            q.pop();

            // Print current node data
            cout << currentNode->data.getName() << " ";

            // Add the left child to the queue
            if (currentNode->leftChild) {
                q.push(currentNode->leftChild);
            }


            Node* sibling = currentNode->leftChild;
            while (sibling) {
                if (sibling->rightSibling) {
                    q.push(sibling->rightSibling);
                }
                sibling = sibling->rightSibling;
            }
        }

        cout << endl;
    }
};




class Parser {
public:
    std::vector<Token> tokens;
    int current = 0;
    LCRSTree tree;

    Parser(const std::vector<Token>& tokenList) : tokens(tokenList), current(0), tree() {}

    Token& peek() {
        return tokens[current];
    }

    Token& nextToken() {
        current++;
        if(current<tokens.size())
            return tokens[current];
        else
            return tokens[current-1];
    }

    bool match(const string& type) {
        if (peek().getType() == type) {
            nextToken();
            return true;
        }
        return false;
    }

    // Helper method to add nodes to the tree
    Node* addNode(Token data) {
        Node* node = new Node(data);
        tree.insertChild(node);
        return node;
    }


    //PURPOSE: checks the parameter string  for what type of identifier it is and returns an int
    //INPUT:  string name (ideally the string passed is from Token.name
    //Output: an enum which describes the identifier.
    int keywordcheck(string name)
    {
        for(auto &word: typekeyword)
        {
            if(name==word)
                return Type;
        }
        for(auto &word:conditionalkeyword)
        {
            if(name==word)
                return Conditional;
        }
        if(name == "return")
        {
            return RETURN;
        }
        return Identifier;
    }

    //THE MAIN FUNCTION FOR BUILDING THE CONCRETE SYNTAX TREE
    //Input : Parser.tokens[] , local int bracecounter, local vector<int> bracelocation
    //Output : populates Parser.tree
    void buildCST()
    {
        int bracecounter=0;
        vector<int> bracelocation;

        Token currenttoken=peek();
        while(current < tokens.size())
        {
            if(match("FUNCTION"))
                parseFunctionDeclaration();
            if(match("IDENTIFIER"))
            {
                int keyword=keywordcheck(peek().getName());
                switch(keyword)
                {
                    case(Type):
                    {
                        parseVariableDeclaration(false);
                    }
                    case(Identifier):
                    {
                       // parseVariableOperation();
                    }
                    case(RETURN):
                    {
                       // parseReturn();
                    }
                }
            }
            if(match("PROCEDURE"))
            {
                parseProcedure();
            }
            if(match("IF")||match("WHILE"))
            {
                //parseiforwhile statement
            }
            if(match("ELSE"))
            {
                //parse else
            }
            if(match("FOR"))
            {

            }

            //procedure declaration
            if(match("SEMICOLON"))
            {parseSemicolon();}

            if(match("L_BRACE") || match("R_BRACE"))
            {parseBrace(bracecounter,bracelocation);}

        }

        if(bracecounter>0)
        {
            cout<<"missing R_BRACE for "<<bracecounter<<" L_BRACE at lines: ";
            for(auto &number : bracelocation)
            {
                cout<<number<<" ";
            }
            exit(-1);
        }
    }
    // parses tokens expecting the syntax for procedure when the token found is PROCEDURE
    void parseProcedure()
    {
        tree.insertChild(new Node(peek()));
        Token tokenused=nextToken();

        if(match("IDENTIFIER"))
        {
            tree.insertSibling(new Node(tokenused));
            tokenused=nextToken();
        }
        else
            Errorstatement("Procedure",tokenused);

        if(match("L_PAREN"))
        {
            tree.insertSibling(new Node(tokenused));
            tokenused=nextToken();
        }
        else
            Errorstatement("Procedure",tokenused);

        if(match("IDENTIFIER"))
        {
            //THIS ONLY WORKS IF THE PROCEDURE IS MAIN.  IF HE MAKES A PROCEDURE THAT HAS DATA TYPES OR IS A FUNCTION DECLARATION CHANGE THIS
            tree.insertSibling(new Node(tokenused));
            tokenused=nextToken();
        }
        else
            Errorstatement("Procedure",tokenused);

        if(match("R_PAREN"))
        {
            tree.insertSibling(new Node(tokenused));
            tokenused=nextToken();
        }
        else
            Errorstatement("Procedure",tokenused);


    }


    //this function is called when the token is found to be FUNCTION
    //input: local variable Token tokenused, local variablle bool multipleparameters
    //output: creates a whole sibling chain out of a function declaration and adds it to the tree
    void parseFunctionDeclaration()
    {

        bool multipleparameters=true;
        tree.insertChild(new Node(peek())); //add the function to the tree
        nextToken();

        if(match("IDENTIFIER")  && keywordcheck(peek().getName())==Type)
            tree.insertSibling(new Node(peek()));
        else
            Errorstatement("FunctionDeclaration",peek());

        nextToken();

        if(match("IDENTIFIER")  && keywordcheck(peek().getName())==Identifier)
            tree.insertSibling(new Node(peek()));
        else
            Errorstatement("FunctionDeclaration",peek());
        nextToken();

        if(match("L_PAREN") )
            tree.insertSibling(new Node(peek()));
        else
            Errorstatement("FunctionDeclaration",peek());
        nextToken();

        while(multipleparameters==true)
        {
            multipleparameters = false;
            parseFunctionDeclarationParameter();

            if(match("COMMA"))
            {
                tree.insertSibling((new Node(peek())));
                nextToken();
                multipleparameters = true;
            }
        }
        if(match("R_PAREN"))
            tree.insertSibling(new Node(peek()));
        else
            Errorstatement("FunctionDeclaration",peek());

        nextToken();
    }

    void parseFunctionDeclarationParameter()
    {
        Token tokenused=peek();

        if(match("IDENTIFIER") && keywordcheck(tokenused.getName())==Type)
        {
            tree.insertSibling(new Node(tokenused));
            tokenused=nextToken();
        }
        else
            Errorstatement("FunctionDeclarationParameter",tokenused);

        if (match("IDENTIFIER") && keywordcheck(tokenused.getName()) == Identifier) {
            tree.insertSibling(new Node(tokenused));
            tokenused = nextToken();
        } else
            Errorstatement("VariableDeclaration", tokenused);

        if (match("L_BRACKET")) {
            parseBracket();
        }

        if(match("ASSIGNMENT_OPERATOR")){
            //parseNumerical();
        }

    }

    //Fuction purpose:  parses a snippet of code following the format keyword->identifier->(optional []) -> optional(=)
    void parseVariableDeclaration()
    {
        bool loop=true;
        tree.insertChild(new Node(peek()));


        while(loop) {
            loop = false;

            if (match("IDENTIFIER") && keywordcheck(peek().getName()) == Identifier) {
                tree.insertSibling(new Node(peek()));
                nextToken();
            } else
                Errorstatement("VariableDeclaration", peek());

            if (match("L_BRACKET")) {
                parseBracket();
            }
            if (match("ASSIGNMENT_OPERATOR")) {
                //parseenumerical/booleanexpression();
            }
            if(match("COMMA"))
            {
                loop = true;
                tree.insertSibling(new Node(peek()));
                nextToken();
            }
        }

    }

    //this function is called when the token is a string (quote -> string -> quote)
    void parseString() {
        //i assume that at this point, the current token is known to be a DblQuote or SglQuote
        Token tokenused = peek();
        if (match("DOUBLE_QUOTE") ) {
            tree.insertSibling(new Node(tokenused));
            tokenused = nextToken();

            //invalid: last char of string is "\"

            if ( !match("STRING") || tokenused.getName().back() == '\\')
                Errorstatement("String", tokenused);
            else
                tree.insertSibling(new Node(tokenused));
            tokenused = nextToken();

            if (match("DOUBLE_QUOTE")  )
                tree.insertSibling(new Node(tokenused));
            else
                Errorstatement("String", tokenused);
            tokenused = nextToken();
        }
        else if (match("SINGLE_QUOTE") ) {
            tree.insertSibling(new Node(tokenused));
            tokenused = nextToken();

            //invalid: last char of string is "\"

            if (!match("STRING")||  tokenused.getName().back() == '\\'  )
                Errorstatement("String", tokenused);
            else
                tree.insertSibling(new Node(tokenused));
            tokenused = nextToken();

            if (match("SINGLE_QUOTE") )
                tree.insertSibling(new Node(tokenused));
            else
                Errorstatement("String", tokenused);
            tokenused = nextToken();
        }
    }

    //also for while statements?
    void parseIfWhileStatement() {

        //case1: if, case2: while
        if (match("IF")   && keywordcheck(peek().getName()) == Conditional) {
            tree.insertSibling(new Node(peek()));


            if (match("L_PAREN") )
                tree.insertSibling(new Node(peek()));
            else
                Errorstatement("If", peek());
            nextToken();

            //parseBoolean();

            if (match("R_PAREN")  )
                tree.insertChild(new Node(peek()));
            else
                Errorstatement("If", peek());
            nextToken();
        }

        else if (match("WHILE")   && keywordcheck(peek().getName()) == Conditional) {
            tree.insertSibling(new Node(peek()));
            nextToken();

            if (match("L_PAREN")  )
                tree.insertSibling(new Node(peek()));
            else
                Errorstatement("While", peek());
             nextToken();

            //parseBoolean();

            if (match("R_PAREN")  )
                tree.insertChild(new Node(peek()));
            else
                Errorstatement("While", peek());
            nextToken();
        }

        nextToken();
    }

    void parseElseStatement() {
        if (match("ELSE")   && keywordcheck(peek().getName()) == Conditional)
            tree.insertSibling(new Node(peek()));
        else
            Errorstatement("Else", peek());
        nextToken();

        if (match("L_PAREN")  )
            tree.insertSibling(new Node(peek()));
        else
            Errorstatement("Else", peek());
         nextToken();

        //parseBoolean()

        if (match("R_PAREN")  )
            tree.insertChild(new Node(peek()));
        else
            Errorstatement("Else", peek());
        nextToken();

    }


    // PA3: RDP - Bracket function
    // Individual function soley designed for handling brackets
void parseBracket() {
        // Add some kind of node
        Token tokenused = peek();

        // Expecting a '['

        if (match("L_BRACKET")) {
            tree.insertSibling(new Node(tokenused));
            tokenused = nextToken();
        }
        else {
            Errorstatement("Bracket", tokenused);
        }

        // Negative Integer error
        if(match("MINUS")) {
            Errorstatement("Bracket",tokenused);
        }

        // Expecting ']'

        if (match("R_BRACKET")) {
            tree.insertChild(new Node(tokenused));
            tokenused = nextToken();
        }

//        //TODO: Uncomment this when parseNumerical() is implemented
//        // Also make sure that we aren't going over tokens by removing "tokenused = nextToken();"
//        // Numerical Expression case
//        if(match("INTEGER")) {
//            parseNumerical();
//            tokenused = nextToken();
//        }
//        else {
//            Errorstatement("Bracket",tokenused);
//        }

            // Missing ']'
        else {
            Errorstatement("Bracket", tokenused);
        }
    nextToken();
    }

        // PA3: RDP - Brace function
    // Individual function soley designed for handling braces and things in-between
    // Parameters: braceCounter, helps keep track of number of '{' there are
    // braceLocation, helps keep track of the line number of a given brace
    void parseBrace(int braceCounter, vector<int> braceLocation) {
        // Add some kind of node
        Token tokenused = peek();

        // Expecting a '{'
        if (match("L_BRACE")) {
            braceCounter++; // increment
            braceLocation.push_back(tokenused.getLine()); // Push to vector
            tree.insertChild(new Node(tokenused));
            tokenused = nextToken();
        }
        // Expecting '}'
       else if (match("R_BRACE")) {
            braceCounter--; // decrement
            braceLocation.pop_back(); // Push to vector
            tree.insertChild(new Node(tokenused));
            tokenused = nextToken();
        }
       // Missing '}'
        else {
            Errorstatement("Brace", tokenused);
        }

        nextToken();
    }


    // PA3: RDP - Semicolon function
    // Individual function soley designed for handling semicolons
    void parseSemicolon() {
        // Init token
            tree.insertSibling(new Node(peek()));
            nextToken();
    }


// PA3: RDP - parseNumerical()
// Handles numerical expressions, recursively gathers all tokens until an invalid token appears
    void parseNumerical() {
        // init token
        Token tokenused = peek();
        
        // Base case: next token is not an integer or an operation
        // NOTE: be cautious with the last check, it might not be right.
        if (!match("INTEGER") && (!match("PLUS") || !match("MINUS") ||
            !match("DIVIDE") || !match("AESTERISK") || !match("MODULO") ||
            !match("CARET")) && (match("IDENTIFIER") && keywordcheck(tokenused.getName()) ==0)) {
            return;
        }

        // Recursive call: 
        tree.insertChild(new Node(tokenused));
        nextToken(); // unsure if this might screw it up
        parseNumerical();
    }



    void Errorstatement(string fromwhere,Token tokenused)
    {
        cout << fromwhere <<" error on line: " << tokenused.getLine() << " Token name: "<<tokenused.getName() <<"Token type: "<<tokenused.getType() << endl;
        exit(-1);
    }

};


// Main function
int main() {
    vector<Token> tokenlist;
    string fileName ="programming_assignment_3-test_file_";
    string tokenizefile="test_file";

//    for(int i=1; i <11; i++) {
//        ignoreComments(fileName + std::to_string(i) + ".c", tokenizefile + std::to_string(i) + ".c");
//        tokenlist = Tokenize(tokenizefile + std::to_string(i) + ".c");
//        continue;
//    }
    int i=1;
    ignoreComments(fileName + std::to_string(i) + ".c", tokenizefile + std::to_string(i) + ".c");
    tokenlist = Tokenize(tokenizefile + std::to_string(i) + ".c");
    Parser CST(tokenlist);
    CST.buildCST();

    return 0;
}
