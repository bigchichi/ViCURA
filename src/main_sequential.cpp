#include "config.h"
#include "execution_context.h"
#include "pipeline_sequential.h"
#include <iostream>

using namespace std;

int main(int argc, char** argv) {
    // Create execution context (sequential mode)
    SequentialContext context;
    context.init(&argc, &argv);

    // Load configuration
    Config config;

    // Check if config file exists
    if (argc > 1 && string(argv[1]) == "--config" && argc > 2) {
        config = Config::fromFile(argv[2]);
    } else {
        config = Config::fromCommandLine(argc, argv);
    }

    // Force sequential mode
    config.use_mpi = false;
    config.sequential_mode = true;

    // Set default pipeline stages for sequential mode
    if (!config.build_olc && !config.read_olc_from_file) {
        config.build_olc = true;  // Default to building OLC
    }
    config.extract_subgraphs = true;
    config.detect_cycles = true;

    // Validate configuration
    if (config.build_olc && (config.input_R1.empty() || config.input_R2.empty())) {
        cerr << "Error: --build-olc requires --input-r1 and --input-r2" << endl;
        Config::printHelp();
        return 1;
    }

    if (config.read_olc_from_file && config.olc_input_path.empty()) {
        cerr << "Error: Must specify --olc-input path" << endl;
        Config::printHelp();
        return 1;
    }

    // Create and run sequential pipeline
    try {
        SequentialPipeline pipeline(config, &context);
        pipeline.execute();
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        context.finalize();
        return 1;
    }

    context.finalize();
    return 0;
}
