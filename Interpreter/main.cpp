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

enum keywords{Identifier=0,Type=1,Conditional=2};
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


// Token struct



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
        if (peek().type == type) {
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
        return Identifier;
    }

    //this function is called when the token is found to be a function
    void parseFunctionDeclaration()
    {
        Token tokenused=peek();
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
        
        if(tokenused.getType()=="IDENTIFIER" && keywordcheck(tokenused.getName()) ==Type)
            tree.insertSibling(new Node(tokenused));
        else
            Errorstatement("FunctionDeclaration",tokenused);
        tokenused=nextToken();

        if(tokenused.getType()=="IDENTIFIER" && keywordcheck(tokenused.getName()) ==Identifier)
            tree.insertSibling(new Node(tokenused));
        else
            Errorstatement("FunctionDeclaration",tokenused);
        tokenused=nextToken();

        if(tokenused.getType()=="R_PAREN")
            tree.insertSibling(new Node(tokenused));
        else
            Errorstatement("FunctionDeclaration",tokenused);

        nextToken();
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
    
=======
    /* PA3: RDP individual functions */
// PA3:RDP - Parenthesis function
// Individual function soley designed for handling parenthesis
    void parseParenthesis() {
        // init token
        Token tokenused = peek();

        // Expecting a '('
        if (tokenused.getType() == "(" && keywordcheck(tokenused.getName()) ==LParen) {
            tree.insertSibling(new Node(tokenused));
            tokenused = nextToken();
        }
        else {
            Errorstatement("Parenthesis", tokenused);
        }

        // Everything in the middle of the parenthesis
        if (tokenused.getType() != "(" || tokenused.getName() != ")") {
            tree.insertSibling(new Node(tokenused));
            tokenused = nextToken();
        }
        else {
            Errorstatement("Parenthesis", tokenused);
        }

        // Expecting ')'
        if (tokenused.getType() == ")" && keywordcheck(tokenused.getName()) ==RParen) {
            tree.insertChild(new Node(tokenused));
            tokenused = nextToken();
        }
        else {
            Errorstatement("Parenthesis",tokenused);
        }

        nextToken();
    }


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

        //TODO: Account for both negative integer error and numerical calculation
        // Negative Integer error
        if(match("MINUS")) {
            Errorstatement("Bracket",tokenused);
        }

        // Numerical Expression case
        if(match("INTEGER")) {
            parseNumerical();
            tokenused = nextToken();
        }
        else {
            Errorstatement("Bracket",tokenused);
        }

        // Everything else
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
    nextToken();
    }

        // PA3: RDP - Brace function
    // Individual function soley designed for handling braces
    void parseBrace() {
        // Add some kind of node
        Token tokenused = peek();

        // Expecting a '{'
        if (tokenused.getType() == "{" && keywordcheck(tokenused.getName()) == LBrace) {
            tree.insertSibling(new Node(tokenused));
            tokenused = nextToken();
        }
        else {
            Errorstatement("Brace", tokenused);
        }

        // Everything else
        if(tokenused.getType() != "{" || tokenused.getType() != "}") {
            tree.insertSibling(new Node(tokenused));
            tokenused = nextToken();
        }
        else {
            Errorstatement("Brace", tokenused);
        }

        // Expecting '}'
        if (tokenused.getType() == "}" && keywordcheck(tokenused.getName() == RBrace)) {
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
        Token tokenused = peek();
        
        if (tokenused.getType() == ";" && keywordcheck(tokenused.getName() == Semicolon)) {
            tree.insertChild(new Node(tokenused));
            tokenused = nextToken();
        }
        nextToken();
    }


    void Errorstatement(string fromwhere,Token tokenused)
    {
        cout << fromwhere <<" error on line: " << tokenused.getLine() << " Token name: "<<tokenused.getName() <<"Token type: "<<tokenused.getType() << endl;
        exit(-1);
    }


    // Helper functions to identify different statements or expressions
    bool isBooleanExpression() {
        // Logic to check if it's a boolean expression
        return match("TRUE") || match("FALSE") || match("IDENTIFIER");
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
