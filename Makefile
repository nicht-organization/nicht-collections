CC ?= gcc
CFLAGS ?= -O3 -Wall -Wextra -std=c11 -Iinclude
LDFLAGS ?= -lm

BUILD_DIR = build
BIN_DIR = $(BUILD_DIR)/bin

TEST_SRC = tests/test_collections.c
BENCH_PHI_SRC = benchmarks/bench_phi_heap.c
BENCH_SPECTRAL_SRC = benchmarks/bench_spectral.c

TEST_BIN = $(BIN_DIR)/test_collections
BENCH_PHI_BIN = $(BIN_DIR)/bench_phi_heap
BENCH_SPECTRAL_BIN = $(BIN_DIR)/bench_spectral

.PHONY: all clean test bench

all: test bench

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Fix here: ensured target uses '| $(BIN_DIR)' without extra characters
$(TEST_BIN): $(TEST_SRC) | $(BIN_DIR)
	$(CC) $(CFLAGS) $< $(LDFLAGS) -o $@

$(BENCH_PHI_BIN): $(BENCH_PHI_SRC) | $(BIN_DIR)
	$(CC) $(CFLAGS) $< $(LDFLAGS) -o $@

$(BENCH_SPECTRAL_BIN): $(BENCH_SPECTRAL_SRC) | $(BIN_DIR)
	$(CC) $(CFLAGS) $< $(LDFLAGS) -o $@

test: $(TEST_BIN)
	@echo "--- Running Unit Tests ---"
	@./$(TEST_BIN)

bench: $(BENCH_PHI_BIN) $(BENCH_SPECTRAL_BIN)
	@echo "--- Running Benchmarks ---"
	@./$(BENCH_PHI_BIN)
	@./$(BENCH_SPECTRAL_BIN)

clean:
	rm -rf $(BUILD_DIR)
	