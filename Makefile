all:
	g++ -g src/dedit.cpp src/CEditor.cpp  src/CSyntaxHighLine.cpp -lcurses -o bin/dedit -std=gnu++0x
clean:
	$(RM) src/*.o
	$(RM) bin/dedit
