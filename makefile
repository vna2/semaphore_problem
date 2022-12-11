OBJS	= main.o shared_mem_sem.o  
SOURCE	= main.cpp shared_mem_sem.cpp  
HEADER	= shared_mem_sem.hpp shared_memory_class.hpp  
OUT	= a.out
CC	 = g++
FLAGS	 = -g3 -c
LFLAGS	 = -lpthread

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)
	@ mkdir -p keys
	@ mkdir -p outputs

main.o: main.cpp
	$(CC) $(FLAGS) main.cpp -lcunit -lcppunit

shared_mem_sem.o: shared_mem_sem.cpp
	$(CC) $(FLAGS) shared_mem_sem.cpp -lcunit -lcppunit


clean:
	rm -f $(OBJS) $(OUT) keys/* files/* outputs/* 

debug: $(OUT)
	valgrind $(OUT)

valgrind: $(OUT)
	valgrind $(OUT)

valgrind_leakcheck: $(OUT)
	valgrind --leak-check=full $(OUT)

valgrind_extreme: $(OUT)
	valgrind --leak-check=full --show-leak-kinds=all --leak-resolution=high --track-origins=yes --vgdb=yes $(OUT)

