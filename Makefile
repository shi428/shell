INCDIR=include
CPP = g++ -std=c++11 -g -I $(INCDIR) -Wall -Werror
WORKDIR=work
SRCDIR=src
SHELLSRCS=shell_main.cpp tokenizer.cpp misc.cpp parser.cpp command.cpp exec.cpp built-in.cpp expansion.cpp cat.cpp read_line.cpp tty_raw_mode.cpp
TOKENSRCS=token_main.cpp tokenizer.cpp misc.cpp
PARSERSRCS=parser.cpp tokenizer.cpp misc.cpp parser_main.cpp command.cpp exec.cpp cat.cpp
READLINESRCS=read_line.cpp read_line_main.cpp trie.cpp tty_raw_mode.cpp
SHELLOBJS=$(SHELLSRCS:%.cpp=%.o)
TOKENOBJS=$(TOKENSRCS:%.cpp=%.o)
PARSEROBJS=$(PARSERSRCS:%.cpp=%.o)
READLINEOBJS=$(READLINESRCS:%.cpp=%.o)

vpath %.cpp $(SRCDIR)/
vpath %.o $(WORKDIR)/
testall: compile_shell
	cd testing && ./testall
test_%: compile_shell
	cd testing && ./testall $@
$(WORKDIR): 
	test -d $(WORKDIR) || mkdir $(WORKDIR)
%: compile_%
	./$*
debug_%: compile_%
	gdb ./$*
memory_%: compile_%
	valgrind --show-leak-kinds=all --leak-check=full ./$*
compile_shell: $(SHELLOBJS)
	$(CPP) $(addprefix $(WORKDIR)/, $(SHELLOBJS)) -o shell
compile_tokenizer: $(TOKENOBJS)
	$(CPP) $(addprefix $(WORKDIR)/,  $(TOKENOBJS)) -o tokenizer
compile_parser: $(PARSEROBJS)
	$(CPP) $(addprefix $(WORKDIR)/, $(PARSEROBJS)) -o parser
compile_readline: $(READLINEOBJS)
	$(CPP) $(addprefix $(WORKDIR)/, $(READLINEOBJS)) -o readline
%.o: %.cpp | $(WORKDIR)
	$(CPP) -c $< -o $(WORKDIR)/$@ 
clean:
	rm -rf shell tokenizer parser work readline testing/*diff testing/out* testing/*.out testing/o* testing/f*
