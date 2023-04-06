#include "branch-predictor.hpp"
#include <math.h>
#include <iostream>
#include "state.hpp"


class GSharePredictor : public BranchPredictor
{
private:
    State * _table;
    uint64_t _bit_count;
    uint32_t _global_history;
    uint64_t _table_size;

public:

    GSharePredictor(uint64_t table_size) :
        _table_size(table_size), 
        _global_history(0),
        _table(new State[table_size]),
        _bit_count(static_cast<uint64_t>(log2(table_size)))
    {
        initTable();
    }

    void initTable()
    {
        for (uint64_t i = 0; i < _table_size; ++i)
        {
            _table[i] = STRONG_TAKEN;
        }
    }

    std::string get_name() const override
    {
        return "GShare predictor, PHT size " + std::to_string(_table_size);
    }

    ~GSharePredictor()
    {
        delete[] _table;
    }

    void predict(uint64_t program_counter, bool taken) override
    {
        
        //print_test_stats(taken);
        uint64_t index = (program_counter << (64 - _bit_count) >> (64 - _bit_count)) ^ _global_history; // keep bottom n bits only for table index

        State& state = _table[index]; // get state

        ++_total_predictions; // increment prior to inspection, then increment misspredictions as they occur

        switch (state) {
            case STRONG_NOT_TAKEN:
                if (taken) {
                    state = WEAK_NOT_TAKEN;
                    ++_misspredictions;
                }
                break;

            case WEAK_NOT_TAKEN:
                if (taken) {
                    state = WEAK_TAKEN;
                    ++_misspredictions;
                } else {
                    state = STRONG_NOT_TAKEN;
                }
                break;

            case WEAK_TAKEN:
                if (taken) {
                    state = STRONG_TAKEN;
                } else {
                    state = WEAK_NOT_TAKEN;
                    ++_misspredictions;
                }
                break;

            case STRONG_TAKEN:
                if (!taken) {
                    state = WEAK_TAKEN;
                    ++_misspredictions;
                }
                break;
            
            default:
                std::cout << "Should not be here.\n";
                exit(0);
                break;
        }
        // update global history
        _global_history = ((_global_history << 1) | taken) & ((1 << _bit_count) - 1);
        //_table[index] = state;
    }

    void print_test_stats(bool taken) {
        std::cout << "Prediction number: " << _total_predictions << "\n";
        std::cout << "This prediction taken: " << taken << "\n";
        std::cout << "Global history: " << _global_history << "\n";
        std::cout << "Table size: " << _table_size  << "\n\n";
    }

};