#ifndef PIPELINE_SEQUENTIAL_H
#define PIPELINE_SEQUENTIAL_H

#include "pipeline_base.h"
#include "OLC_shrink.h"
#include <iostream>

using namespace std;

/**
 * Sequential pipeline implementation
 * Full OLC pipeline: FASTQ -> OLC -> Subgraph -> Cycle -> Assembly
 */
class SequentialPipeline : public PipelineBase {
public:
    SequentialPipeline(const Config& config, ExecutionContext* context)
        : PipelineBase(config, context) {}

    void execute() override {
        if (context_->isMaster()) {
            cout << "=== ViCURA Sequential Pipeline ===" << endl;
            config_.print();
        }

        begin_ = clock();

        // Step 1: Read input
        if (config_.build_olc && !config_.input_R1.empty() && !config_.input_R2.empty()) {
            readFASTQAndBuildOLC();
        } else if (config_.read_olc_from_file && !config_.olc_input_path.empty()) {
            readOLCFromFile();
        } else {
            cerr << "Error: No valid input specified." << endl;
            cerr << "Use --input-r1/r2 with --build-olc or --olc-input" << endl;
            return;
        }

        // Step 2: Build OLC graph
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

        end_ = clock();
        printSummary();
    }

private:
    /**
     * Read FASTQ files and build initial OLC
     */
    void readFASTQAndBuildOLC() {
        cout << "Reading FASTQ files..." << endl;
        cout << "  R1: " << config_.input_R1 << endl;
        cout << "  R2: " << config_.input_R2 << endl;

        reads_ = file_.file_read_Dtree_shrink(
            config_.input_R1,
            config_.input_R2,
            para_,
            1,
            Droot_,
            2
        );

        cout << "Reads loaded: " << reads_.size() << endl;

        para_->rLengthNormal = config_.read_length_normal;
        para_->kmerlength = config_.kmer_length;
    }

    /**
     * Read pre-computed OLC data from files
     */
    void readOLCFromFile() {
        cout << "Reading OLC from: " << config_.olc_input_path << endl;

        for (int i = 3; i >= 3; i--) {
            cout << "Reading OLC file " << i << "..." << endl;

            if (i == 3) {
                read_reads_info(temp_, config_.olc_input_path, i, 0);
            } else {
                read_reads_info(temp_, config_.olc_input_path, i, 1);
            }

            cout << "Reads loaded: " << temp_.size() << endl;
        }

        reads_ = temp_;
    }

    /**
     * Build OLC graph from reads
     */
    void buildOLCGraph() {
        cout << "Building OLC graph..." << endl;

        get_pairs_Dtree(reads_, para_, config_.overlap_min, config_.overlap_max, Droot_);

        cout << "OLC graph built" << endl;
    }

    /**
     * Extract and select largest subgraphs
     */
    void extractSubgraphs() {
        cout << "Extracting subgraphs..." << endl;

        subgraph(sub_, reads_);
        largesub_ = large_sub(sub_, config_.subgraph_select_number);
        OLC_shrink(temp_, reads_, largesub_[0].node_list);

        reads_ = temp_;

        cout << "Subgraph extracted, size: " << temp_.size() << endl;

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

        cout << "Root nodes: " << root_.size() << endl;
        cout << "Leaf nodes: " << leave_.size() << endl;
    }

    /**
     * Detect cycles and shrink them
     */
    void detectAndShrinkCycles() {
        cout << "Detecting and shrinking cycles..." << endl;

        temp2_ = temp_;
        OLC_cyc_shrink_detail(temp2_, temp_, cyc_);
        OLC_cyc_sort(temp2_, temp_, cyc_);

        cout << "Cycles detected: " << cyc_.size() << endl;
    }

    /**
     * Find paths and output results
     */
    void findPathsAndOutput() {
        cout << "Finding paths and generating output..." << endl;

        root_.clear();
        for (int i = 0; i < temp2_.size(); i++) {
            if (temp2_[i].predecessor.size() == 0 && temp2_[i].successor.size() > 0) {
                root_.push_back(i);
            }
        }

        get_root_leave_OLC(temp2_, root_, leave_, 10, cyc_, results_);
        strain_print_reads(temp2_, temp_, cyc_, results_, config_.result_path, para_);

        cout << "Results written to: " << config_.result_path << endl;
    }
};

#endif // PIPELINE_SEQUENTIAL_H
