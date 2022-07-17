INCDIR=include
DEPDIR=deps
WORKDIR=work
SRCDIR=src

AST_SRCS:=  $(wildcard  src/ast/*.cpp)
BUILTIN_SRCS := $(wildcard  src/built_ins/*.cpp)
EXPANSION_SRCS:= $(wildcard  src/expansion/*.cpp)
JOB_SRCS:= $(wildcard  src/job_control/*.cpp)
SHELLUTIL_SRCS:= $(wildcard src/shell_utils/*.cpp)
AUTOCOMPLETE_SRCS:= $(wildcard src/autocomplete/*.cpp)
READLINE_SRCS:= $(wildcard src/read_line/*.cpp)

SHELLSRCS:= src/shell_main.cpp src/lex.yy.cpp src/yacc.yy.cpp src/misc.cpp $(EXPANSION_SRCS) $(BUILTIN_SRCS) $(JOB_SRCS) $(AST_SRCS) $(SHELLUTIL_SRCS) $(AUTOCOMPLETE_SRCS) $(READLINE_SRCS)
SHELLOBJS=$(SHELLSRCS:%.cpp=%.o)
DEPFILES:= $(SHELLSRCS:%.cpp=$(DEPDIR)/%.d)

CXX_FLAGS = -std=c++17 -I$(INCDIR) -Isrc -Wall -g 
DEPFLAGS= -MT $@ -MMD -MP -MF $(DEPDIR)/$*.d

vpath %.o $(WORKDIR)/

compile_shell: $(SHELLOBJS)
		$(CXX) $(CXX_FLAGS) $(addprefix $(WORKDIR)/, $(SHELLOBJS)) -o shell
testall: testregular testmemory
testmemory: compile_shell
		cd testing/memory_tests && 	./testall $*
testmemory: compile_shell
		cd testing/memory_tests && 	./testall
testregular: compile_shell
		cd testing/regular_tests && ./testall
testregular_%: compile_shell
		cd testing/regular_tests && ./testall $*
$(WORKDIR): src/lex.yy.cpp src/yacc.yy.cpp
		test -d $(WORKDIR) || mkdir $(WORKDIR)
		mkdir -p $(addprefix $(WORKDIR)/, $(dir $(SHELLOBJS)))
%: compile_%
		./$*
debug_%: compile_%
		gdb ./$*
memory_%: compile_%
		valgrind --track-origins=yes --show-leak-kinds=all --leak-check=full ./$*
src/lex.yy.cpp: src/lex.l
		$(LEX) --header-file=$(patsubst %.cpp,%.hpp, $@) -o $@ $<
src/yacc.yy.cpp: src/yacc.y
		$(YACC) -d -y -t --debug -o $@ $<
%.o %.d: %.cpp | $(DEPDIR) $(WORKDIR)
		$(CXX) $(CXX_FLAGS) $(DEPFLAGS) -c -o $(WORKDIR)/$@ $<  
clean_shell:
		rm -rf $(DEPDIR)
		rm -rf $(WORKDIR)
		rm -rf shell
		rm -rf src/*.yy.*
clean_test%:
		rm testing/$*_tests/out* testing/$*_tests/f* testing/$*_tests/*.*
clean: clean_shell clean_testregular clean_testmemory

$(DEPDIR):
	mkdir -p $(DEPDIR)
	mkdir -p $(addprefix $(DEPDIR)/, $(dir $(SHELLOBJS)))
	mkdir -p $(DEPDIR)/$(INCDIR)

-include $(DEPFILES)
