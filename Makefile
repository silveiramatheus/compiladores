CXX = g++
CXXFLAGS = -std=c++11 -Wall

target: etapa1

etapa1: main.o lex.yy.o symbols.o 
	$(CXX) $(CXXFLAGS) lex.yy.o symbols.o -o etapa1

%.o: %.cpp 
	$(CXX) $(CXXFLAGS) $< -c

lex.yy.o: lex.yy.cpp
	$(CXX) $(CXXFLAGS) lex.yy.cpp -c

lex.yy.cpp: scanner.l
	flex -o lex.yy.cpp scanner.l 

clean:
	rm -f etapa1 lex.yy.cpp *.o