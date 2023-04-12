#ifndef BRANCH_PREDICTOR_HPP
#define BRANCH_PREDICTOR_HPP

#include <stdint.h>
#include <string>

class BranchPredictor
{

protected:

    uint64_t _total_predictions;
    uint64_t _misspredictions;

public:
    // program_counter : a program counter of a conditional branch instruction
    // taken: true if the branch was taken, false if not taken 
    virtual void predict(uint64_t program_counter, bool taken) = 0;

    uint64_t get_total_predictions() const
    {
        return _total_predictions;
    }

    uint64_t get_misspredictions() const
    {
        return _misspredictions;
    }

    virtual double get_accuracy() const
    {
        return 100.00 - get_misspredictions();
    }

    virtual double get_missprediction_rate() const
    {
        if (_total_predictions == 0) {
            return 0.0;
        }
        return (_misspredictions / static_cast<double>(_total_predictions)) * 100;
    }

    virtual std::string get_name() const = 0;

    // can be used for profiled predictors, to retain data structures, histories, weights etc
    // but restart prediction statistics
    void reset_stats() 
    {
        _misspredictions = 0;
        _total_predictions = 0;
    }
    
};

#endif /* BRANCH_PREDICTOR_HPP */



