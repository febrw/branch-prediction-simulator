#include "branch-predictor.hpp"
#include <math.h>
#include <iostream>

class GSharePredictor : public BranchPredictor
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
    uint32_t _global_history;
    uint64_t _table_size;

public:

    GSharePredictor(uint64_t table_size)
    {
        _table_size = table_size; 
        _global_history = 0;
        _total_predictions = 0;
        _misspredictions = 0;
        _table = new State[table_size];
        _bit_count = static_cast<uint64_t>(log2(table_size));
        initTable(table_size);
    }

    void initTable(uint64_t table_size)
    {
        for (int i = 0; i < table_size; ++i)
        {
            _table[i] = WEAK_NOT_TAKEN;
        }
    }

    std::string get_name() const
    {
        return "GShare predictor, PHT size " + std::to_string(_table_size);
    }

    ~GSharePredictor()
    {
        delete[] _table;
    }

    void predict(uint64_t mem_address, bool taken)
    {
        // shift up global history bits, leave LSB unset (=0), top bits discarded.
        _global_history = (_global_history << (32 - _bit_count + 1) >> (32 - _bit_count));
        // if the branch was taken, update tge LSB to be set
        // otherwise, we can leave unset to 0 if the branch was not taken 
        if (taken) {
            _global_history |= 1;
            
        }
        //print_test_stats(taken);
        uint64_t index = (mem_address << (64 - _bit_count) >> (64 - _bit_count)) ^ _global_history; // keep bottom n bits only for table index
        State state = _table[index]; // get state

        ++_total_predictions; // increment prior to inspection, then increment misspredictions as they occur

        switch (state)
        {
        case STRONG_NOT_TAKEN:
            if (taken)
            {
                state = WEAK_NOT_TAKEN;
                ++_misspredictions;
            }
            break;

        case WEAK_NOT_TAKEN:
            if (taken)
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
            if (taken)
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
            if (!taken)
            {
                state = WEAK_TAKEN;
                ++_misspredictions;
            }
            break;
        
        default:
            std::cout << "Should not be here.\n";
            exit(0);
            break;
        }
    }

    void print_test_stats(bool taken) {
        std::cout << "Prediction number: " << _total_predictions << "\n";
        std::cout << "This prediction taken: " << taken << "\n";
        std::cout << "Global history: " << _global_history << "\n";
        std::cout << "Table size: " << _table_size  << "\n\n";
    } 
};