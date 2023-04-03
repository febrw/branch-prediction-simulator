#include "branch-predictor.hpp"

class AlwaysTakenPredictor : public BranchPredictor
{

public:

    AlwaysTakenPredictor()
    {
        _total_predictions = 0;
        _misspredictions = 0;
    }

    void predict(uint64_t program_counter, bool taken) override
    {
        ++_total_predictions;
        if (!taken) // if branch not taken
        {
            ++_misspredictions; // increment misspredictions, as we predicted taken
        }
    }

    std::string get_name() const override
    {
        return "Always Taken predictor";
    }

};