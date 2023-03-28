#include "branch-predictor.hpp"
#include <math.h>

class TwoBitPredictor : public BranchPredictor
{
private:

    enum State {
        STRONG_NOT_TAKEN, // 00
        WEAK_NOT_TAKEN, // 01
        WEAK_TAKEN, // 10
        STRONG_TAKEN // 11
    };

    State * _table;
    uint64_t _bit_count;

public:

    TwoBitPredictor(uint64_t table_size)
    {
        _total_predictions = 0;
        _misspredictions = 0;
        _table = new State[table_size];
        _bit_count = static_cast<uint64_t>(log2(table_size));
    }

    void initTable(uint64_t table_size)
    {
        for (int i = 0; i < table_size; ++i)
        {
            _table[i] = WEAK_NOT_TAKEN;
        }
    }

    ~TwoBitPredictor()
    {
        delete[] _table;
    }

    void predict(uint64_t mem_address, bool ground_truth)
    {
        uint64_t index = mem_address << (64 - _bit_count) >> (64 - _bit_count);
        State state = _table[index];
        ++_total_predictions;
        switch (state)
        {
        case STRONG_NOT_TAKEN:
            if (ground_truth)
            {
                state = WEAK_NOT_TAKEN;
                ++_misspredictions;
            }
            break;

        case WEAK_NOT_TAKEN:
            if (ground_truth)
            {
                state = WEAK_TAKEN;
                ++_misspredictions;
            }
            else
            {
                state = STRONG_NOT_TAKEN;
            }
            break;

        case WEAK_TAKEN:
            if (ground_truth)
            {
                state = STRONG_TAKEN;
            }
            else
            {
                state = WEAK_NOT_TAKEN;
                ++_misspredictions;
            }
            break;

        case STRONG_TAKEN:
            if (!ground_truth)
            {
                state = WEAK_TAKEN;
                ++_misspredictions;
            }
            break;
        
        default:
            break;
        }
    } 
};