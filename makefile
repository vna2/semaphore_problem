OBJS	= main.o
SOURCE	= main.cpp
HEADER	= semaphores.hpp shared_memory_class.hpp child.hpp
OUT	= a.out
CC	 = g++
FLAGS	 = -g3 -c
LFLAGS	 = -lpthread
# -g option enables debugging mode
# -c flag generates object code for separate files


all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)


# create/compile the individual files >>separately<<
main.o: main.cpp
	$(CC) $(FLAGS) main.cpp -std=c++11 -lcunit -lcppunit


# clean house
clean:
	rm -f $(OBJS) $(OUT) keys/* files/* 

# run the program
run: $(OUT)
	./$(OUT)

# compile program with debugging information
debug: $(OUT)
	valgrind $(OUT)

# run program with valgrind for errors
valgrind: $(OUT)
	valgrind $(OUT)

# run program with valgrind for leak checks
valgrind_leakcheck: $(OUT)
	valgrind --leak-check=full $(OUT)

# run program with valgrind for leak checks (extreme)
valgrind_extreme: $(OUT)
	valgrind --leak-check=full --show-leak-kinds=all --leak-resolution=high --track-origins=yes --vgdb=yes $(OUT)
