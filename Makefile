CC = gcc
CFLAGS = -Wall -Wextra -g -Iinclude

# All source files to be compiled
SRCS = src/main.c src/process.c src/scheduler.c src/fcfs.c src/sjf.c src/stcf.c src/rr.c src/mlfq.c src/metrics.c src/gantt.c src/utils.c
OBJS = $(SRCS:.c=.o)
EXEC = schedsim

# all: Compile the simulator 
all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# clean: Remove binaries and object files 
clean:
	rm -f $(OBJS) $(EXEC)

# test: Run automated test suite 
test: $(EXEC)
	bash tests/test_suite.sh