CC       := cc
CFLAGS   := -std=c11 -Wall -Wextra -Wpedantic -O2 -Iinclude
LDFLAGS  :=
TEST_TARGET   := bin/algorithms_tests
BENCH_TARGET  := bin/algorithms_benchmark

LIB_SOURCES  := $(wildcard src/*.c)
LIB_OBJECTS  := $(patsubst %.c,build/%.o,$(LIB_SOURCES))
TEST_OBJECT  := build/tests/test_main.o
BENCH_OBJECT := build/tests/benchmark_main.o

.PHONY: all clean run benchmark

all: $(TEST_TARGET) $(BENCH_TARGET)

$(TEST_TARGET): $(LIB_OBJECTS) $(TEST_OBJECT)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

$(BENCH_TARGET): $(LIB_OBJECTS) $(BENCH_OBJECT)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

build/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

-include $(LIB_OBJECTS:.o=.d) $(TEST_OBJECT:.o=.d) $(BENCH_OBJECT:.o=.d)

run: $(TEST_TARGET)
	./$(TEST_TARGET)

benchmark: $(BENCH_TARGET)
	./$(BENCH_TARGET)

clean:
	rm -rf build bin
