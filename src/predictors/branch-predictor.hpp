#ifndef BRANCH_PREDICTOR_HPP
#define BRANCH_PREDICTOR_HPP

#include <stdint.h>

class BranchPredictor
{

protected:

    uint64_t _total_predictions;
    uint64_t _misspredictions;

public:
    // mem_address : a program counter of a conditional branch instruction
    // taken: true if the branch was taken, false if not taken 
    virtual void predict(uint64_t mem_address, bool taken) = 0;

    uint64_t get_total_predictions() const
    {
        return _total_predictions;
    }

    uint64_t get_misspredictions() const
    {
        return _misspredictions;
    }

    double get_accuracy() const
    {
        return (1.0 - (_misspredictions / (static_cast<double>(_total_predictions)))) * 100;
    }

    virtual std::string get_name() const = 0;
    
};

#endif /* BRANCH_PREDICTOR_HPP */