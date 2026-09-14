# ViCURA v1.0 - Refactored Release

**Release Date:** September 14, 2026  
**Status:** Stable - Sequential Mode  
**Build Status:** ✅ Sequential | ⏸️ Parallel (requires MPI)

## What's Included

### Executables
- `bin/dbg_sequential` - Sequential genome assembler (618 KB)

### Source Code
- `src/` - Unified source code
  - `core/` - Core data structures (Reads, Graph, Dynamic Tree)
  - `algorithms/` - Assembly algorithms (Bubble, Cycle, DBG, Topological)
  - `io/` - File I/O operations
  - `utils/` - Utility functions
  - `main_sequential.cpp` - Sequential entry point
  - `main_parallel.cpp` - Parallel entry point

### Headers
- `include/` - Header files and abstraction layer
  - `execution_context.h` - Execution mode abstraction
  - `config.h` - Configuration management
  - `pipeline.h` - Pipeline orchestration
  - `mpi_wrapper.h` - MPI compatibility layer
  - `dbg.h` - Core definitions
  - Other algorithm headers

### Configuration
- `config/default.conf` - Sequential mode template
- `config/parallel.conf` - Parallel mode template

### Build System
- `Makefile` - Unified build system

### Documentation
- `README.md` - User guide
- `REFACTORING_FINAL.md` - Complete refactoring report
- `REFACTORING_STATUS.md` - Development status

## Quick Start

### Build
```bash
cd ViCURA_v1.0
make sequential
```

### Run
```bash
./bin/dbg_sequential --help

# Example usage
./bin/dbg_sequential \
    --input-r1 ../data/shi_double_corrected_R1.fastq \
    --input-r2 ../data/shi_double_corrected_R2.fastq \
    --workspace /tmp/assembly \
    --build-olc \
    --extract-subgraphs \
    --detect-cycles
```

## Requirements

### Sequential Version
- C++11 compatible compiler (gcc 4.8+)
- Make
- ~4-40 GB RAM (depending on dataset size)

### Parallel Version (optional)
- MPI implementation (OpenMPI 1.10+ or MPICH 3.2+)
- mpicxx compiler wrapper

## Key Features

✅ Unified codebase for sequential and parallel modes  
✅ Flexible configuration system  
✅ Clean abstraction layer  
✅ No MPI required for sequential mode  
✅ Comprehensive documentation  
✅ Extensible architecture  

## Version History

### v1.0 (2026-09-14)
- Initial refactored release
- Unified codebase from separate sequential/parallel implementations
- Added abstraction layer (ExecutionContext, Config, Pipeline)
- Added MPI wrapper for non-MPI builds
- Sequential mode fully functional
- Comprehensive documentation

## Known Limitations

- Parallel mode not yet tested (requires MPI environment)
- Limited automated testing
- Performance benchmarking pending

## Support

See `README.md` for detailed usage instructions.  
See `REFACTORING_FINAL.md` for architecture and development details.

## License

[Add license information]

---

**Built with:** Claude Code  
**Architecture:** x86-64  
**Platform:** Linux
