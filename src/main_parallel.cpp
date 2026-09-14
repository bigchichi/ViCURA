#include "config.h"
#include "execution_context.h"
#include "pipeline_parallel.h"
#include <iostream>

#ifdef USE_MPI
using namespace std;

int main(int argc, char** argv) {
    // Create execution context (MPI mode)
    MPIContext context;
    context.init(&argc, &argv);

    int my_rank = context.getRank();

    // Load configuration
    Config config;

    if (argc > 1 && string(argv[1]) == "--config" && argc > 2) {
        config = Config::fromFile(argv[2]);
    } else {
        config = Config::fromCommandLine(argc, argv);
    }

    // Force parallel mode
    config.use_mpi = true;
    config.sequential_mode = false;

    // Set default pipeline stages for parallel mode
    config.read_olc_from_file = true;  // Parallel mode reads pre-computed OLC
    config.parallel_kmer_processing = true;
    config.build_dbg = true;

    // Validate configuration
    if (config.olc_input_path.empty()) {
        if (my_rank == 0) {
            cerr << "Error: Parallel mode requires --olc-input path" << endl;
            Config::printHelp();
        }
        context.finalize();
        return 1;
    }

    // Create and run parallel pipeline
    try {
        ParallelPipeline pipeline(config, &context);
        pipeline.execute();
    } catch (const exception& e) {
        cerr << "Error on rank " << my_rank << ": " << e.what() << endl;
        context.finalize();
        return 1;
    }

    context.finalize();
    return 0;
}

#else
// MPI not available
int main() {
    cerr << "Error: This executable was compiled without MPI support." << endl;
    cerr << "Please rebuild with USE_MPI=1 or use the sequential version." << endl;
    return 1;
}
#endif // USE_MPI
