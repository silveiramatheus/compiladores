CXX = g++
CXXFLAGS = -std=c++11 -Wall

target: etapa4

etapa4: main.o symbols.o ast.o parser.tab.o lex.yy.o semantic.o
	$(CXX) $(CXXFLAGS) symbols.o ast.o parser.tab.o lex.yy.o semantic.o -o etapa4

semantic.o: parser.tab.hpp
symbols.o: parser.tab.hpp
ast.o: parser.tab.hpp
main.o: parser.tab.hpp
parser.tab.o: parser.tab.cpp parser.tab.hpp

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $< -c

lex.yy.cpp: scanner.l
	flex -o lex.yy.cpp scanner.l 

parser.tab.cpp parser.tab.hpp: parser.ypp
	bison parser.ypp -d

clean:
	rm -f etapa4 lex.yy.cpp parser.tab.cpp parser.tab.hpp *.o