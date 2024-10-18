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
    
    void Errorstatement(string fromwhere,Token tokenused)
    {
        cout << fromwhere <<" error on line: " << tokenused.getLine() << " Token name: "<<tokenused.getName() <<"Token type: "<<tokenused.getType() << endl;
        exit(-1);
    }
    // <NUMERICAL_EXPRESSION> ::= <NUMERICAL_OPERAND> | <L_PAREN> <NUMERICAL_OPERAND> <R_PAREN> ...
//    Node* parseNumericalExpression() {
//        Node* node = addNode("NumericalExpression");
//
//        if (match("L_PAREN")) {
//            addNode("(");
//            parseNumericalExpression();
//            match("R_PAREN");
//            addNode(")");
//        } else {
//            parseNumericalOperand();
//        }
//
//        if (match("PLUS") || match("MINUS") || match("ASTERISK") || match("DIVIDE") || match("MODULO") || match("CARET")) {
//            addNode("Operator");
//            parseNumericalExpression();
//        }
//
//        return node;
//    }
//
//    // <NUMERICAL_OPERAND> ::= <IDENTIFIER> | <INTEGER> | <GETCHAR_FUNCTION> | <USER_DEFINED_FUNCTION>
//    Node* parseNumericalOperand() {
//        Node* node = addNode("NumericalOperand");
//        if (match("IDENTIFIER")) {
//            addNode("Identifier");
//        } else if (match("INTEGER")) {
//            addNode("Integer");
//        } else {
//            // Add support for other operand types like getchar function or user-defined functions.
//            // We'll need separate methods for these, which we can invoke here.
//        }
//        return node;
//    }
//
//    // <BOOLEAN_EXPRESSION> ::= <BOOLEAN_TRUE> | <BOOLEAN_FALSE> | <IDENTIFIER> ...
//    Node* parseBooleanExpression() {
//        Node* node = addNode("BooleanExpression");
//
//        if (match("TRUE")) {
//            addNode("true");
//        } else if (match("FALSE")) {
//            addNode("false");
//        } else if (match("IDENTIFIER")) {
//            addNode("Identifier");
//        } else {
//            // Parse numerical expressions in boolean comparisons
//            parseNumericalExpression();
//            if (match("BOOLEAN_EQUAL") || match("BOOLEAN_NOT_EQUAL") || match("LT") || match("GT") || match("LT_EQUAL") || match("GT_EQUAL")) {
//                addNode("RelationalOperator");
//                parseNumericalExpression();
//            }
//        }
//
//        if (match("BOOLEAN_AND") || match("BOOLEAN_OR")) {
//            addNode("BooleanOperator");
//            parseBooleanExpression();
//        }
//
//        return node;
//    }
//
//    // <EXPRESSION> ::= <BOOLEAN_EXPRESSION> | <NUMERICAL_EXPRESSION>
//    Node* parseExpression() {
//        Node* node = addNode("Expression");
//        if (isBooleanExpression()) {
//            parseBooleanExpression();
//        } else {
//            parseNumericalExpression();
//        }
//        return node;
//    }
//
//    // <SELECTION_STATEMENT> ::= if <L_PAREN> <BOOLEAN_EXPRESSION> <R_PAREN> <STATEMENT> ...
//    Node* parseSelectionStatement() {
//        Node* node = addNode("SelectionStatement");
//        match("IF");
//        addNode("if");
//        match("L_PAREN");
//        parseBooleanExpression();
//        match("R_PAREN");
//        parseStatement();
//
//        if (match("ELSE")) {
//            addNode("else");
//            parseStatement();
//        }
//        return node;
//    }
//
//    // <ITERATION_STATEMENT> ::= for <L_PAREN> <INITIALIZATION_EXPRESSION> <SEMICOLON> ...
//    Node* parseIterationStatement() {
//        Node* node = addNode("IterationStatement");
//        match("FOR");
//        addNode("for");
//        match("L_PAREN");
//        parseInitializationExpression();
//        match("SEMICOLON");
//        parseBooleanExpression();
//        match("SEMICOLON");
//        parseExpression();
//        match("R_PAREN");
//        parseStatement();
//        return node;
//    }
//
//    // <INITIALIZATION_EXPRESSION> ::= <IDENTIFIER> <ASSIGNMENT_OPERATOR> <EXPRESSION> ...
//    Node* parseInitializationExpression() {
//        Node* node = addNode("InitializationExpression");
//        match("IDENTIFIER");
//        addNode("Identifier");
//        match("ASSIGNMENT_OPERATOR");
//        addNode("=");
//        parseExpression();
//        return node;
//    }
//
//    Node* parseDeclarationStatement() {
//        Node* node = addNode("DeclarationStatement");
//
//        // Match and add the type (e.g., int, float)
//        if (match("int")) {
//            addNode("int");
//        } else if (match("float")) {
//            addNode("float");
//        } else if (match("bool")) {
//            addNode("bool");
//        } else if (match("char")) {
//            addNode("char");
//        } else if (match("string")) {
//            addNode("string");
//        }
//
//        // Match and add the identifier (variable name)
//        if (match("IDENTIFIER")) {
//            addNode("Identifier");
//        } else {
//            std::cerr << "Syntax error: Expected an identifier after type." << std::endl;
//            exit(1);
//        }
//
//        // Check if there's an assignment operator (=) for initialization
//        if (match("ASSIGNMENT_OPERATOR")) {
//            addNode("=");
//
//            // Parse the expression on the right-hand side of the assignment
//            parseExpression();
//        }
//
//        // Match the semicolon at the end of the statement
//        if (!match("SEMICOLON")) {
//            std::cerr << "Syntax error: Expected ';' at the end of the declaration statement." << std::endl;
//            exit(1);
//        }
//
//        return node;
//    }
//
//    Node* parseAssignmentStatement() {
//        Node* node = addNode("AssignmentStatement");
//
//        // Match and add the identifier (variable name)
//        if (match("IDENTIFIER")) {
//            addNode("Identifier");
//        } else {
//            std::cerr << "Syntax error: Expected an identifier in assignment statement." << std::endl;
//            exit(1);
//        }
//
//        // Match and add the assignment operator (=)
//        if (match("ASSIGNMENT_OPERATOR")) {
//            addNode("=");
//        } else {
//            std::cerr << "Syntax error: Expected '=' in assignment statement." << std::endl;
//            exit(1);
//        }
//
//        // Parse the expression on the right-hand side of the assignment
//        parseExpression();
//
//        // Match the semicolon at the end of the statement
//        if (!match("SEMICOLON")) {
//            std::cerr << "Syntax error: Expected ';' at the end of the assignment statement." << std::endl;
//            exit(1);
//        }
//
//        return node;
//    }
//
//
//
//    // <STATEMENT> ::= <DECLARATION_STATEMENT> | <ASSIGNMENT_STATEMENT> ...
//    Node* parseStatement() {
//        Node* node = addNode("Statement");
//        if (isDeclarationStatement()) {
//            parseDeclarationStatement();
//        } else if (isAssignmentStatement()) {
//            parseAssignmentStatement();
//        } else if (isIterationStatement()) {
//            parseIterationStatement();
//        } else if (isSelectionStatement()) {
//            parseSelectionStatement();
//        }
//        return node;
//    }

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


    // Entry point to parse the program
//    Node* parseProgram() {
//        Node* root = addNode("Program");
//        while (!isEnd()) {
//            parseStatement();
//        }
//        return root;
//    }

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
