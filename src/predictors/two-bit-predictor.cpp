#include "branch-predictor.hpp"
#include <math.h>
#include <iostream>

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
    uint64_t _table_size;

public:

    TwoBitPredictor(uint64_t table_size)
    {
        _table_size = table_size;
        _total_predictions = 0;
        _misspredictions = 0;
        _table = new State[table_size];
        _bit_count = static_cast<uint64_t>(log2(table_size));
        initTable(table_size);
    }

    void initTable(uint64_t table_size)
    {
        for (uint64_t i = 0; i < table_size; ++i)
        {
            _table[i] = STRONG_TAKEN;
        }
    }

    ~TwoBitPredictor()
    {
        delete[] _table;
    }

    std::string get_name() const override
    {
        return "Two-Bit predictor, PHT size " + std::to_string(_table_size);
    }

    void predict(uint64_t program_counter, bool taken) override
    {
        uint64_t index = program_counter & (_table_size - 1); // mask to keep bottom n bits only for table index
        State& state = _table[index]; // get state
        ++_total_predictions;
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
        //_table[index] = state;
    } 
};