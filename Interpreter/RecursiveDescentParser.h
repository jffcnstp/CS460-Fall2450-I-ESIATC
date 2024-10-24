#ifndef INTERPRETER_RECURSIVEDESCENTPARSER_H
#define INTERPRETER_RECURSIVEDESCENTPARSER_H

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "../CommentRemove.h"
#include "Token.h"
#include "RecursiveDescentParser.h"

using namespace std;

enum keywords { Identifier = 0, Type = 1, Conditional = 2, RETURN = 3, Function = 4 };

// Node of the LCRS Tree
struct Node {
    Token data;
    Node* leftChild;
    Node* rightSibling;

    Node(Token token);
};

// Implements the structure of the concrete syntax tree
class LCRSTree {
private:
    Node* currentNode;
    void deleteNode(Node* node);

public:
    Node* root;

    LCRSTree();
    ~LCRSTree();

    void insertChild(Node* child);
    void insertSibling(Node* sibling);
    Node* getCurrentNode();
    void breadthFirstTraversal();
};

class Parser {
public:
    std::vector<Token> tokens;
    int current;
    LCRSTree tree;

    Parser(const std::vector<Token>& tokenList);

    Token& peek();
    Token& nextToken();
    bool match(const string& type);
    int keywordcheck(string name);
    void buildCST();
    void parseProcedure();
    void parseFunctionDeclaration();
    void parseFunctionDeclarationParameter();
    void parseVariableDeclaration();
    void parseVariableOperation();
    void parseFunctionCallParameters();
    void parseReturn();
    void parseString();
    void parseIfWhileStatement();
    void parseElseStatement();
    void parseForStatement();
    void parseBracket();
    void parseBrace(int &braceCounter, vector<int> &braceLocation);
    void parseSemicolon();
    void parseExpression();
    void Errorstatement(string fromwhere, Token tokenused);
};

#endif // INTERPRETER_RECURSIVEDESCENTPARSER_H
