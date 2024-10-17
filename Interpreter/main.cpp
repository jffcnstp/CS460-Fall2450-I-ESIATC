#include <iostream>
#include <queue>
#include <string>
#include <vector>
#include <fstream>
#include "CommentRemove.h"

using namespace std;

// Node of the LCRS Tree
struct Node {
    string data;
    Node* leftChild; 
    Node* rightSibling;

    Node(string data) : data(data), leftChild(nullptr), rightSibling(nullptr) {}
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
    void insertChild(Node* parent, Node* child) {
        if (!parent->leftChild) {
            parent->leftChild = child;
        } else {
            Node* temp = parent->leftChild;
            while (temp->rightSibling) {
                temp = temp->rightSibling;
            }
            temp->rightSibling = child;
        }
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
            cout << currentNode->data << " ";

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
struct Token {
    string type;
    string name;
    int line;
    Token(string typeinput, string nameinput,int linenumber) : type(typeinput), name(nameinput), line(linenumber) {}

    string getType() { return type; }
    string getName() { return name; }
};

// Tokenizer function
vector<Token> Tokenize(const string& fileName) {
    ifstream inputFile(fileName); // Filecheck
    if (!inputFile) {
        cerr << "Error opening input file: " << fileName << endl;
        exit(1);
    }

    int linenumber = 1;
    enum TokenType{Identifier,Integer,Equal,LParen,RParen,LBracket,RBracket,LBrace,RBrace,DblQuote,SglQuote,Semicolon,Assignment,BoolEQ,BoolOR,BoolAND,};
    char currentChar;
    vector<Token> tokenlist;
    char casetype;

    while (inputFile.get(currentChar)) {
        if (currentChar == '\n') linenumber += 1;
        if (isalpha(currentChar))
            casetype = 'a';
        else if (isdigit(currentChar))
            casetype = '1';
        else
            casetype = currentChar;

        switch (casetype) {
            case 'a': {
                string idenname = "";
                while (isalnum(currentChar) || currentChar == '_') {
                    idenname += currentChar;
                    inputFile.get(currentChar);
                }

                // Check for reserved keywords
                if (idenname == "function") {
                    tokenlist.emplace_back("FUNCTION", "function",linenumber);
                } else if (idenname == "procedure") {
                    tokenlist.emplace_back("PROCEDURE", "procedure",linenumber);
                } else if (idenname == "void") {
                    tokenlist.emplace_back("VOID", "void",linenumber);
                } else if (idenname == "return") {
                    tokenlist.emplace_back("RETURN", "return",linenumber);
                } else if (idenname == "true") {
                    tokenlist.emplace_back("TRUE", "true",linenumber);
                } else if (idenname == "false") {
                    tokenlist.emplace_back("FALSE", "false",linenumber);
                } else if (idenname == "if") {
                    tokenlist.emplace_back("IF", "if",linenumber);
                } else if (idenname == "else") {
                    tokenlist.emplace_back("ELSE", "else",linenumber);
                } else if (idenname == "for") {
                    tokenlist.emplace_back("FOR", "for",linenumber);
                } else if (idenname == "while") {
                    tokenlist.emplace_back("WHILE", "while",linenumber);
                } else {
                    tokenlist.emplace_back("IDENTIFIER", idenname,linenumber);
                }

                inputFile.unget();
                break;
            }
            case '1': {
                string intname = "";
                while (isdigit(currentChar)) {
                    intname += currentChar;
                    inputFile.get(currentChar);
                }
                if (isalpha(currentChar)) {
                    cout << "Syntax error on line " << linenumber << ": invalid integer";
                    inputFile.close();
                    exit(3);
                } else {
                    tokenlist.emplace_back("INTEGER", intname,linenumber);
                    inputFile.unget();
                }
                break;
            }

            case '=':
                inputFile.get(currentChar);
                if (currentChar == '=') {
                    tokenlist.emplace_back("BOOLEAN_EQUAL", "==",linenumber);
                } else {
                    inputFile.unget();
                    tokenlist.emplace_back("ASSIGNMENT_OPERATOR", "=",linenumber);
                }
                break;
            case '(':
                tokenlist.emplace_back("L_PAREN", "(",linenumber);
                break;
            case ')':
                tokenlist.emplace_back("R_PAREN", ")",linenumber);
                break;
            case '{':
                {tokenlist.emplace_back("L_BRACE","{",linenumber); break;}
            case '}':
                {tokenlist.emplace_back("R_BRACE","}",linenumber); break;}
            case '[':
                {tokenlist.emplace_back("L_BRACKET","[",linenumber); break;}
            case ']':
                {tokenlist.emplace_back("R_BRACKET","]",linenumber); break;}
            case '"':
                {
                    std::string strname="";
                    tokenlist.emplace_back("DOUBLE_QUOTE","\"",linenumber);
                    inputFile.get(currentChar);
                    while(currentChar != '"')
                    {
                        strname+=currentChar;
                        inputFile.get(currentChar);
                    }
                    tokenlist.emplace_back("STRING",strname,linenumber);
                    tokenlist.emplace_back("DOUBLE_QUOTE","\"",linenumber);
                    break;
                }
            case '\'':
                {
                    std::string strname="";
                    tokenlist.emplace_back("SINGLE_QUOTE","'",linenumber);
                    inputFile.get(currentChar);
                    while(currentChar != '\'')
                    {
                        strname+=currentChar;
                        inputFile.get(currentChar);
                    }
                    tokenlist.emplace_back("STRING",strname,linenumber);
                    tokenlist.emplace_back("SINGLE_QUOTE","'",linenumber);
                    break;
                }
            case ';':
                {tokenlist.emplace_back("SEMICOLON",";",linenumber); break;}
            case ',':
                {tokenlist.emplace_back("COMMA",",",linenumber); break;}
            case '+':
                {tokenlist.emplace_back("PLUS","+",linenumber); break;}
            case '-':
                {tokenlist.emplace_back("MINUS","-",linenumber); break;}
            case '*':
                {
                    tokenlist.emplace_back("ASTERISK","*",linenumber);
                    break;
                }
            case '/':
                {tokenlist.emplace_back("DIVIDE","/",linenumber); break;}
            case '%':
                {tokenlist.emplace_back("MODULO","%",linenumber); break;}
            case '^':
                {tokenlist.emplace_back("CARET","^",linenumber); break;}
            case '<':
                {
                    inputFile.get(currentChar);
                    if(currentChar == '=')
                        tokenlist.emplace_back("LT_EQUAL","<=",linenumber);
                    else
                    {
                        tokenlist.emplace_back("LT","<",linenumber);
                        inputFile.unget();
                    }
                    break;
                }//editing all of these needs =
            case '>':
                 {
                    inputFile.get(currentChar);
                    if(currentChar == '=')
                        tokenlist.emplace_back("GT_EQUAL",">=",linenumber);
                    else
                    {
                        tokenlist.emplace_back("GT",">",linenumber);
                        inputFile.unget();
                    }
                    break;
                }
            case '&':
                {
                    inputFile.get(currentChar);
                    if(currentChar == '&')
                        tokenlist.emplace_back("BOOLEAN_AND","&&",linenumber);
                    else
                    {
                        inputFile.close();
                        std::cout<<"syntax error at line "<<linenumber<<": incomplete BOOLEAN_AND"<<std::endl;
                        exit(4);
                    }
                    break;
                }
            case '|':
                {
                    inputFile.get(currentChar);
                    if(currentChar == '|')
                        tokenlist.emplace_back("BOOLEAN_OR","||",linenumber);
                    else
                    {
                        inputFile.close();
                        std::cout<<"syntax error at line "<<linenumber<<": incomplete BOOLEAN_OR"<<std::endl;
                        exit(4);
                    }
                    break;
                }
            case '!':
            {
                inputFile.get(currentChar);
                if(currentChar == '=')
                    tokenlist.emplace_back("BOOLEAN_NOT_EQUAL","!=",linenumber);
                else
                {
                    tokenlist.emplace_back("BOOLEAN_NOT",",",linenumber);
                    inputFile.unget();
                }
            break;
            }
            
            default:
                break;
        }
    }
    return tokenlist;
}


class Parser {
    public:
    std::vector<Token> tokens;
    int current = 0;
    LCRSTree tree;

    Parser(const std::vector<Token>& tokenList) : tokens(tokenList), current(0), tree() {}

    Token& peek() {
        return tokens[current];
    }

    Token& consume() {
        return tokens[current++];
    }

    void error(const std::string& message) {
        std::cerr << "Error: " << message << " at token index " << current << std::endl;
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
            consume();
            return true;
        }
        return false;
    }

    // Helper method to add nodes to the tree
    Node* addNode(const std::string& data) {
        Node* node = new Node(data);
        tree.insertChild(tree.getCurrentNode(), node);
        return node;
    }

    // <NUMERICAL_EXPRESSION> ::= <NUMERICAL_OPERAND> | <L_PAREN> <NUMERICAL_OPERAND> <R_PAREN> ...
    Node* parseNumericalExpression() {
        Node* node = addNode("NumericalExpression");
        
        if (match("L_PAREN")) {
            addNode("(");
            parseNumericalExpression();
            match("R_PAREN");
            addNode(")");
        } else {
            parseNumericalOperand();
        }

        if (match("PLUS") || match("MINUS") || match("ASTERISK") || match("DIVIDE") || match("MODULO") || match("CARET")) {
            addNode("Operator");
            parseNumericalExpression();
        }

        return node;
    }

    // <NUMERICAL_OPERAND> ::= <IDENTIFIER> | <INTEGER> | <GETCHAR_FUNCTION> | <USER_DEFINED_FUNCTION>
    Node* parseNumericalOperand() {
        Node* node = addNode("NumericalOperand");
        if (match("IDENTIFIER")) {
            addNode("Identifier");
        } else if (match("INTEGER")) {
            addNode("Integer");
        } else {
            // Add support for other operand types like getchar function or user-defined functions.
            // We'll need separate methods for these, which we can invoke here.
        }
        return node;
    }

    // <BOOLEAN_EXPRESSION> ::= <BOOLEAN_TRUE> | <BOOLEAN_FALSE> | <IDENTIFIER> ...
    Node* parseBooleanExpression() {
        Node* node = addNode("BooleanExpression");
        
        if (match("TRUE")) {
            addNode("true");
        } else if (match("FALSE")) {
            addNode("false");
        } else if (match("IDENTIFIER")) {
            addNode("Identifier");
        } else {
            // Parse numerical expressions in boolean comparisons
            parseNumericalExpression();
            if (match("BOOLEAN_EQUAL") || match("BOOLEAN_NOT_EQUAL") || match("LT") || match("GT") || match("LT_EQUAL") || match("GT_EQUAL")) {
                addNode("RelationalOperator");
                parseNumericalExpression();
            }
        }

        if (match("BOOLEAN_AND") || match("BOOLEAN_OR")) {
            addNode("BooleanOperator");
            parseBooleanExpression();
        }

        return node;
    }

    // <EXPRESSION> ::= <BOOLEAN_EXPRESSION> | <NUMERICAL_EXPRESSION>
    Node* parseExpression() {
        Node* node = addNode("Expression");
        if (isBooleanExpression()) {
            parseBooleanExpression();
        } else {
            parseNumericalExpression();
        }
        return node;
    }

    // <SELECTION_STATEMENT> ::= if <L_PAREN> <BOOLEAN_EXPRESSION> <R_PAREN> <STATEMENT> ...
    Node* parseSelectionStatement() {
        Node* node = addNode("SelectionStatement");
        match("IF");
        addNode("if");
        match("L_PAREN");
        parseBooleanExpression();
        match("R_PAREN");
        parseStatement();

        if (match("ELSE")) {
            addNode("else");
            parseStatement();
        }
        return node;
    }

    // <ITERATION_STATEMENT> ::= for <L_PAREN> <INITIALIZATION_EXPRESSION> <SEMICOLON> ...
    Node* parseIterationStatement() {
        Node* node = addNode("IterationStatement");
        match("FOR");
        addNode("for");
        match("L_PAREN");
        parseInitializationExpression();
        match("SEMICOLON");
        parseBooleanExpression();
        match("SEMICOLON");
        parseExpression();
        match("R_PAREN");
        parseStatement();
        return node;
    }

    // <INITIALIZATION_EXPRESSION> ::= <IDENTIFIER> <ASSIGNMENT_OPERATOR> <EXPRESSION> ...
    Node* parseInitializationExpression() {
        Node* node = addNode("InitializationExpression");
        match("IDENTIFIER");
        addNode("Identifier");
        match("ASSIGNMENT_OPERATOR");
        addNode("=");
        parseExpression();
        return node;
    }

    Node* parseDeclarationStatement() {
        Node* node = addNode("DeclarationStatement");

        // Match and add the type (e.g., int, float)
        if (match("int")) {
            addNode("int");
        } else if (match("float")) {
            addNode("float");
        } else if (match("bool")) {
            addNode("bool");
        } else if (match("char")) {
            addNode("char");
        } else if (match("string")) {
            addNode("string");
        }

        // Match and add the identifier (variable name)
        if (match("IDENTIFIER")) {
            addNode("Identifier");
        } else {
            std::cerr << "Syntax error: Expected an identifier after type." << std::endl;
            exit(1);
        }

        // Check if there's an assignment operator (=) for initialization
        if (match("ASSIGNMENT_OPERATOR")) {
            addNode("=");

            // Parse the expression on the right-hand side of the assignment
            parseExpression();
        }

        // Match the semicolon at the end of the statement
        if (!match("SEMICOLON")) {
            std::cerr << "Syntax error: Expected ';' at the end of the declaration statement." << std::endl;
            exit(1);
        }
        
        return node;
    }

    Node* parseAssignmentStatement() {
        Node* node = addNode("AssignmentStatement");

        // Match and add the identifier (variable name)
        if (match("IDENTIFIER")) {
            addNode("Identifier");
        } else {
            std::cerr << "Syntax error: Expected an identifier in assignment statement." << std::endl;
            exit(1);
        }

        // Match and add the assignment operator (=)
        if (match("ASSIGNMENT_OPERATOR")) {
            addNode("=");
        } else {
            std::cerr << "Syntax error: Expected '=' in assignment statement." << std::endl;
            exit(1);
        }

        // Parse the expression on the right-hand side of the assignment
        parseExpression();

        // Match the semicolon at the end of the statement
        if (!match("SEMICOLON")) {
            std::cerr << "Syntax error: Expected ';' at the end of the assignment statement." << std::endl;
            exit(1);
        }
        
        return node;
    }



    // <STATEMENT> ::= <DECLARATION_STATEMENT> | <ASSIGNMENT_STATEMENT> ...
    Node* parseStatement() {
        Node* node = addNode("Statement");
        if (isDeclarationStatement()) {
            parseDeclarationStatement();
        } else if (isAssignmentStatement()) {
            parseAssignmentStatement();
        } else if (isIterationStatement()) {
            parseIterationStatement();
        } else if (isSelectionStatement()) {
            parseSelectionStatement();
        }
        return node;
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


    // Entry point to parse the program
    Node* parseProgram() {
        Node* root = addNode("Program");
        while (!isEnd()) {
            parseStatement();
        }
        return root;
    }

    bool isEnd() {
        return current >= tokens.size();
    }
};


// Main function
int main() {
    std::vector<Token> tokenlist;
    string fileName ="programming_assignment_3-test_file_1.c";
    ignoreComments(fileName,"test_file3.c");
    tokenlist=Tokenize("test_file3.c");

    return 0;
}
