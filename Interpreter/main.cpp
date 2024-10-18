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
        return tokens[current];
    }

    void error(const std::string& message) {
        std::cerr << "CST Error: " << message << " at line: " << tokens[current].line << std::endl;
        exit(EXIT_FAILURE); // Stop execution on error
    }

    void advance() {
        if (current < tokens.size() - 1) {
            current++;
        } else {
            error("Unexpected end of input");
        }
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
                        parseVariableDeclaration();
                    }
                    case(Identifier):
                    {
                        //variableoperation
                        //functioncall
                    }
                    case(RETURN):
                    {
                        //returnstatement
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
    void parseFunctionDeclaration()
    {
        Token tokenused=peek();
        bool multipleparameters=true;
        tree.insertChild(new Node(tokenused)); //add the function to the tree
        tokenused=nextToken();

        if(tokenused.getType() == "IDENTIFIER" && keywordcheck(tokenused.getName())==Type)
            tree.insertSibling(new Node(tokenused));
        else
            Errorstatement("FunctionDeclaration",tokenused);

        tokenused=nextToken();

        if(tokenused.getType() == "IDENTIFIER" && keywordcheck(tokenused.getName())==Identifier)
            tree.insertSibling(new Node(tokenused));
        else
            Errorstatement("FunctionDeclaration",tokenused);
        tokenused=nextToken();

        if(tokenused.getType() =="L_PAREN")
            tree.insertSibling(new Node(tokenused));
        else
            Errorstatement("FunctionDeclaration",tokenused);
        tokenused=nextToken();
        while(multipleparameters==true)
        {
            multipleparameters = false;
            if (tokenused.getType() == "IDENTIFIER" && keywordcheck(tokenused.getName()) == Type)
                tree.insertSibling(new Node(tokenused));
            else
                Errorstatement("FunctionDeclaration", tokenused);
            tokenused = nextToken();

            if (tokenused.getType() == "IDENTIFIER" && keywordcheck(tokenused.getName()) == Identifier)
                tree.insertSibling(new Node(tokenused));
            else
                Errorstatement("FunctionDeclaration", tokenused);
            tokenused = nextToken();

            if(match("COMMA"))
            {
                tree.insertSibling((new Node(peek())));
                tokenused=nextToken();
                multipleparameters = true;
            }
        }
        if(tokenused.getType()=="R_PAREN")
            tree.insertSibling(new Node(tokenused));
        else
            Errorstatement("FunctionDeclaration",tokenused);

        nextToken();
    }

    void parseVariableDeclaration()
    {
        tree.insertChild(new Node(peek()));
        Token tokenused = nextToken();

        if(match("IDENTIFIER") && keywordcheck(tokenused.getName())==0)
        {

        }
        else
            Errorstatement("VariableDeclaration",tokenused);

    }

    //this function is called when the token is a string (quote -> string -> quote)
    void parseString() {
        //i assume that at this point, the current token is known to be a DblQuote or SglQuote
        Token tokenused = peek();
        if (tokenused.getType()=="DOUBLE_QUOTE" && keywordcheck(tokenused.getName()) == Identifier) {
            tree.insertSibling(new Node(tokenused));
            tokenused = nextToken();

            //invalid: last char of string is "\"
            if (tokenused.getName().back() == '\\' || tokenused.getType() != "STRING" || keywordcheck(tokenused.getName()) != Identifier)
                Errorstatement("String", tokenused);
            else
                tree.insertSibling(new Node(tokenused));
            tokenused = nextToken();

            if (tokenused.getType() == "DOUBLE_QUOTE" && keywordcheck(tokenused.getName()) == Identifier)
                tree.insertSibling(new Node(tokenused));
            else
                Errorstatement("String", tokenused);
            tokenused = nextToken();
        }
        else if (tokenused.getType()=="SINGLE_QUOTE" && keywordcheck(tokenused.getName()) == Identifier) {
            tree.insertSibling(new Node(tokenused));
            tokenused = nextToken();

            //invalid: last char of string is "\"
            if (tokenused.getName().back() == '\\' || tokenused.getType() != "STRING" || keywordcheck(tokenused.getName()) != Identifier)
                Errorstatement("String", tokenused);
            else
                tree.insertSibling(new Node(tokenused));
            tokenused = nextToken();

            if (tokenused.getType() == "SINGLE_QUOTE" && keywordcheck(tokenused.getName()) == Identifier)
                tree.insertSibling(new Node(tokenused));
            else
                Errorstatement("String", tokenused);
            tokenused = nextToken();
        }
        nextToken();
    }

    //also for while statements?
    void parseIfWhileStatement() {
        Token tokenused = peek();
        //case1: if, case2: while
        if (tokenused.getType() == "If" && keywordcheck(tokenused.getName()) == Conditional) {
            tree.insertSibling(new Node(tokenused));
            tokenused = nextToken();

            if (tokenused.getType() == "L_PAREN" && keywordcheck(tokenused.getName()) == Identifier)
                tree.insertSibling(new Node(tokenused));
            else
                Errorstatement("If", tokenused);
            tokenused = nextToken();

            //parseBoolean();

            if (tokenused.getType() == "R_PAREN" && keywordcheck(tokenused.getName()) == Identifier)
                tree.insertSibling(new Node(tokenused));
            else
                Errorstatement("If", tokenused);
            tokenused = nextToken();

            if (tokenused.getType() == "L_BRACE" && keywordcheck(tokenused.getName()) == Identifier)
                tree.insertSibling(new Node(tokenused));
            else
                Errorstatement("If", tokenused);
            tokenused = nextToken();

            //stuff inside the brace goes here
            //see parseBrace?

            if (tokenused.getType() == "R_BRACE" && keywordcheck(tokenused.getName()) == Identifier)
                tree.insertSibling(new Node(tokenused));
            else
                Errorstatement("If", tokenused);
            tokenused = nextToken();
        }
        else if (tokenused.getType() == "While" && keywordcheck(tokenused.getName()) == Conditional) {
            tree.insertSibling(new Node(tokenused));
            tokenused = nextToken();

            if (tokenused.getType() == "L_PAREN" && keywordcheck(tokenused.getName()) == Identifier)
                tree.insertSibling(new Node(tokenused));
            else
                Errorstatement("While", tokenused);
            tokenused = nextToken();

            //parseBoolean();

            if (tokenused.getType() == "R_PAREN" && keywordcheck(tokenused.getName()) == Identifier)
                tree.insertSibling(new Node(tokenused));
            else
                Errorstatement("While", tokenused);
            tokenused = nextToken();

            if (tokenused.getType() == "L_BRACE" && keywordcheck(tokenused.getName()) == Identifier)
                tree.insertSibling(new Node(tokenused));
            else
                Errorstatement("While", tokenused);
            tokenused = nextToken();

            //stuff inside the brace goes here
            //see parseBrace?

            if (tokenused.getType() == "R_BRACE" && keywordcheck(tokenused.getName()) == Identifier)
                tree.insertSibling(new Node(tokenused));
            else
                Errorstatement("While", tokenused);
            tokenused = nextToken();
        }


        nextToken();
    }

    void parseElseStatement() {
        Token tokenused = peek();
        if (tokenused.getType() == "Else" && keywordcheck(tokenused.getName()) == Conditional)
            tree.insertSibling(new Node(tokenused));
        else
            Errorstatement("Else", tokenused);
        tokenused = nextToken();

        if (tokenused.getType() == "L_BRACE" && keywordcheck(tokenused.getName()) == Identifier)
            tree.insertSibling(new Node(tokenused));
        else
            Errorstatement("Else", tokenused);
        tokenused = nextToken();

        //stuff inside the brace goes here
        //see parseBrace?

        if (tokenused.getType() == "R_BRACE" && keywordcheck(tokenused.getName()) == Identifier)
            tree.insertSibling(new Node(tokenused));
        else
            Errorstatement("Else", tokenused);
        tokenused = nextToken();

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



        // Everything else

        if (!match("L_BRACKET") || !match("R_BRACKET")) {
            tree.insertSibling(new Node(tokenused));
            tokenused = nextToken();
        }
        else {
            Errorstatement("Bracket",tokenused);
        }

        // Expecting ']'

        if (match("R_BRACKET")) {
            tree.insertChild(new Node(tokenused));
            tokenused = nextToken();
        }
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
            tree.insertSibling(new Node(tokenused));
            tokenused = nextToken();
        }
        else {
            Errorstatement("Brace", tokenused);
        }

        // Everything else
        if(!match("L_BRACE") || !match("R_BRACE")) {
            tree.insertSibling(new Node(tokenused));
            tokenused = nextToken();
        }
        else {
            Errorstatement("Brace", tokenused);
        }

        // Expecting '}'
        if (match("R_BRACE")) {
            braceCounter--; // decrement
            braceLocation.push_back(tokenused.getLine()); // Push to vector
            tree.insertChild(new Node(tokenused));
            tokenused = nextToken();
        }
            // Missing '}'
        else {
            Errorstatement("Brace", tokenused);
        }
        
        // Checks to see if braces are being used properly
        if (braceCounter % 2 != 0) {
            Errorstatement("Brace", tokenused);
        }
        nextToken();
    }


    // PA3: RDP - Semicolon function
    // Individual function soley designed for handling semicolons
    void parseSemicolon() {
        // Init token
        Token tokenused = peek();

        if (tokenused.getType() == ";" && match("SEMICOLON")) {
            tree.insertChild(new Node(tokenused));
            tokenused = nextToken();
        }
        else {
            Errorstatement("Semicolon", tokenused);
        }
        nextToken();

    }

    void Errorstatement(string fromwhere,Token tokenused)
    {
        cout << fromwhere <<" error on line: " << tokenused.getLine() << " Token name: "<<tokenused.getName() <<"Token type: "<<tokenused.getType() << endl;
        exit(-1);
    }


    bool isDeclarationStatement() {
        // A declaration begins with a type (int, float, etc.)
        return match("int") || match("float") || match("bool") || match("char") || match("string");
    }

    bool isAssignmentStatement() {
        // An assignment statement starts with an identifier, followed by an assignment operator
        int savedPos = current;  // Save the current position
        if (match("IDENTIFIER")) {
            if (match("ASSIGNMENT_OPERATOR")) {
                current = savedPos;  // Reset position after lookahead
                return true;
            }
        }
        current = savedPos;  // Reset position if no match
        return false;
    }


    bool isSelectionStatement() {
        // A selection statement starts with the 'if' keyword
        return match("IF");
    }


    bool isIterationStatement() {
        // An iteration statement starts with 'for' or 'while'
        return match("FOR") || match("WHILE");
    }

    bool isEnd() {
        return current >= tokens.size();
    }
};


// Main function
int main() {
    vector<Token> tokenlist;
    string fileName ="programming_assignment_3-test_file_";
    string tokenizefile="test_file";
    for(int i=1; i <11; i++)
    {
        ignoreComments(fileName+std::to_string(i)+".c", tokenizefile+std::to_string(i)+".c");
        tokenlist = Tokenize(tokenizefile+std::to_string(i)+".c");
        continue;
    }
    return 0;
}
