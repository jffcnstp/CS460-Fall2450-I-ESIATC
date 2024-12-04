# Compiler
CXX := g++

# Compiler Flags
CXXFLAGS := -std=c++17 -Wall -Wextra -O2

# Sources and Headers
SOURCES := main.cpp
HEADERS := Token.h SymbolTable.h RecursiveDescentParser.h CommentRemove.h AbstractSyntaxTree.h Interpreter.h

# Output executable
OUTPUT := main

# Rules
.PHONY: all clean

all: $(OUTPUT)

$(OUTPUT): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $(OUTPUT) $(SOURCES)

# Clean
clean:
	rm -f $(OUTPUT)
