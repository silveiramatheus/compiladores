CXX = g++
CXXFLAGS = -std=c++11 -Wall

target: etapa2

etapa2: main.o symbols.o parser.tab.o lex.yy.o
	$(CXX) $(CXXFLAGS) symbols.o parser.tab.o lex.yy.o -o etapa2

symbols.o: parser.tab.hpp
main.o: parser.tab.hpp
parser.tab.o: parser.tab.cpp parser.tab.hpp

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $< -c

lex.yy.cpp: scanner.l
	flex -o lex.yy.cpp scanner.l 

parser.tab.cpp parser.tab.hpp: parser.ypp
	bison parser.ypp -d

clean:
	rm -f etapa2 lex.yy.cpp parser.tab.cpp parser.tab.hpp *.o