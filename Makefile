# ViCURA Unified Makefile
# Supports both sequential and parallel (MPI) builds

# Compiler settings
CXX_SEQ = g++
CXX_MPI = mpicxx
CXXFLAGS = -std=c++11 -fomit-frame-pointer -O3 -ffast-math -funroll-loops -mmmx -msse -msse2 -msse3 -fmessage-length=0

# Directories
SRCDIR = src
INCDIR = include
COREDIR = $(SRCDIR)/core
ALGODIR = $(SRCDIR)/algorithms
IODIR = $(SRCDIR)/io
UTILDIR = $(SRCDIR)/utils
OBJDIR = build
BINDIR = bin

# Include paths
INCLUDES = -I$(INCDIR)

# Source files (shared between sequential and parallel)
CORE_SRCS = $(COREDIR)/Reads.cpp $(COREDIR)/Graph.cpp $(COREDIR)/dynamic_tree.cpp
ALGO_SRCS = $(ALGODIR)/bubble.cpp $(ALGODIR)/Debrujin.cpp $(ALGODIR)/Topological.cpp \
            $(ALGODIR)/cycle.cpp $(ALGODIR)/Merge.cpp $(ALGODIR)/shrink.cpp
IO_SRCS = $(IODIR)/File.cpp
UTIL_SRCS = $(UTILDIR)/Function.cpp

SHARED_SRCS = $(CORE_SRCS) $(ALGO_SRCS) $(IO_SRCS) $(UTIL_SRCS)

# Object files
SHARED_OBJS = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SHARED_SRCS))

# Sequential-specific
SEQ_MAIN = $(SRCDIR)/main_sequential.cpp
SEQ_OBJS = $(OBJDIR)/main_sequential.o $(SHARED_OBJS)
SEQ_TARGET = $(BINDIR)/dbg_sequential

# Parallel-specific
PAR_MAIN = $(SRCDIR)/main_parallel.cpp
PAR_OBJS = $(OBJDIR)/main_parallel.o $(SHARED_OBJS)
PAR_TARGET = $(BINDIR)/dbg_parallel

# Default target
.DEFAULT_GOAL := help

# Phony targets
.PHONY: all sequential parallel clean help dirs test

# Help message
help:
	@echo "ViCURA De Bruijn Graph Assembler - Build System"
	@echo ""
	@echo "Targets:"
	@echo "  make sequential    - Build sequential version (no MPI required)"
	@echo "  make parallel      - Build parallel version (requires MPI)"
	@echo "  make all           - Build both sequential and parallel versions"
	@echo "  make clean         - Remove all build artifacts"
	@echo "  make test          - Run basic tests"
	@echo "  make help          - Show this help message"
	@echo ""
	@echo "Example usage:"
	@echo "  ./bin/dbg_sequential --input-r1 data/R1.fastq --input-r2 data/R2.fastq --workspace /path/to/work"
	@echo "  mpirun -n 4 ./bin/dbg_parallel --olc-input /path/to/olc --workspace /path/to/work"

# Build all versions
all: sequential parallel

# Build sequential version
sequential: dirs $(SEQ_TARGET)

# Build parallel version
parallel: dirs $(PAR_TARGET)

# Create necessary directories
dirs:
	@mkdir -p $(OBJDIR)/core
	@mkdir -p $(OBJDIR)/algorithms
	@mkdir -p $(OBJDIR)/io
	@mkdir -p $(OBJDIR)/utils
	@mkdir -p $(BINDIR)

# Sequential executable
$(SEQ_TARGET): $(SEQ_OBJS)
	@echo "Linking sequential executable..."
	$(CXX_SEQ) $(CXXFLAGS) -o $@ $^
	@echo "Sequential build complete: $@"

# Parallel executable
$(PAR_TARGET): $(PAR_OBJS)
	@echo "Linking parallel executable..."
	$(CXX_MPI) $(CXXFLAGS) -DUSE_MPI -o $@ $^
	@echo "Parallel build complete: $@"

# Sequential main object
$(OBJDIR)/main_sequential.o: $(SEQ_MAIN) $(INCDIR)/*.h
	@echo "Compiling $<..."
	$(CXX_SEQ) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Parallel main object
$(OBJDIR)/main_parallel.o: $(PAR_MAIN) $(INCDIR)/*.h
	@echo "Compiling $< (with MPI)..."
	$(CXX_MPI) $(CXXFLAGS) $(INCLUDES) -DUSE_MPI -c $< -o $@

# Shared object files (core)
$(OBJDIR)/core/%.o: $(COREDIR)/%.cpp $(INCDIR)/*.h
	@echo "Compiling $<..."
	$(CXX_SEQ) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Shared object files (algorithms)
$(OBJDIR)/algorithms/%.o: $(ALGODIR)/%.cpp $(INCDIR)/*.h
	@echo "Compiling $<..."
	$(CXX_SEQ) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Shared object files (io)
$(OBJDIR)/io/%.o: $(IODIR)/%.cpp $(INCDIR)/*.h
	@echo "Compiling $<..."
	$(CXX_SEQ) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Shared object files (utils)
$(OBJDIR)/utils/%.o: $(UTILDIR)/%.cpp $(INCDIR)/*.h
	@echo "Compiling $<..."
	$(CXX_SEQ) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Clean build artifacts
clean:
	@echo "Cleaning build artifacts..."
	rm -rf $(OBJDIR) $(BINDIR)
	@echo "Clean complete"

# Basic test
test: sequential
	@echo "Running basic tests..."
	@if [ -f $(SEQ_TARGET) ]; then \
		echo "Sequential executable exists: OK"; \
		$(SEQ_TARGET) --help; \
	else \
		echo "Sequential executable not found: FAIL"; \
		exit 1; \
	fi
	@if command -v mpirun >/dev/null 2>&1 && [ -f $(PAR_TARGET) ]; then \
		echo "Parallel executable exists: OK"; \
		echo "Testing MPI initialization..."; \
		mpirun -n 2 $(PAR_TARGET) --help; \
	else \
		echo "Parallel testing skipped (MPI not available or executable not built)"; \
	fi

# Install (optional)
install: all
	@echo "Installing executables to /usr/local/bin..."
	@sudo cp $(SEQ_TARGET) /usr/local/bin/
	@sudo cp $(PAR_TARGET) /usr/local/bin/
	@echo "Installation complete"

# Dependency tracking
-include $(OBJDIR)/**/*.d
