#ifndef BRANCH_PREDICTOR_HPP
#define BRANCH_PREDICTOR_HPP

#include <stdint.h>

class BranchPredictor
{
protected:
    uint64_t _total_predictions;
    uint64_t _misspredictions;

public:
    virtual void predict(uint64_t mem_address, bool ground_truth) = 0;
};

#endif /* BRANCH_PREDICTOR_HPP */