CC ?= gcc
CFLAGS = -I. -Wall -g
DEPS = train_yard.h
OBJ = main.o train_yard.o
TEST_OBJ = test_runner.o train_yard.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: train_yard_app

train_yard_app: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

test: $(TEST_OBJ)
	$(CC) -o test_runner_app $^ $(CFLAGS)
	./test_runner_app

clean:
	rm -f *.o train_yard_app test_runner_app
	rm -rf *.dSYM
