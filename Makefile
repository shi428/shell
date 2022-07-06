INCDIR=include
CPP = g++ -std=c++17 -I$(INCDIR) -Isrc -Wall -g #-Wall -I$(INCDIR) -Isrc
WORKDIR=work
SRCDIR=src
AST_SRCS=$(shell ls src/ast)
BUILTIN_SRCS = $(shell ls src/built_ins)
EXPANSION_SRCS=$(shell ls src/expansion)
JOB_SRCS=$(shell ls src/job_control)
SHELLUTIL_SRCS=$(shell ls src/shell_utils)
AUTOCOMPLETE_SRCS=$(shell ls src/autocomplete)
READLINE_SRCS=$(shell ls src/read_line)
SHELLSRCS=shell_main.cpp misc.cpp lex.yy.cpp yacc.yy.cpp $(EXPANSION_SRCS) $(BUILTIN_SRCS) $(JOB_SRCS) $(AST_SRCS) $(SHELLUTIL_SRCS) $(AUTOCOMPLETE_SRCS) $(READLINE_SRCS)
SHELLOBJS=$(SHELLSRCS:%.cpp=%.o)
	TOKENOBJS=$(TOKENSRCS:%.cpp=%.o)
vpath %.cpp $(SRCDIR)/ 
vpath %.cpp $(SRCDIR)/expansion
vpath %.cpp $(SRCDIR)/built_ins
vpath %.cpp $(SRCDIR)/job_control
vpath %.cpp $(SRCDIR)/ast
vpath %.cpp $(SRCDIR)/shell_utils
vpath %.cpp $(SRCDIR)/autocomplete
vpath %.cpp $(SRCDIR)/read_line
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
yacc.yy.cpp: yacc.y
		bison -d -y -t --debug -o $(SRCDIR)/yacc.yy.cpp $<
compile_shell:
		make $(MAKEFLAGS) $(WORKDIR)
		make $(MAKEFLAGS) headers
		make $(MAKEFLAGS) $(SHELLOBJS)
		$(CPP) $(addprefix $(WORKDIR)/, $(SHELLOBJS)) -o shell
compile_readline: $(READLINEOBJS)
		$(CPP) $(addprefix $(WORKDIR)/, $(READLINEOBJS)) -o readline
%.o: %.cpp
		@#time --format='%e' $(CPP) -c $<  -o  $(WORKDIR)/$@ 
		$(CPP) -c $< -o  $(WORKDIR)/$@ 
clean:
		rm -rf shell tokenizer parser work readline testing/*diff testing/out* testing/*.out testing/o* testing/f* src/lex.yy.cpp src/yacc.yy.cpp src/yacc.yy.hpp src/lex.yy.hpp include/*.gch src/*.gch
