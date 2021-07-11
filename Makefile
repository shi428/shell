CPP = g++ -std=c++11 -g -Wall -Werror
SRCS=shell.cpp
TOKENSRCS=token_main.cpp tokenizer.cpp
SHELLOBJS=$(SRCS:%.cpp=%.o)
TOKENOBJS=$(TOKENSRCS:%.cpp=%.o)
OBJS=$(TOKENOBJS) $(SHELLOBJS)
token: compile_token
	./tokenizer
shell: compile_shell
	./shell
debug: compile_shell
	gdb ./$(ELF)
memory: compile_shell
	valgrind --show-leak-kinds=all --leak-check=full ./$(ELF)
compile_shell: $(SHELLOBJS)
	$(CPP) $(SHELLOBJS) -o shell
compile_token: $(TOKENOBJS)
	$(CPP) $(TOKENOBJS) -o tokenizer
.cpp.o:
	$(CPP) -c $*.cpp
clean:
	rm -rf $(OBJS) shell tokenizer
	
