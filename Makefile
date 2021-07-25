CPP = g++ -std=c++11 -g -Wall -Werror
SRCS=shell_main.cpp tokenizer.cpp misc.cpp parser.cpp command.cpp exec.cpp
TOKENSRCS=token_main.cpp tokenizer.cpp misc.cpp
PARSERSRCS=parser.cpp tokenizer.cpp misc.cpp parser_main.cpp command.cpp
SHELLOBJS=$(SRCS:%.cpp=%.o)
TOKENOBJS=$(TOKENSRCS:%.cpp=%.o)
PARSEROBJS=$(PARSERSRCS:%.cpp=%.o)
OBJS=$(TOKENOBJS) $(SHELLOBJS) $(PARSEROBJS)
%: compile_%
	./$*
debug_%: compile_%
	gdb ./$*
memory_%: compile_%
	valgrind --show-leak-kinds=all --leak-check=full ./$*
compile_shell: $(SHELLOBJS)
	$(CPP) $(SHELLOBJS) -o shell
compile_tokenizer: $(TOKENOBJS)
	$(CPP) $(TOKENOBJS) -o tokenizer
compile_parser: $(PARSEROBJS)
	$(CPP) $(PARSEROBJS) -o parser
%.o: %.cpp
	$(CPP) -c $*.cpp
clean:
	rm -rf $(OBJS) shell tokenizer parser
	
