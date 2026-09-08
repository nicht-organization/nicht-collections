CC ?= gcc
CFLAGS ?= -O3 -Wall -Wextra -std=c11 -D_POSIX_C_SOURCE=200809L -Iinclude
LDFLAGS ?= -lm

COV_FLAGS = -O0 -g --coverage -D_POSIX_C_SOURCE=200809L -Iinclude

BUILD_DIR = build
BIN_DIR = $(BUILD_DIR)/bin

TEST_SRC = tests/test_collections.c
BENCH_PHI_SRC = benchmarks/bench_phi_heap.c
BENCH_SPECTRAL_SRC = benchmarks/bench_spectral.c

TEST_BIN = $(BIN_DIR)/test_collections
BENCH_PHI_BIN = $(BIN_DIR)/bench_phi_heap
BENCH_SPECTRAL_BIN = $(BIN_DIR)/bench_spectral

.PHONY: all clean test bench coverage

all: test bench

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(TEST_BIN): $(TEST_SRC) | $(BIN_DIR)
	$(CC) $(CFLAGS) $(TEST_SRC) $(LDFLAGS) -o $@

$(BENCH_PHI_BIN): $(BENCH_PHI_SRC) | $(BIN_DIR)
	$(CC) $(CFLAGS) $(BENCH_PHI_SRC) $(LDFLAGS) -o $@

$(BENCH_SPECTRAL_BIN): $(BENCH_SPECTRAL_SRC) | $(BIN_DIR)
	$(CC) $(CFLAGS) $(BENCH_SPECTRAL_SRC) $(LDFLAGS) -o $@

test: $(TEST_BIN)
	@echo "--- Running Unit Tests ---"
	@./$(TEST_BIN)

bench: $(BENCH_PHI_BIN) $(BENCH_SPECTRAL_BIN)
	@echo "--- Running Benchmarks ---"
	@./$(BENCH_PHI_BIN)
	@./$(BENCH_SPECTRAL_BIN)

coverage: clean
	@echo "=== Compiling & Running Unit Tests with GCOV Instrumentation ==="
	$(CC) $(COV_FLAGS) $(TEST_SRC) $(LDFLAGS) -o test_collections_cov
	@./test_collections_cov
	@echo "=== Generating GCOV Coverage Report ==="
	@gcov test_collections_cov-test_collections.gcda
	@rm -f test_collections_cov

clean:
	rm -rf $(BUILD_DIR) *.gcda *.gcno *.gcov test_collections_cov
