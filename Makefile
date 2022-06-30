INCDIR=include
CPP = g++ -std=c++17 -g -Wall -I$(INCDIR)
WORKDIR=work
SRCDIR=src
AST_SRCS=$(shell ls src/ast)
BUILTIN_SRCS = $(shell ls src/built_ins)
EXPANSION_SRCS=$(shell ls src/expansion)
JOB_SRCS=$(shell ls src/job_control)
SHELLSRCS=shell_main.cpp misc.cpp command.cpp cat.cpp read_line.cpp tty_raw_mode.cpp trie.cpp autocomplete.cpp lex.yy.cpp yacc.yy.cpp shell.cpp tee.cpp $(EXPANSION_SRCS) $(BUILTIN_SRCS) $(JOB_SRCS) $(AST_SRCS)
TOKENSRCS=token_main.cpp misc.cpp read_line.cpp trie.cpp tty_raw_mode.cpp autocomplete.cpp
PARSERSRCS=tokenizer.cpp misc.cpp parser_main.cpp trie.cpp tty_raw_mode.cpp autocomplete.cpp read_line.cpp command.cpp parser.cpp built-in.cpp
READLINESRCS=read_line.cpp read_line_main.cpp trie.cpp tty_raw_mode.cpp
SHELLOBJS=$(SHELLSRCS:%.cpp=%.o)
TOKENOBJS=$(TOKENSRCS:%.cpp=%.o)
PARSEROBJS=$(PARSERSRCS:%.cpp=%.o)
READLINEOBJS=$(READLINESRCS:%.cpp=%.o)

vpath %.cpp $(SRCDIR)/ 
vpath %.cpp $(SRCDIR)/expansion
vpath %.cpp $(SRCDIR)/built_ins
vpath %.cpp $(SRCDIR)/job_control
vpath %.cpp $(SRCDIR)/ast
vpath %.l $(SRCDIR)/
vpath %.y $(SRCDIR)/
vpath %.o $(WORKDIR)/
testall: compile_shell
	cd testing && ./testall
test_%: compile_shell
	cd testing && ./testall $@
headers:
	$(CPP) include/shell_util.h
	$(CPP) include/shell.h
$(WORKDIR): lex.yy.cpp yacc.yy.cpp
	test -d $(WORKDIR) || mkdir $(WORKDIR)
%: compile_%
	./$*
debug_%: compile_%
	gdb ./$*
memory_%: compile_%
	valgrind --show-leak-kinds=all --leak-check=full ./$*
lex.yy.cpp: lex.l
	lex  --header-file=$(SRCDIR)/$(patsubst %.cpp,%.hpp, $@) -o $(SRCDIR)/$@ $<
#lex.yy.o: lex.yy.cpp
#	time --format='%e' $(CPP) -c $(SRCDIR)/$< -o $(WORKDIR)/$@ 
yacc.yy.cpp: yacc.y
	bison -d -y -t --debug -o $(SRCDIR)/yacc.yy.cpp $<
#yacc.yy.o: yacc.yy.cpp
compile_shell:
	make $(MAKEFLAGS) $(WORKDIR)
	make $(MAKEFLAGS) headers
	make $(MAKEFLAGS) $(SHELLOBJS)
	$(CPP) $(addprefix $(WORKDIR)/, $(SHELLOBJS)) -o shell
compile_tokenizer: $(TOKENOBJS) lex.yy.o
	$(CPP) $(addprefix $(WORKDIR)/,  $(TOKENOBJS) lex.yy.o) -o tokenizer
compile_parser: $(PARSEROBJS) lex.yy.o yacc.yy.o
	$(CPP) $(addprefix $(WORKDIR)/, $(PARSEROBJS) lex.yy.o yacc.yy.o) -o parser
compile_readline: $(READLINEOBJS)
	$(CPP) $(addprefix $(WORKDIR)/, $(READLINEOBJS)) -o readline
%.o: %.cpp
	@#time --format='%e' $(CPP) -c $<  -o  $(WORKDIR)/$@ 
	$(CPP) -c $< -o  $(WORKDIR)/$@ 
clean:
	rm -rf shell tokenizer parser work readline testing/*diff testing/out* testing/*.out testing/o* testing/f* src/lex.yy.cpp src/yacc.yy.cpp src/yacc.yy.hpp src/lex.yy.hpp include/*.gch src/*.gch
