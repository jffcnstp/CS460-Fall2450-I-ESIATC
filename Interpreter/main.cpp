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
    

//    /* PA3: RDP individual functions */
//// PA3:RDP - Parenthesis function
//// Individual function soley designed for handling parenthesis
//    void parseParenthesis() {
//        // init token
//        Token tokenused = peek();
//
//        // Expecting a '('
//        if (tokenused.getType() == "(" && keywordcheck(tokenused.getName()) ==LParen) {
//            tree.insertSibling(new Node(tokenused));
//            tokenused = nextToken();
//        }
//        else {
//            Errorstatement("Parenthesis", tokenused);
//        }
//
//        // Everything in the middle of the parenthesis
//        // TODO: Add an and statement that checks the keyword
//        if (tokenused.getType() != "(" || tokenused.getName() != ")") {
//            tree.insertSibling(new Node(tokenused));
//            tokenused = nextToken();
//        }
//        else {
//            Errorstatement("Parenthesis", tokenused);
//        }
//
//        // Expecting ')'
//        if (tokenused.getType() == ")" && keywordcheck(tokenused.getName()) ==RParen) {
//            tree.insertChild(new Node(tokenused));
//            tokenused = nextToken();
//        }
//        else {
//            Errorstatement("Parenthesis",tokenused);
//        }
//
//        nextToken();
//    }


    // PA3: RDP - Bracket function
    // Individual function soley designed for handling brackets
void parseBracket() {
        // Add some kind of node
        Token tokenused = peek();

        // Expecting a '['
        if (tokenused.getType() == "[" && keywordcheck(tokenused.getName()) == LBracket) {
            tree.insertSibling(new Node(tokenused));
            tokenused = nextToken();
        }
        else {
            Errorstatement("Bracket", tokenused);
        }

        // Everything else
        // TODO: Add an and statement that checks the keyword
        if (tokenused.getType() != "[" || tokenused.getType() != "]") {
            tree.insertSibling(new Node(tokenused));
            tokenused = nextToken();
        }
        else {
            Errorstatement("Bracket",tokenused);
        }

        // Expecting ']'
        if (tokenused.getType() == "]" && keywordcheck(tokenused.getName()) == RBracket) {
            tree.insertChild(new Node(tokenused));
            tokenused = nextToken();
        }
            // Missing ']'
        else {
            Errorstatement("Bracket", tokenused);
        }
    }

        // PA3: RDP - Brace function
    // Individual function soley designed for handling braces
    void parseBrace(int & counter,vector<int> &location) {
        // Add some kind of node
        Token tokenused = peek();

        // Expecting a '{'
        if (match("L_BRACE")) {
            tree.insertChild(new Node(tokenused));
            counter+=1;
            location.push_back(tokenused.getLine());
            tokenused = nextToken();
        }
        if(match("R_BRACE"))
        {
            tree.insertChild(new Node(tokenused));
            counter-=1;
            if(counter<0)
            {
                Errorstatement("invalid R_BRACE",tokenused);
            }
            else {
                location.pop_back();
                tokenused = nextToken();
            }
        }





    }


    // PA3: RDP - Semicolon function
    // Individual function soley designed for handling semicolons
    void parseSemicolon() {
        // Init token
        Token tokenused = peek();
        
        if (tokenused.getType() == ";" && keywordcheck(tokenused.getName() == ";")) {
            tree.insertChild(new Node(tokenused));
            tokenused = nextToken();
        }
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
