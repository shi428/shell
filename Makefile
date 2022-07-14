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
HEADERS=shell.h shell_util.h

SHELLSRCS=shell_main.cpp misc.cpp lex.yy.cpp yacc.yy.cpp $(EXPANSION_SRCS) $(BUILTIN_SRCS) $(JOB_SRCS) $(AST_SRCS) $(SHELLUTIL_SRCS) $(AUTOCOMPLETE_SRCS) $(READLINE_SRCS)
SHELLOBJS=$(SHELLSRCS:%.cpp=%.o)
PRECOMPILED_HEADERS=$(HEADERS:%.h=%.h.gch)

vpath %.cpp $(SRCDIR)/ 
vpath %.cpp $(SRCDIR)/expansion
vpath %.cpp $(SRCDIR)/built_ins
vpath %.cpp $(SRCDIR)/job_control
vpath %.cpp $(SRCDIR)/ast
vpath %.cpp $(SRCDIR)/shell_utils
vpath %.cpp $(SRCDIR)/autocomplete
vpath %.cpp $(SRCDIR)/read_line
vpath %.h $(INCDIR)/
vpath %.l $(SRCDIR)/
vpath %.y $(SRCDIR)/
vpath %.o $(WORKDIR)/
compile_shell:
		$(MAKE) $(WORKDIR)
		$(MAKE) headers
		$(MAKE) $(SHELLOBJS)
		$(CPP) $(addprefix $(WORKDIR)/, $(SHELLOBJS)) -o shell
testall: testregular testmemory
testmemory: compile_shell
		cd testing/memory_tests && 	./testall $@
testmemory: compile_shell
		cd testing/memory_tests && 	./testall
testregular: compile_shell
		cd testing/regular_tests && ./testall
testregular_%: compile_shell
		cd testing/regular_tests && ./testall $@
headers: $(PRECOMPILED_HEADERS)
$(WORKDIR): lex.yy.cpp yacc.yy.cpp
		test -d $(WORKDIR) || mkdir $(WORKDIR)
%: compile_%
		./$*
debug_%: compile_%
		gdb ./$*
memory_%: compile_%
		valgrind --track-origins=yes --show-leak-kinds=all --leak-check=full ./$*
lex.yy.cpp: lex.l
		lex  --header-file=$(SRCDIR)/$(patsubst %.cpp,%.hpp, $@) -o $(SRCDIR)/$@ $<
yacc.yy.cpp: yacc.y
		bison -d -y -t --debug -o $(SRCDIR)/yacc.yy.cpp $<
compile_readline: $(READLINEOBJS)
		$(CPP) $(addprefix $(WORKDIR)/, $(READLINEOBJS)) -o readline
%.h.gch: %.h
		$(CPP) $< -o $(INCDIR)/$@
%.o: %.cpp
		@#time --format='%e' $(CPP) -c $<  -o  $(WORKDIR)/$@ 
		$(CPP) -c $< -o  $(WORKDIR)/$@ 
clean_shell:
		rm -rf $(addprefix $(WORKDIR)/, $(SHELLOBJS)) $(PRECOMPILED_HEADERS) shell src/lex.yy.cpp src/yacc.yy.cpp src/yacc.yy.hpp src/lex.yy.hpp
clean_test%:
		rm testing/$*_tests/out* testing/$*_tests/f* testing/$*_tests/*.*
clean: clean_shell clean_testregular clean_testmemory
