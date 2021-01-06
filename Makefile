CPP = g++ -std=c++11 -g -Wall -Werror
ELF = shell
SRCS=shell.cpp
OBJS=$(SRCS:%.cpp=%.o)
run: compile
	./$(ELF)
debug: compile
	gdb ./$(ELF)
memory: compile
	valgrind --show-leak-kinds=all --leak-check=full ./$(ELF)
compile: $(OBJS)
	$(CPP) $(OBJS) -o shell
.cpp.o:
	$(CPP) -c $*.cpp
clean:
	rm -rf $(OBJS) shell
	
