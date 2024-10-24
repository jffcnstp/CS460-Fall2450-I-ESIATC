//
// Created by storm on 10/17/2024.
//

#ifndef INTERPRETER_TOKEN_H
#define INTERPRETER_TOKEN_H

#include<string>
#include<vector>
#include<fstream>
#include<iostream>

using namespace std;
struct Token {
    string type;
    string name;
    int line;
    Token(string typeinput, string nameinput,int linenumber) : type(typeinput), name(nameinput), line(linenumber) {}

    string getType() { return type; }
    string getName() { return name; }
    int getLine(){return line;}
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
                }
                else if (idenname == "TRUE") {
                    tokenlist.emplace_back("TRUE", "true",linenumber);
                } else if (idenname == "FALSE") {
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
                    tokenlist.emplace_back("BOOLEAN_NOT","!",linenumber);
                    inputFile.unget();
                }
                break;
            }

            default:
                break;
        }
    }
    return tokenlist;
};

#endif //INTERPRETER_TOKEN_H
