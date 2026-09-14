#ifndef PIPELINE_H
#define PIPELINE_H

#include "config.h"
#include "execution_context.h"
#include "dbg.h"
#include "OLC_shrink.h"
#include "bucket.h"
#include "SDBG.h"
#include <ctime>
#include <vector>
#include <string>

using namespace std;

/**
 * Main pipeline orchestrator for ViCURA DBG assembly
 */
class Pipeline {
private:
    Config config_;
    ExecutionContext* context_;

    // Core data structures
    vector<Reads> temp_;
    vector<Reads> reads_;
    vector<Reads> temp2_;
    vector<Reads> full_reads_;
    vector<Result> results_;
    vector<ull> root_;
    vector<ull> leave_;
    vector<Debrujin_node> dbg_;
    vector<Cycle> cyc_;
    vector<Subgraph> sub_;
    vector<Subgraph> largesub_;

    Parameter* para_;
    Dynamic_Tree* Droot_;
    Dynamic_Tree* Droot2_;
    Files file_;

    clock_t begin_, end_, mid_;

public:
    Pipeline(const Config& config, ExecutionContext* context)
        : config_(config), context_(context) {
        para_ = new Parameter();
        Droot_ = new Dynamic_Tree();
        Droot2_ = new Dynamic_Tree();
    }

    ~Pipeline() {
        delete para_;
        delete Droot_;
        delete Droot2_;
    }

    /**
     * Execute the full pipeline based on configuration
     */
    void execute() {
        if (context_->isMaster()) {
            cout << "Starting ViCURA pipeline..." << endl;
            config_.print();
        }

        begin_ = clock();

        // Sequential workflow
        if (config_.sequential_mode) {
            executeSequentialPipeline();
        }
        // Parallel workflow
        else if (config_.use_mpi) {
            executeParallelPipeline();
        }

        end_ = clock();

        if (context_->isMaster()) {
            double elapsed = double(end_ - begin_) / CLOCKS_PER_SEC;
            cout << "Pipeline completed in " << elapsed << " seconds" << endl;
        }
    }

private:
    /**
     * Sequential pipeline: full OLC + subgraph extraction + cycle detection
     */
    void executeSequentialPipeline() {
        if (context_->isMaster()) {
            cout << "=== Sequential Pipeline ===" << endl;
        }

        // Step 1: Read FASTQ files and build OLC
        if (config_.build_olc && !config_.input_R1.empty() && !config_.input_R2.empty()) {
            readFASTQAndBuildOLC();
        }
        // Or read pre-computed OLC
        else if (config_.read_olc_from_file && !config_.olc_input_path.empty()) {
            readOLCFromFile();
        }
        else {
            cerr << "Error: No valid input specified. Use --input-r1/r2 with --build-olc or --olc-input" << endl;
            return;
        }

        // Step 2: Generate OLC graph
        if (config_.build_olc) {
            buildOLCGraph();
        }

        // Step 3: Extract largest subgraphs
        if (config_.extract_subgraphs) {
            extractSubgraphs();
        }

        // Step 4: Cycle detection and shrinking
        if (config_.detect_cycles) {
            detectAndShrinkCycles();
        }

        // Step 5: Path finding and output
        findPathsAndOutput();
    }

    /**
     * Parallel pipeline: read OLC + parallel k-mer processing + DBG construction
     */
    void executeParallelPipeline() {
        if (context_->isMaster()) {
            cout << "=== Parallel Pipeline (MPI) ===" << endl;
            cout << "Running with " << context_->getSize() << " processes" << endl;
        }

        // Step 1: Read OLC information (pre-computed)
        if (!config_.olc_input_path.empty()) {
            readOLCFromFile();
        } else {
            cerr << "Error: Parallel mode requires --olc-input path" << endl;
            return;
        }

        // Step 2: Setup reads for parallel processing
        setupReadsForParallel();

        // Step 3: Parallel k-mer bucketing
        if (config_.parallel_kmer_processing) {
            parallelKmerBucketing();
        }

        // Step 4: Parallel DBG construction
        if (config_.build_dbg) {
            parallelDBGConstruction();
        }

        if (context_->isMaster()) {
            cout << "Parallel pipeline completed" << endl;
        }
    }

    /**
     * Read FASTQ files and build initial OLC
     */
    void readFASTQAndBuildOLC() {
        if (context_->isMaster()) {
            cout << "Reading FASTQ files..." << endl;
            cout << "  R1: " << config_.input_R1 << endl;
            cout << "  R2: " << config_.input_R2 << endl;
        }

        reads_ = file_.file_read_Dtree_shrink(
            config_.input_R1,
            config_.input_R2,
            para_,
            1,
            Droot_,
            2
        );

        if (context_->isMaster()) {
            cout << "Reads loaded: " << reads_.size() << endl;
        }

        para_->rLengthNormal = config_.read_length_normal;
        para_->kmerlength = config_.kmer_length;
    }

    /**
     * Read pre-computed OLC data from files
     */
    void readOLCFromFile() {
        if (context_->isMaster()) {
            cout << "Reading OLC from: " << config_.olc_input_path << endl;
        }

        for (int i = 3; i >= 3; i--) {
            if (context_->isMaster()) {
                cout << "Reading OLC file " << i << "..." << endl;
            }

            if (i == 3) {
                read_reads_info(temp_, config_.olc_input_path, i, 0);
            } else {
                read_reads_info(temp_, config_.olc_input_path, i, 1);
            }

            if (context_->isMaster()) {
                cout << "Reads loaded: " << temp_.size() << endl;
            }
        }
    }

    /**
     * Build OLC graph from reads
     */
    void buildOLCGraph() {
        if (context_->isMaster()) {
            cout << "Building OLC graph..." << endl;
        }

        get_pairs_Dtree(reads_, para_, config_.overlap_min, config_.overlap_max, Droot_);

        if (context_->isMaster()) {
            cout << "OLC graph built" << endl;
        }
    }

    /**
     * Extract and select largest subgraphs
     */
    void extractSubgraphs() {
        if (context_->isMaster()) {
            cout << "Extracting subgraphs..." << endl;
        }

        subgraph(sub_, reads_);
        largesub_ = large_sub(sub_, config_.subgraph_select_number);
        OLC_shrink(temp_, reads_, largesub_[0].node_list);

        reads_ = temp_;

        if (context_->isMaster()) {
            cout << "Subgraph extracted, size: " << temp_.size() << endl;
        }

        // Find root and leaf nodes
        root_.clear();
        leave_.clear();
        for (int i = 0; i < temp_.size(); i++) {
            if (temp_[i].predecessor.size() == 0 && temp_[i].successor.size() > 0) {
                root_.push_back(i);
            }
            if (temp_[i].successor.size() == 0 && temp_[i].predecessor.size() > 0) {
                leave_.push_back(i);
            }
        }

        if (context_->isMaster()) {
            cout << "Root nodes: " << root_.size() << endl;
            cout << "Leaf nodes: " << leave_.size() << endl;
        }
    }

    /**
     * Detect cycles and shrink them
     */
    void detectAndShrinkCycles() {
        if (context_->isMaster()) {
            cout << "Detecting and shrinking cycles..." << endl;
        }

        temp2_ = temp_;
        OLC_cyc_shrink_detail(temp2_, temp_, cyc_);
        OLC_cyc_sort(temp2_, temp_, cyc_);

        if (context_->isMaster()) {
            cout << "Cycles detected: " << cyc_.size() << endl;
        }
    }

    /**
     * Find paths and output results
     */
    void findPathsAndOutput() {
        if (context_->isMaster()) {
            cout << "Finding paths and generating output..." << endl;
        }

        root_.clear();
        for (int i = 0; i < temp2_.size(); i++) {
            if (temp2_[i].predecessor.size() == 0 && temp2_[i].successor.size() > 0) {
                root_.push_back(i);
            }
        }

        get_root_leave_OLC(temp2_, root_, leave_, 10, cyc_, results_);
        strain_print_reads(temp2_, temp_, cyc_, results_, config_.result_path, para_);

        if (context_->isMaster()) {
            cout << "Results written to: " << config_.result_path << endl;
        }
    }

    /**
     * Setup reads for parallel processing
     */
    void setupReadsForParallel() {
        cout << "Process " << context_->getRank() << " setting up reads..." << endl;

        int thick = 0;
        for (int i = 0; i < temp_.size(); i++) {
            for (int j = 0; j < temp_[i].successor.size(); j++) {
                temp_[i].next.push_back(temp_[i].successor[j]);
                if (temp_[i].successor[j] > temp_.size() - 1) {
                    cout << "Error: successor index out of range at " << i << endl;
                }
            }
            for (int j = 0; j < temp_[i].predecessor.size(); j++) {
                temp_[i].next.push_back(temp_[i].predecessor[j]);
                if (temp_[i].predecessor[j] > temp_.size() - 1) {
                    cout << "Error: predecessor index out of range at " << i << endl;
                }
            }
            if (temp_[i].read_count > 1) {
                thick++;
            }
            temp_[i].indegree = temp_[i].predecessor.size();
            temp_[i].outdegree = temp_[i].successor.size();
            temp_[i].length = temp_[i].sequence.size();
        }

        para_->rLengthNormal = temp_[0].sequence.size();
        para_->kmerlength = temp_[0].sequence.length();
        reads_ = temp_;

        cout << "Process " << context_->getRank() << " reads setup complete" << endl;
    }

    /**
     * Parallel k-mer bucketing
     */
    void parallelKmerBucketing() {
        int bucket_num = pow(4.0, config_.minimizer_size) - 1;

        if (context_->isMaster()) {
            cout << "Parallel k-mer bucketing with " << bucket_num + 1 << " buckets" << endl;
        }

        string unqdir = config_.workspace + "/unqtemp/temp_" + to_string(context_->getRank());
        string mkdir_cmd = "mkdir -p " + unqdir;
        system(mkdir_cmd.c_str());

        context_->barrier();

        // Process assigned buckets
        for (int j = 0; j <= bucket_num; j++) {
            if (j % context_->getSize() == context_->getRank()) {
                // bucket_remove_repeated_kmer_all(j, config_.workspace, para_, config_.minimizer_size, reads_);
            }
        }

        context_->barrier();
    }

    /**
     * Parallel DBG construction
     */
    void parallelDBGConstruction() {
        int bucket_num = pow(4.0, config_.minimizer_size) - 1;
        int* bucket_kmer_num = new int[bucket_num + 10];

        if (context_->isMaster()) {
            cout << "Collecting k-mer counts..." << endl;

            int count = 0;
            bucket_kmer_num[0] = 0;

            for (int j = 0; j < bucket_num + 1; j++) {
                string kmer_num_path = config_.workspace + "/unqtemp/bucket_num_" + to_string(j);
                ifstream numin(kmer_num_path);

                if (numin.is_open()) {
                    string line;
                    int num;
                    getline(numin, line);
                    stringstream ss(line);
                    ss >> num;
                    count += num;
                    bucket_kmer_num[j + 1] = count;
                    numin.close();
                }
            }
        }

        // Broadcast bucket counts to all processes
        context_->broadcast(bucket_kmer_num, bucket_num + 10, 0);
        context_->barrier();

        // Initialize DBG in parallel
        for (int j = 0; j < (bucket_num + 1) / 4; j++) {
            if (j % context_->getSize() == context_->getRank()) {
                cout << "Process " << context_->getRank() << " initializing bucket " << j << endl;
                bucket_dbg_init(j, config_.minimizer_size, config_.workspace, bucket_kmer_num, para_);
            }
        }

        context_->barrier();
        delete[] bucket_kmer_num;

        if (context_->isMaster()) {
            cout << "DBG construction complete" << endl;
        }
    }
};

#endif // PIPELINE_H
