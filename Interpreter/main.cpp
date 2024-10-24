#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "CommentRemove.h"
#include "Token.h"
#include "RecursiveDescentParser.h"

using namespace std;


// Main function
int main() {
    vector<Token> tokenlist;
    string fileName ="programming_assignment_3-test_file_";
    string tokenizefile="test_file";

//    for(int i=1; i <11; i++) {
//        ignoreComments(fileName + std::to_string(i) + ".c", tokenizefile + std::to_string(i) + ".c");
//        tokenlist = Tokenize(tokenizefile + std::to_string(i) + ".c");
//        continue;
//    }
    int i=1;
    ignoreComments(fileName + std::to_string(i) + ".c", tokenizefile + std::to_string(i) + ".c");
    tokenlist = Tokenize(tokenizefile + std::to_string(i) + ".c");
    Parser CST(tokenlist);
    CST.buildCST();
    cout<<"CST built successfully"<<endl;
    CST.tree.breadthFirstTraversal();

    return 0;
}
