#ifndef MPI_WRAPPER_H
#define MPI_WRAPPER_H

/**
 * MPI Wrapper - Provides MPI-like interface for both MPI and non-MPI builds
 * When USE_MPI is defined: forwards to actual MPI functions
 * When USE_MPI is not defined: provides no-op implementations
 */

#ifdef USE_MPI
    #include "mpi.h"
    // MPI is available, use real functions
    #define MPI_AVAILABLE 1
#else
    // MPI is not available, provide stubs
    #define MPI_AVAILABLE 0

    // Define MPI types as int when MPI is not available
    typedef int MPI_Comm;
    #define MPI_COMM_WORLD 0
    #define MPI_INT 0

    // Stub implementations
    inline int MPI_Init(int* argc, char*** argv) { return 0; }
    inline int MPI_Finalize() { return 0; }
    inline int MPI_Comm_rank(MPI_Comm comm, int* rank) { *rank = 0; return 0; }
    inline int MPI_Comm_size(MPI_Comm comm, int* size) { *size = 1; return 0; }
    inline int MPI_Barrier(MPI_Comm comm) { return 0; }
    inline int MPI_Bcast(void* buffer, int count, int datatype, int root, MPI_Comm comm) { return 0; }
#endif

#endif // MPI_WRAPPER_H
