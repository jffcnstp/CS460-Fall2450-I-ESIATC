#include <iostream>
#include <fstream>
#include "CommentRemove.h"
#include <vector>
#include <ctype.h>
#include <string>

struct Token
{
    private:
    std::string type;
    std::string name;
    public:
    Token (std::string typeinput,std::string nameinput)
    {
        type=typeinput;
        name=nameinput;
    }
    std::string gettype(){return type;}
    std::string getname(){return name;}

};

std::vector<Token> Tokenize (const std::string& fileName)
{

    std::ifstream inputFile(fileName); //Filecheck

    if (!inputFile) {
        std::cerr << "Error opening input file: " << fileName << std::endl;
        exit(1);
    }

    int linenumber=1;
    enum TokenType{Identifier,Integer,Equal,LParen,RParen,LBracket,RBracket,LBrace,RBrace,DblQuote,SglQuote,Semicolon,Assignment,BoolEQ,BoolOR,BoolAND,};
    char currentChar;
    std::vector<Token> tokenlist;
    char casetype;
    while (inputFile.get(currentChar)) 
    {
        if(currentChar == '\n')
            linenumber+=1;
        if(isalpha(currentChar))
            casetype='a';
        else if(isdigit(currentChar))
            casetype = '1';
        else
            casetype = currentChar;
        
        switch(casetype)
        {
            case 'a':
                {
                    std::string idenname="";
                    while(isalnum(currentChar)||currentChar=='_')
                    {
                        idenname+=currentChar;
                        inputFile.get(currentChar);
                    }
                    tokenlist.emplace_back("IDENTIFIER",idenname);
                    inputFile.unget();
                    break;
                }
            case '1' :
                {
                    std::string intname="";
                    while(isdigit(currentChar))
                    {
                        intname+=currentChar;
                        inputFile.get(currentChar);
                    }
                    if(isalpha(currentChar))
                        {
                            std::cout<<"Syntax error on line "<<linenumber<<": invalid integer";
                            inputFile.close();
                            exit(3);
                        }
                    else
                    {
                        tokenlist.emplace_back("INTEGER",intname);
                        inputFile.unget();
                    }
                    break;
                }
            case '=':
                {
                    inputFile.get(currentChar);
                    if(currentChar == '=')
                    {
                        tokenlist.emplace_back("BOOLEAN_EQUAL","==");
                    }
                    else
                    {
                        inputFile.unget();
                        tokenlist.emplace_back("ASSIGNMENT_OPERATOR","=");
                    }
                    break;
                }
            case '(':
                {
                    tokenlist.emplace_back("L_PAREN","(");
                    break;
                }
            case ')':
                {tokenlist.emplace_back("R_PAREN",")"); break;}
            case '{':
                {tokenlist.emplace_back("L_BRACE","{"); break;}
            case '}':
                {tokenlist.emplace_back("R_BRACE","}"); break;}
            case '[':
                {tokenlist.emplace_back("L_BRACKET","["); break;}
            case ']':
                {tokenlist.emplace_back("R_BRACKET","]"); break;}
            case '"':
                {
                    std::string strname="";
                    tokenlist.emplace_back("DOUBLE_QUOTE","\"");
                    inputFile.get(currentChar);
                    while(currentChar != '"')
                    {
                        strname+=currentChar;
                        inputFile.get(currentChar);
                    }
                    tokenlist.emplace_back("STRING",strname);
                    tokenlist.emplace_back("DOUBLE_QUOTE","\"");
                    break;
                }
            case '\'':
                {
                    std::string strname="";
                    tokenlist.emplace_back("SINGLE_QUOTE","'");
                    inputFile.get(currentChar);
                    while(currentChar != '\'')
                    {
                        strname+=currentChar;
                        inputFile.get(currentChar);
                    }
                    tokenlist.emplace_back("STRING",strname);
                    tokenlist.emplace_back("SINGLE_QUOTE","'");
                    break;
                }
            case ';':
                {tokenlist.emplace_back("SEMICOLON",";"); break;}
            case ',':
                {tokenlist.emplace_back("COMMA",","); break;}
            case '+':
                {tokenlist.emplace_back("PLUS","+"); break;}
            case '-':
                {tokenlist.emplace_back("MINUS","-"); break;}
            case '*':
                {
                    tokenlist.emplace_back("ASTERISK","*"); 
                    break;
                }
            case '/':
                {tokenlist.emplace_back("DIVIDE","/"); break;}
            case '%':
                {tokenlist.emplace_back("MODULO","%"); break;}
            case '^':
                {tokenlist.emplace_back("CARET","^"); break;}
            case '<':
                {
                    inputFile.get(currentChar);
                    if(currentChar == '=')
                        tokenlist.emplace_back("LT_EQUAL","<=");
                    else
                    {
                        tokenlist.emplace_back("LT","<");
                        inputFile.unget();
                    }
                    break;
                }//editing all of these needs =
            case '>':
                 {
                    inputFile.get(currentChar);
                    if(currentChar == '=')
                        tokenlist.emplace_back("GT_EQUAL",">=");
                    else
                    {
                        tokenlist.emplace_back("GT",">");
                        inputFile.unget();
                    }
                    break;
                }
            case '&':
                {
                    inputFile.get(currentChar);
                    if(currentChar == '&')
                        tokenlist.emplace_back("BOOLEAN_AND","&&");
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
                        tokenlist.emplace_back("BOOLEAN_OR","||");
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
                    tokenlist.emplace_back("BOOLEAN_NOT_EQUAL","!=");
                else
                {
                    tokenlist.emplace_back("BOOLEAN_NOT",",");
                    inputFile.unget();
                }
            break;
            }
        }
               
    }
    return tokenlist;
}   


int main()
{
    std::vector<Token> displayvect;

    ignoreComments("../programming_assignment_2-test_file_1.c", "P2output1.txt");
    displayvect=Tokenize("P2output1.txt");
    std::cout<<"Token List for test file 1:"<<std::endl;
    for(int counter=0;counter<displayvect.size();counter++)
    {
        std::cout<<"Token Type: "<<displayvect[counter].gettype()<<std::endl;
        std::cout<<"Token Name: "<<displayvect[counter].getname()<<std::endl<<std::endl;
    }
    
    ignoreComments("../programming_assignment_2-test_file_2.c", "P2output2.txt");
    displayvect=Tokenize("P2output2.txt");
    std::cout<<"Token List for test file 2:"<<std::endl;
    for(int counter=0;counter<displayvect.size();counter++)
    {
        std::cout<<"Token Type: "<<displayvect[counter].gettype()<<std::endl;
        std::cout<<"Token Name: "<<displayvect[counter].getname()<<std::endl<<std::endl;
    }

    ignoreComments("../programming_assignment_2-test_file_3.c", "P2output3.txt");
    displayvect=Tokenize("P2output3.txt");
    std::cout<<"Token List for test file 3:"<<std::endl;
    for(int counter=0;counter<displayvect.size();counter++)
    {
        std::cout<<"Token Type: "<<displayvect[counter].gettype()<<std::endl;
        std::cout<<"Token Name: "<<displayvect[counter].getname()<<std::endl<<std::endl;
    }

    ignoreComments("../programming_assignment_2-test_file_4.c", "P2output4.txt");
    displayvect=Tokenize("P2output4.txt");
    std::cout<<"Token List for test file 4:"<<std::endl;
    for(int counter=0;counter<displayvect.size();counter++)
    {
        std::cout<<"Token Type: "<<displayvect[counter].gettype()<<std::endl;
        std::cout<<"Token Name: "<<displayvect[counter].getname()<<std::endl<<std::endl;
    }

    // ignoreComments("../programming_assignment_2-test_file_5.c", "P2output5.txt");
    // displayvect=Tokenize("P2output5.txt");
    // std::cout<<"Token List for test file 5:"<<std::endl;
    // for(int counter=0;counter<displayvect.size();counter++)
    // {
    //     std::cout<<"Token Type: "<<displayvect[counter].gettype()<<std::endl;
    //     std::cout<<"Token Name: "<<displayvect[counter].getname()<<std::endl<<std::endl;
    // }
    
    ignoreComments("../programming_assignment_2-test_file_6.c", "P2output6.txt");
    displayvect=Tokenize("P2output6.txt");
    std::cout<<"Token List for test file 6:"<<std::endl;
    for(int counter=0;counter<displayvect.size();counter++)
    {
        std::cout<<"Token Type: "<<displayvect[counter].gettype()<<std::endl;
        std::cout<<"Token Name: "<<displayvect[counter].getname()<<std::endl<<std::endl;
    }
    
    return 0;
}