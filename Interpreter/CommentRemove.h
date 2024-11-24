#ifndef COMMENT_REMOVE_H
#define COMMENT_REMOVE_H

/*
 * CS460
 * Programming Assignment 1: Ignore Comments
 * Date: September 8, 2024
 * Team: Fifty If-Else Statements in a Trench Coat (FIESTC)
 * Members: Ryan Nguyen
 *          Anthony Tara
 *          Jericho Cariaga
 *          Jeff Huang
 *          Adam Erskine
 *
 * This program is meant to take a C style program file and replace all comments with whitespace
 */

#include <iostream>
#include <fstream>
#include <string>

void ignoreComments(const std::string& fileName, const std::string& outputName) {
    std::ifstream inputFile(fileName);
    std::ofstream outputFile(outputName);

    if (!inputFile) {
        std::cerr << "Error opening input file: " << fileName << std::endl;
        exit(10);
    }

    if (!outputFile) {
        std::cerr << "Error opening output file: " << outputName << std::endl;
        exit(10);
    }

    char currentChar;
    int linenum=1;
    int commenterrorline;
    enum DFAState { Normal, Slash, SingleLine, MultiLine, MultiLineEnd, DoubleQuote,Illegalchar };
    DFAState state = Normal;


    while (inputFile.get(currentChar)) {
        if(currentChar == '\n')
            {
                linenum+=1;
            }
        switch (state) {
            case Normal:
                if (currentChar == '/') {
                    state = Slash;
                }
                else if (currentChar == '\"') {
                    outputFile.put(currentChar);
                    state = DoubleQuote;
                }
                else if ( currentChar == '*'){
                    outputFile.put(currentChar);
                    state = Illegalchar;
                    }
                else {
                    outputFile.put(currentChar);
                }
                break;
            case Slash:
                if (currentChar == '/') {
                    outputFile.put(' ');
                    state = SingleLine;
                }
                else if (currentChar == '*') {
                    outputFile.put(' ');
                    state = MultiLine;
                    commenterrorline=linenum;
                }
                else {
                    state = Normal;
                    outputFile.put('/');
                    outputFile.put(currentChar);
                }
                break;
            case SingleLine:
                if (currentChar != '\n') {
                    outputFile.put(' ');
                }
                else {
                    outputFile.put('\n');
                    state = Normal;
                }
                break;
            case MultiLine:
                if (currentChar == '*') {
                    outputFile.put(' ');
                    state = MultiLineEnd;
                }
                else if (currentChar == '\n') {
                    outputFile.put('\n');
                }
                else {
                    outputFile.put(' ');
                }
                break;
            case MultiLineEnd:
                if (currentChar == '/') {
                    outputFile.put(' ');
                    outputFile.put(' ');
                    state = Normal;
                }
                else if(currentChar == '*')
                {
                    outputFile.put(' ');
                }
                else {
                    state = MultiLine;
                }
                break;
            case DoubleQuote:
                if (currentChar != '\"') {
                    outputFile.put(currentChar);
                }
                else if (currentChar == '\n') {
                    outputFile.put('\n');
                }
                else {
                    outputFile.put(currentChar);
                    state = Normal;
                }
                break;
            case Illegalchar:
                if(currentChar =='/')
                    {
                        inputFile.close();
                        outputFile.close();
                        std::cout << "ERROR: Program file "<< fileName <<" contains C-style, unterminated comment on line "<<linenum << std::endl;
                        exit(13);
                    }
                else
                {
                    outputFile.put(currentChar);
                    state = Normal;
                    break;
                }
        }
    }

    inputFile.close();
    outputFile.close();
    if(state==MultiLine)
        std::cout << "ERROR: Program file "<< fileName <<" contains C-style, unterminated comment on line "<<commenterrorline << std::endl;
    else
        std::cout << "Comments in file: " << fileName << " successfully ignored.\n";
}


#endif