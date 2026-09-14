#ifndef PIPELINE_PARALLEL_H
#define PIPELINE_PARALLEL_H

#include "pipeline_base.h"
#include "bucket.h"
#include "SDBG.h"
#include <iostream>

using namespace std;

/**
 * Parallel pipeline implementation
 * Focused workflow: Pre-computed OLC -> Parallel K-mer -> DBG construction
 */
class ParallelPipeline : public PipelineBase {
public:
    ParallelPipeline(const Config& config, ExecutionContext* context)
        : PipelineBase(config, context) {}

    void execute() override {
        int my_rank = context_->getRank();

        if (context_->isMaster()) {
            cout << "=== ViCURA Parallel Pipeline (MPI) ===" << endl;
            cout << "Running with " << context_->getSize() << " processes" << endl;
            config_.print();
        }

        begin_ = clock();

        // Step 1: Read pre-computed OLC
        if (!config_.olc_input_path.empty()) {
            readOLCData();
        } else {
            if (my_rank == 0) {
                cerr << "Error: Parallel mode requires --olc-input path" << endl;
            }
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

        end_ = clock();

        if (context_->isMaster()) {
            cout << "Parallel pipeline completed" << endl;
            printSummary();
        }
    }

private:
    /**
     * Read pre-computed OLC data from files
     */
    void readOLCData() {
        int my_rank = context_->getRank();

        if (my_rank == 0) {
            cout << "Reading OLC from: " << config_.olc_input_path << endl;
        }

        for (int i = 3; i >= 3; i--) {
            cout << "[Process " << my_rank << "] Reading OLC file " << i << "..." << endl;

            if (i == 3) {
                read_reads_info(temp_, config_.olc_input_path, i, 0);
            } else {
                read_reads_info(temp_, config_.olc_input_path, i, 1);
            }

            cout << "[Process " << my_rank << "] Reads loaded: " << temp_.size() << endl;
        }
    }

    /**
     * Setup reads for parallel processing
     */
    void setupReadsForParallel() {
        int my_rank = context_->getRank();

        cout << "[Process " << my_rank << "] Setting up reads for parallel processing..." << endl;

        int thick = 0;
        for (int i = 0; i < temp_.size(); i++) {
            // Setup successor connections
            for (int j = 0; j < temp_[i].successor.size(); j++) {
                temp_[i].next.push_back(temp_[i].successor[j]);
                if (temp_[i].successor[j] > temp_.size() - 1) {
                    cout << "[Process " << my_rank << "] Error: successor index out of range at " << i << endl;
                }
            }

            // Setup predecessor connections
            for (int j = 0; j < temp_[i].predecessor.size(); j++) {
                temp_[i].next.push_back(temp_[i].predecessor[j]);
                if (temp_[i].predecessor[j] > temp_.size() - 1) {
                    cout << "[Process " << my_rank << "] Error: predecessor index out of range at " << i << endl;
                }
            }

            // Count thick reads
            if (temp_[i].read_count > 1) {
                thick++;
            }

            // Update degree information
            temp_[i].indegree = temp_[i].predecessor.size();
            temp_[i].outdegree = temp_[i].successor.size();
            temp_[i].length = temp_[i].sequence.size();
        }

        para_->rLengthNormal = temp_[0].sequence.size();
        para_->kmerlength = temp_[0].sequence.length();
        reads_ = temp_;

        cout << "[Process " << my_rank << "] Reads setup complete (thick reads: " << thick << ")" << endl;
    }

    /**
     * Parallel k-mer bucketing across MPI processes
     */
    void parallelKmerBucketing() {
        int my_rank = context_->getRank();
        int comm_sz = context_->getSize();
        int bucket_num = pow(4.0, config_.minimizer_size) - 1;

        if (my_rank == 0) {
            cout << "Parallel k-mer bucketing with " << (bucket_num + 1) << " buckets" << endl;
            cout << "Minimizer size: " << config_.minimizer_size << endl;
        }

        // Create process-specific temporary directory
        string unqdir = config_.workspace + "/unqtemp/temp_" + to_string(my_rank);
        string mkdir_cmd = "mkdir -p " + unqdir;
        system(mkdir_cmd.c_str());

        context_->barrier();

        // Distribute buckets across processes
        // Each process handles buckets where (bucket_id % comm_sz == my_rank)
        for (int j = 0; j <= bucket_num; j++) {
            if (j % comm_sz == my_rank) {
                cout << "[Process " << my_rank << "] Processing bucket " << j << endl;
                // bucket_remove_repeated_kmer_all(j, config_.workspace, para_, config_.minimizer_size, reads_);
            }
        }

        context_->barrier();

        if (my_rank == 0) {
            cout << "K-mer bucketing complete" << endl;
        }
    }

    /**
     * Parallel DBG construction
     */
    void parallelDBGConstruction() {
        int my_rank = context_->getRank();
        int comm_sz = context_->getSize();
        int bucket_num = pow(4.0, config_.minimizer_size) - 1;

        if (my_rank == 0) {
            cout << "Starting parallel DBG construction..." << endl;
        }

        // Allocate bucket k-mer count array
        int* bucket_kmer_num = new int[bucket_num + 10];

        // Master process collects k-mer counts from all buckets
        if (my_rank == 0) {
            cout << "Collecting k-mer counts from buckets..." << endl;

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
                } else {
                    // Bucket file doesn't exist, assume 0 k-mers
                    bucket_kmer_num[j + 1] = count;
                }
            }

            cout << "Total k-mers: " << count << endl;
        }

        // Broadcast bucket counts to all processes
        context_->broadcast(bucket_kmer_num, bucket_num + 10, 0);
        context_->barrier();

        // Initialize DBG in parallel
        // Distribute (bucket_num + 1) / 4 tasks across processes
        int total_tasks = (bucket_num + 1) / 4;

        for (int j = 0; j < total_tasks; j++) {
            if (j % comm_sz == my_rank) {
                cout << "[Process " << my_rank << "] Initializing DBG for bucket group " << j << endl;
                bucket_dbg_init(j, config_.minimizer_size, config_.workspace, bucket_kmer_num, para_);
            }
        }

        context_->barrier();

        // Cleanup
        delete[] bucket_kmer_num;

        if (my_rank == 0) {
            cout << "DBG construction complete" << endl;
        }
    }
};

#endif // PIPELINE_PARALLEL_H
