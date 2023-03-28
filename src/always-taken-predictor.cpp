#include "branch-predictor.hpp"

class AlwaystakenPredictor : public BranchPredictor
{

public:

    AlwaystakenPredictor()
    {
        _total_predictions = 0;
        _misspredictions = 0;
    }

    void predict(uint64_t mem_address, bool ground_truth)
    {
        ++_total_predictions;
        if (!ground_truth) // if branch not taken
        {
            ++_misspredictions; // increment misspredictions, as we predicted taken
        }
    }
};