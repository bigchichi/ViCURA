#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>

using namespace std;

/**
 * Configuration management for ViCURA DBG pipeline
 */
class Config {
public:
    // Execution mode
    bool use_mpi;
    bool sequential_mode;

    // File paths
    string workspace;
    string input_R1;
    string input_R2;
    string result_path;
    string olc_input_path;  // For parallel mode: pre-computed OLC data

    // Algorithm parameters
    int minimizer_size;
    int kmer_length;
    int read_length_normal;
    int overlap_min;
    int overlap_max;
    int subgraph_select_number;

    // Pipeline flags
    bool build_olc;           // Build OLC from FASTQ
    bool read_olc_from_file;  // Read pre-computed OLC
    bool extract_subgraphs;
    bool detect_cycles;
    bool build_dbg;
    bool parallel_kmer_processing;

    // Reference paths (optional)
    string ref_path;
    string strain_path;
    int strain_num;

    Config() {
        // Default values
        use_mpi = false;
        sequential_mode = true;

        workspace = ".";
        result_path = workspace + "/result";
        olc_input_path = "";

        minimizer_size = 5;
        kmer_length = 150;
        read_length_normal = 150;
        overlap_min = 25;
        overlap_max = 149;
        subgraph_select_number = 5;

        build_olc = false;
        read_olc_from_file = false;
        extract_subgraphs = false;
        detect_cycles = false;
        build_dbg = false;
        parallel_kmer_processing = false;

        ref_path = "";
        strain_path = "";
        strain_num = 0;
    }

    /**
     * Create configuration from command line arguments
     */
    static Config fromCommandLine(int argc, char** argv) {
        Config config;

        for (int i = 1; i < argc; i++) {
            string arg = argv[i];

            if (arg == "--workspace" && i + 1 < argc) {
                config.workspace = argv[++i];
            }
            else if (arg == "--input-r1" && i + 1 < argc) {
                config.input_R1 = argv[++i];
            }
            else if (arg == "--input-r2" && i + 1 < argc) {
                config.input_R2 = argv[++i];
            }
            else if (arg == "--olc-input" && i + 1 < argc) {
                config.olc_input_path = argv[++i];
                config.read_olc_from_file = true;
            }
            else if (arg == "--minimizer-size" && i + 1 < argc) {
                config.minimizer_size = atoi(argv[++i]);
            }
            else if (arg == "--kmer-length" && i + 1 < argc) {
                config.kmer_length = atoi(argv[++i]);
            }
            else if (arg == "--sequential") {
                config.sequential_mode = true;
                config.use_mpi = false;
            }
            else if (arg == "--parallel") {
                config.sequential_mode = false;
                config.use_mpi = true;
            }
            else if (arg == "--build-olc") {
                config.build_olc = true;
            }
            else if (arg == "--extract-subgraphs") {
                config.extract_subgraphs = true;
            }
            else if (arg == "--detect-cycles") {
                config.detect_cycles = true;
            }
            else if (arg == "--build-dbg") {
                config.build_dbg = true;
            }
            else if (arg == "--help" || arg == "-h") {
                printHelp();
                exit(0);
            }
        }

        // Update result path based on workspace
        if (config.result_path == "./result") {
            config.result_path = config.workspace + "/result";
        }

        return config;
    }

    /**
     * Load configuration from file
     */
    static Config fromFile(const string& filepath) {
        Config config;
        ifstream file(filepath);

        if (!file.is_open()) {
            cerr << "Warning: Could not open config file: " << filepath << endl;
            return config;
        }

        string line;
        while (getline(file, line)) {
            // Skip comments and empty lines
            if (line.empty() || line[0] == '#') continue;

            stringstream ss(line);
            string key, value;

            if (getline(ss, key, '=') && getline(ss, value)) {
                // Trim whitespace
                key.erase(0, key.find_first_not_of(" \t"));
                key.erase(key.find_last_not_of(" \t") + 1);
                value.erase(0, value.find_first_not_of(" \t"));
                value.erase(value.find_last_not_of(" \t") + 1);

                // Parse key-value pairs
                if (key == "workspace") config.workspace = value;
                else if (key == "input_R1") config.input_R1 = value;
                else if (key == "input_R2") config.input_R2 = value;
                else if (key == "minimizer_size") config.minimizer_size = atoi(value.c_str());
                else if (key == "kmer_length") config.kmer_length = atoi(value.c_str());
                else if (key == "use_mpi") config.use_mpi = (value == "true" || value == "1");
            }
        }

        file.close();
        return config;
    }

    /**
     * Print configuration
     */
    void print() const {
        cout << "=== ViCURA Configuration ===" << endl;
        cout << "Execution mode: " << (use_mpi ? "Parallel (MPI)" : "Sequential") << endl;
        cout << "Workspace: " << workspace << endl;
        cout << "Input R1: " << input_R1 << endl;
        cout << "Input R2: " << input_R2 << endl;
        cout << "Minimizer size: " << minimizer_size << endl;
        cout << "K-mer length: " << kmer_length << endl;
        cout << "Result path: " << result_path << endl;

        if (read_olc_from_file) {
            cout << "OLC input path: " << olc_input_path << endl;
        }

        cout << "Pipeline stages:" << endl;
        if (build_olc) cout << "  - Build OLC" << endl;
        if (read_olc_from_file) cout << "  - Read OLC from file" << endl;
        if (extract_subgraphs) cout << "  - Extract subgraphs" << endl;
        if (detect_cycles) cout << "  - Detect cycles" << endl;
        if (build_dbg) cout << "  - Build DBG" << endl;
        cout << "============================" << endl;
    }

    /**
     * Print help message
     */
    static void printHelp() {
        cout << "ViCURA - De Bruijn Graph Genome Assembler" << endl;
        cout << "Usage: dbg [options]" << endl;
        cout << endl;
        cout << "Options:" << endl;
        cout << "  --workspace <path>        Set workspace directory" << endl;
        cout << "  --input-r1 <path>         Input FASTQ file (R1)" << endl;
        cout << "  --input-r2 <path>         Input FASTQ file (R2)" << endl;
        cout << "  --olc-input <path>        Pre-computed OLC data path" << endl;
        cout << "  --minimizer-size <int>    Minimizer size (default: 5)" << endl;
        cout << "  --kmer-length <int>       K-mer length (default: 150)" << endl;
        cout << "  --sequential              Run in sequential mode" << endl;
        cout << "  --parallel                Run in parallel mode (requires MPI)" << endl;
        cout << "  --build-olc               Build OLC from FASTQ files" << endl;
        cout << "  --extract-subgraphs       Extract subgraphs" << endl;
        cout << "  --detect-cycles           Detect and handle cycles" << endl;
        cout << "  --build-dbg               Build De Bruijn graph" << endl;
        cout << "  -h, --help                Show this help message" << endl;
    }
};

#endif // CONFIG_H
