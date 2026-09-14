#ifndef EXECUTION_CONTEXT_H
#define EXECUTION_CONTEXT_H

#include <string>
#include <vector>

/**
 * Abstract base class for execution context
 * Provides unified interface for sequential and parallel execution
 */
class ExecutionContext {
public:
    virtual ~ExecutionContext() {}

    // Get process rank (0 for sequential, 0 to N-1 for parallel)
    virtual int getRank() const = 0;

    // Get total number of processes (1 for sequential, N for parallel)
    virtual int getSize() const = 0;

    // Synchronization barrier
    virtual void barrier() = 0;

    // Broadcast integer data from root to all processes
    virtual void broadcast(int* data, int count, int root) = 0;

    // Initialize the execution context
    virtual void init(int* argc, char*** argv) = 0;

    // Finalize the execution context
    virtual void finalize() = 0;

    // Check if this is the master process
    virtual bool isMaster() const { return getRank() == 0; }

    // Check if running in parallel mode
    virtual bool isParallel() const = 0;
};

/**
 * Sequential execution context (single process)
 */
class SequentialContext : public ExecutionContext {
public:
    SequentialContext() {}
    ~SequentialContext() {}

    int getRank() const override { return 0; }
    int getSize() const override { return 1; }
    void barrier() override { /* No-op for sequential */ }
    void broadcast(int* data, int count, int root) override { /* No-op for sequential */ }
    void init(int* argc, char*** argv) override { /* No-op for sequential */ }
    void finalize() override { /* No-op for sequential */ }
    bool isParallel() const override { return false; }
};

#ifdef USE_MPI
#include "mpi.h"

/**
 * MPI parallel execution context
 */
class MPIContext : public ExecutionContext {
private:
    int rank_;
    int size_;

public:
    MPIContext() : rank_(0), size_(1) {}
    ~MPIContext() {}

    int getRank() const override { return rank_; }
    int getSize() const override { return size_; }

    void barrier() override {
        MPI_Barrier(MPI_COMM_WORLD);
    }

    void broadcast(int* data, int count, int root) override {
        MPI_Bcast(data, count, MPI_INT, root, MPI_COMM_WORLD);
    }

    void init(int* argc, char*** argv) override {
        MPI_Init(argc, argv);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank_);
        MPI_Comm_size(MPI_COMM_WORLD, &size_);
    }

    void finalize() override {
        MPI_Finalize();
    }

    bool isParallel() const override { return true; }
};
#endif // USE_MPI

#endif // EXECUTION_CONTEXT_H
