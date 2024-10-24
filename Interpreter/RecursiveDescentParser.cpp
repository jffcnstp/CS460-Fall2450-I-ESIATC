#include "RecursiveDescentParser.h"

std::string typekeyword[] = { "int", "bool", "char", "void" };
std::string conditionalkeyword[] = { "if", "for", "while", "else" };
vector<string> operandlist = { "PLUS", "MINUS", "ASTERISK", "DIVIDE", "MODULO", "LT_EQUAL", "LT", "GT_EQUAL", "GT", "BOOLEAN_AND", "BOOLEAN_OR", "BOOLEAN_NOT_EQUAL", "BOOLEAN_EQUAL" };

// Node implementation
Node::Node(Token token) : data(token), leftChild(nullptr), rightSibling(nullptr) {}

// LCRSTree implementation
LCRSTree::LCRSTree() : root(nullptr), currentNode(nullptr) {}

LCRSTree::~LCRSTree() { deleteNode(root); }

void LCRSTree::deleteNode(Node* node) {
    if (!node) return;
    deleteNode(node->leftChild);
    deleteNode(node->rightSibling);
    delete node;
}

void LCRSTree::insertChild(Node* child) {
    if (root == nullptr) {
        root = child;
        currentNode = root;
        return;
    }
    currentNode->leftChild = child;
    currentNode = child;
}

void LCRSTree::insertSibling(Node* sibling) {
    if (root == nullptr) {
        root = sibling;
        currentNode = root;
        return;
    }
    currentNode->rightSibling = sibling;
    currentNode = sibling;
}

Node* LCRSTree::getCurrentNode() {
    return currentNode;
}

void LCRSTree::breadthFirstTraversal() {
    if (!root) return;
    Node* traversal = root;
    while (traversal) {
        cout << traversal->data.getName() << " ";
        if (traversal->rightSibling)
            traversal = traversal->rightSibling;
        else if (traversal->leftChild) {
            cout << endl;
            traversal = traversal->leftChild;
        } else
            break;
    }
}

// Parser implementation
Parser::Parser(const std::vector<Token>& tokenList) : tokens(tokenList), current(0), tree() {}

Token& Parser::peek() {
    return tokens[current];
}

Token& Parser::nextToken() {
    current++;
    if (current < tokens.size())
        return tokens[current];
    else
        return tokens[current - 1];
}

bool Parser::match(const string& type) {
    return peek().getType() == type;
}

int Parser::keywordcheck(string name) {
    for (auto &word : typekeyword) {
        if (name == word)
            return Type;
    }
    for (auto &word : conditionalkeyword) {
        if (name == word)
            return Conditional;
    }
    if (name == "return")
        return RETURN;
    if (name == "printf")
        return Function;
    return Identifier;
}

void Parser::buildCST() {
    int braceCounter = 0;
    vector<int> braceLocation;
    while (current < tokens.size()) {
        if (match("FUNCTION"))
            parseFunctionDeclaration();
        else if (match("IDENTIFIER")) {
            int keyword = keywordcheck(peek().getName());
            switch (keyword) {
                case Type:
                    parseVariableDeclaration();
                    break;
                case Identifier:
                case Function:
                    parseVariableOperation();
                    break;
                case RETURN:
                    parseReturn();
                    break;
            }
        } else if (match("PROCEDURE")) {
            parseProcedure();
        } else if (match("IF") || match("WHILE")) {
            parseIfWhileStatement();
        } else if (match("ELSE")) {
            parseElseStatement();
        } else if (match("FOR")) {
            parseForStatement();
        }
        if (match("SEMICOLON"))
            parseSemicolon();
        if (match("L_BRACE") || match("R_BRACE"))
            parseBrace(braceCounter, braceLocation);
    }
    if (braceCounter > 0) {
        cout << "Missing R_BRACE for " << braceCounter << " L_BRACE at lines: ";
        for (auto &number : braceLocation)
            cout << number << " ";
        exit(-1);
    }
}

void Parser::parseProcedure() {
    tree.insertChild(new Node(peek()));
    nextToken();

    if (match("IDENTIFIER") && keywordcheck(peek().getName()) == Identifier) {
        tree.insertSibling(new Node(peek()));
        nextToken();
    } else {
        Errorstatement("Procedure Identifier", peek());
    }

    if (match("L_PAREN")) {
        tree.insertSibling(new Node(peek()));
        nextToken();
    } else {
        Errorstatement("Procedure L_PAREN", peek());
    }

    parseFunctionDeclarationParameter();

    if (match("R_PAREN")) {
        tree.insertSibling(new Node(peek()));
        nextToken();
    } else {
        Errorstatement("Procedure R_PAREN", peek());
    }
}

void Parser::parseFunctionDeclaration() {
    bool multipleParameters = true;
    tree.insertChild(new Node(peek()));
    nextToken();

    if (match("IDENTIFIER") && keywordcheck(peek().getName()) == Type)
        tree.insertSibling(new Node(peek()));
    else
        Errorstatement("FunctionDeclaration Keyword", peek());
    nextToken();

    if (match("IDENTIFIER") && keywordcheck(peek().getName()) == Identifier)
        tree.insertSibling(new Node(peek()));
    else
        Errorstatement("FunctionDeclaration Identifier", peek());
    nextToken();

    if (match("L_PAREN"))
        tree.insertSibling(new Node(peek()));
    else
        Errorstatement("FunctionDeclaration L_PAREN", peek());
    nextToken();

    while (multipleParameters) {
        multipleParameters = false;
        parseFunctionDeclarationParameter();
        if (match("COMMA")) {
            tree.insertSibling(new Node(peek()));
            nextToken();
            multipleParameters = true;
        }
    }
    if (match("R_PAREN"))
        tree.insertSibling(new Node(peek()));
    else
        Errorstatement("FunctionDeclaration R_PAREN", peek());
    nextToken();
}

void Parser::parseFunctionDeclarationParameter() {
    Token tokenUsed = peek();
    if (match("IDENTIFIER") && keywordcheck(tokenUsed.getName()) == Type) {
        tree.insertSibling(new Node(tokenUsed));
        if (peek().getName() == "void") {
            nextToken();
            return;
        }
        tokenUsed = nextToken();
    } else {
        Errorstatement("FunctionDeclarationParameter Keyword", tokenUsed);
    }

    if (match("IDENTIFIER") && keywordcheck(tokenUsed.getName()) == Identifier) {
        tree.insertSibling(new Node(tokenUsed));
        tokenUsed = nextToken();
    } else {
        Errorstatement("FunctionDeclarationParameter Identifier", tokenUsed);
    }

    if (match("L_BRACKET")) {
        parseBracket();
    }

    if (match("ASSIGNMENT_OPERATOR")) {
        parseExpression();
    }
}

void Parser::parseVariableDeclaration() {
    bool loop = true;
    tree.insertChild(new Node(peek()));
    nextToken();

    while (loop) {
        loop = false;

        if (match("IDENTIFIER") && keywordcheck(peek().getName()) == Identifier) {
            tree.insertSibling(new Node(peek()));
            nextToken();
        } else {
            Errorstatement("VariableDeclaration IDENTIFIER", peek());
        }

        if (match("L_BRACKET")) {
            parseBracket();
        }
        if (match("ASSIGNMENT_OPERATOR")) {
            parseExpression();
        }
        if (match("COMMA")) {
            loop = true;
            tree.insertSibling(new Node(peek()));
            nextToken();
        }
    }
}

void Parser::parseVariableOperation() {
    tree.insertChild(new Node(peek()));
    nextToken();

    if (match("L_BRACKET")) {
        parseBracket();
    }
    if (match("ASSIGNMENT_OPERATOR")) {
        tree.insertSibling(new Node(peek()));
        nextToken();
        if (match("DOUBLE_QUOTE") || match("SINGLE_QUOTE"))
            parseString();
        else
            parseExpression();
    } else if (match("L_PAREN")) {
        tree.insertSibling(new Node(peek()));
        nextToken();
        parseFunctionCallParameters();

        if (match("R_PAREN")) {
            tree.insertSibling(new Node(peek()));
            nextToken();
        } else {
            Errorstatement("VariableOperation R_PAREN", peek());
        }
    }
    if (!match("SEMICOLON")) {
        Errorstatement("VariableOperation SEMICOLON", peek());
    }
}

void Parser::parseFunctionCallParameters() {
    bool loop = true;
    while (loop) {
        loop = false;
        parseExpression();

        if (match("COMMA")) {
            loop = true;
            tree.insertSibling(new Node(peek()));
            nextToken();
        }
    }
    if (!match("R_PAREN")) {
        Errorstatement("FunctionCallParameter R_PAREN", peek());
    }
}

void Parser::parseReturn() {
    tree.insertChild(new Node(peek()));
    nextToken();

    if (match("L_PAREN")) {
        tree.insertSibling(new Node(peek()));
        nextToken();

        parseExpression();

        if (match("R_PAREN")) {
            tree.insertSibling(new Node(peek()));
            nextToken();
        } else {
            Errorstatement("Return R_PAREN", peek());
        }
    } else if (match("IDENTIFIER") && keywordcheck(peek().getName()) == Identifier) {
        tree.insertSibling(new Node(peek()));
        nextToken();
    }
    if (!match("SEMICOLON")) {
        Errorstatement("Return SEMICOLON", peek());
    }
}

void Parser::parseString() {
    Token tokenUsed = peek();
    if (match("DOUBLE_QUOTE")) {
        tree.insertSibling(new Node(tokenUsed));
        tokenUsed = nextToken();

        if (!match("STRING") || tokenUsed.getName().back() == '\\') {
            Errorstatement("String IllegalString", tokenUsed);
        } else {
            tree.insertSibling(new Node(tokenUsed));
            tokenUsed = nextToken();
        }

        if (match("DOUBLE_QUOTE")) {
            tree.insertSibling(new Node(tokenUsed));
            tokenUsed = nextToken();
        } else {
            Errorstatement("String DoubleQuote", tokenUsed);
        }
    } else if (match("SINGLE_QUOTE")) {
        tree.insertSibling(new Node(tokenUsed));
        tokenUsed = nextToken();

        if (!match("STRING") || tokenUsed.getName().back() == '\\') {
            Errorstatement("String IllegalString", tokenUsed);
        } else {
            tree.insertSibling(new Node(tokenUsed));
            tokenUsed = nextToken();
        }

        if (match("SINGLE_QUOTE")) {
            tree.insertSibling(new Node(tokenUsed));
            tokenUsed = nextToken();
        } else {
            Errorstatement("String SingleQuote", tokenUsed);
        }
    }
}

void Parser::parseIfWhileStatement() {
    if (match("IF") && keywordcheck(peek().getName()) == Conditional) {
        tree.insertChild(new Node(peek()));
        nextToken();

        if (match("L_PAREN")) {
            tree.insertSibling(new Node(peek()));
        } else {
            Errorstatement("If L_PAREN", peek());
        }
        nextToken();

        parseExpression();

        if (match("R_PAREN")) {
            tree.insertSibling(new Node(peek()));
        } else {
            Errorstatement("If R_PAREN", peek());
        }
        nextToken();
    } else if (match("WHILE") && keywordcheck(peek().getName()) == Conditional) {
        tree.insertSibling(new Node(peek()));
        nextToken();

        if (match("L_PAREN")) {
            tree.insertSibling(new Node(peek()));
        } else {
            Errorstatement("While L_PAREN", peek());
        }
        nextToken();

        parseExpression();

        if (match("R_PAREN")) {
            tree.insertSibling(new Node(peek()));
        } else {
            Errorstatement("While R_PAREN", peek());
        }
        nextToken();
    }
}

void Parser::parseElseStatement() {
    if (match("ELSE") && keywordcheck(peek().getName()) == Conditional) {
        tree.insertChild(new Node(peek()));
    } else {
        Errorstatement("Else ELSE", peek());
    }
    nextToken();
}

void Parser::parseForStatement() {
    Token tokenUsed = peek();
    if (tokenUsed.getType() == "FOR" && keywordcheck(tokenUsed.getName()) == Conditional) {
        tree.insertChild(new Node(tokenUsed));
    } else {
        Errorstatement("For FOR", tokenUsed);
    }
    tokenUsed = nextToken();

    if (tokenUsed.getType() == "L_PAREN" && keywordcheck(tokenUsed.getName()) == Identifier) {
        tree.insertSibling(new Node(tokenUsed));
    } else {
        Errorstatement("For L_PAREN", tokenUsed);
    }
    tokenUsed = nextToken();

    if (tokenUsed.getType() == "IDENTIFIER" && keywordcheck(tokenUsed.getName()) == Identifier) {
        tree.insertSibling(new Node(tokenUsed));
    } else {
        Errorstatement("For statement1 operand1", tokenUsed);
    }
    tokenUsed = nextToken();

    if (tokenUsed.getType() == "ASSIGNMENT_OPERATOR" && keywordcheck(tokenUsed.getName()) == Identifier) {
        tree.insertSibling(new Node(tokenUsed));
    } else {
        Errorstatement("For statement1 assignment", tokenUsed);
    }
    tokenUsed = nextToken();

    if (tokenUsed.getType() == "INTEGER" && keywordcheck(tokenUsed.getName()) == Identifier) {
        tree.insertSibling(new Node(tokenUsed));
    } else {
        Errorstatement("For statement1 operand2", tokenUsed);
    }
    tokenUsed = nextToken();

    parseSemicolon();

    parseExpression();

    parseSemicolon();

    tokenUsed = peek();
    if (tokenUsed.getType() == "IDENTIFIER" && keywordcheck(tokenUsed.getName()) == Identifier) {
        tree.insertSibling(new Node(tokenUsed));
    } else {
        Errorstatement("For statement3 operand1", tokenUsed);
    }
    tokenUsed = nextToken();

    if (tokenUsed.getType() == "ASSIGNMENT_OPERATOR" && keywordcheck(tokenUsed.getName()) == Identifier) {
        tree.insertSibling(new Node(tokenUsed));
    } else {
        Errorstatement("For statement3 assignment", tokenUsed);
    }
    tokenUsed = nextToken();

    parseExpression();

    tokenUsed = peek();
    if (tokenUsed.getType() == "R_PAREN" && keywordcheck(tokenUsed.getName()) == Identifier) {
        tree.insertSibling(new Node(tokenUsed));
    } else {
        Errorstatement("For R_PAREN", tokenUsed);
    }
    tokenUsed = nextToken();
}

void Parser::parseBracket() {
    if (match("L_BRACKET")) {
        tree.insertSibling(new Node(peek()));
        nextToken();
    } else {
        Errorstatement("Bracket L_BRACKET", peek());
    }

    if (match("MINUS")) {
        Errorstatement("Bracket ILLEGALMINUS", peek());
    }
    if (match("PLUS")) {
        tree.insertSibling(new Node(peek()));
        nextToken();
    }

    parseExpression();

    if (match("R_BRACKET")) {
        tree.insertSibling(new Node(peek()));
    } else {
        Errorstatement("Bracket R_BRACKET", peek());
    }
    nextToken();
}

void Parser::parseBrace(int &braceCounter, vector<int> &braceLocation) {
    if (match("L_BRACE")) {
        braceCounter++;
        braceLocation.push_back(peek().getLine());
        tree.insertChild(new Node(peek()));
        nextToken();
    } else if (match("R_BRACE")) {
        braceCounter--;
        if (braceCounter < 0) {
            cout << "Illegal R_BRACE without accompanying L_BRACE at line: " << peek().getLine();
            exit(-1);
        }
        braceLocation.pop_back();
        tree.insertChild(new Node(peek()));
        nextToken();
    }
}

void Parser::parseSemicolon() {
    tree.insertSibling(new Node(peek()));
    nextToken();
}

void Parser::parseExpression() {
    bool loop = true;
    while (loop) {
        loop = false;
        if (match("DOUBLE_QUOTE") || match("SINGLE_QUOTE")) {
            parseString();
        } else if ((match("MINUS") && tokens[current + 1].getType() == "INTEGER") || match("INTEGER")) {
            if (match("MINUS")) {
                tree.insertSibling(new Node(peek()));
                nextToken();
            }
            tree.insertSibling(new Node(peek()));
            nextToken();
        } else if ((match("BOOLEAN_NOT") && tokens[current + 1].getType() == "IDENTIFIER" && keywordcheck(tokens[current + 1].getName()) == Identifier)
                   || (match("IDENTIFIER") && keywordcheck(peek().getName()) == Identifier) || match("TRUE") || match("FALSE")) {
            if (match("BOOLEAN_NOT")) {
                tree.insertSibling(new Node(peek()));
                nextToken();
            }

            tree.insertSibling(new Node(peek()));
            nextToken();

            if (match("L_BRACKET"))
                parseBracket();
            else if (match("L_PAREN")) {
                tree.insertSibling(new Node(peek()));
                nextToken();
                parseFunctionCallParameters();

                if (match("R_PAREN")) {
                    tree.insertSibling(new Node(peek()));
                    nextToken();
                } else {
                    Errorstatement("ExpressionParse R_PAREN", peek());
                }
            }
        } else if (match("L_PAREN")) {
            tree.insertSibling(new Node(peek()));
            nextToken();

            parseExpression();

            if (match("R_PAREN")) {
                tree.insertSibling(new Node(peek()));
                nextToken();
            } else {
                Errorstatement("ExpressionParse R_PAREN", peek());
            }

        } else {
            Errorstatement("ExpressionParse Operator", peek());
        }

        if (find(operandlist.begin(), operandlist.end(), peek().getType()) != operandlist.end()) {
            loop = true;
            tree.insertSibling(new Node(peek()));
            nextToken();
        }
    }
    if (!match("R_PAREN") && !match("SEMICOLON") && !match("R_BRACKET") && !match("COMMA")) {
        Errorstatement("ExpressionParse Operand", peek());
    }
}

void Parser::Errorstatement(string fromWhere, Token tokenUsed) {
    cout << fromWhere << " error on line: " << tokenUsed.getLine() << " Token name: " << tokenUsed.getName() << " Token type: " << tokenUsed.getType() << endl;
    exit(-1);
}
