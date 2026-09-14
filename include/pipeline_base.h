#ifndef PIPELINE_BASE_H
#define PIPELINE_BASE_H

#include "config.h"
#include "execution_context.h"
#include "dbg.h"
#include <ctime>
#include <vector>
#include <string>

using namespace std;

/**
 * Base pipeline class with common functionality
 */
class PipelineBase {
protected:
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
    PipelineBase(const Config& config, ExecutionContext* context)
        : config_(config), context_(context) {
        para_ = new Parameter();
        Droot_ = new Dynamic_Tree();
        Droot2_ = new Dynamic_Tree();
    }

    virtual ~PipelineBase() {
        delete para_;
        delete Droot_;
        delete Droot2_;
    }

    /**
     * Execute the pipeline - to be implemented by subclasses
     */
    virtual void execute() = 0;

protected:
    /**
     * Print execution summary
     */
    void printSummary() {
        if (context_->isMaster()) {
            double elapsed = double(end_ - begin_) / CLOCKS_PER_SEC;
            cout << "Pipeline completed in " << elapsed << " seconds" << endl;
        }
    }
};

#endif // PIPELINE_BASE_H
