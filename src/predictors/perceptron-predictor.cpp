#include "branch-predictor.hpp"
#include <cstdint>
#include <vector>
#include <string>

class PerceptronPredictor : public BranchPredictor {

private:
    const int _history_length;
    const int _table_entries;
    const int _weight_bits;

    std::vector<std::vector<int>> _weights; // 2D vectror of integer weights, |table_entries| X |history length|
    std::vector<int> _history;

public:
    // _table_entries : number of perceptrons used, N
    // _history_length : number of bits used in the GHR, stores the number of previous branches remembered
    PerceptronPredictor(int history_length, int table_entries, int weight_bits)
        : _history_length(history_length),
          _table_entries(table_entries),
          _weight_bits(weight_bits),
          _weights(table_entries, std::vector<int>(history_length, 1 << (weight_bits - 1))), // each weight set to 
          _history(history_length, 0) // initialise length and zero out
    {
        _total_predictions = 0;
        _misspredictions = 0;
    }

    void predict(uint64_t program_counter, bool taken) override {
        int prediction = 0;
        
        for (int i = 0; i < _history_length; ++i) {
            int weight = _weights[program_counter % _table_entries][i]; // index into table of weights by prog counter, then into weight by history bit
            prediction += taken ? weight : -weight; // update prediction for each history bit
        }
        bool pred_taken = prediction >= 0;
        //if (pred_taken ) {std::cout << "nono\n";}

        if (pred_taken != taken) {
            ++_misspredictions;
            update(program_counter, taken);
        }

        ++_total_predictions;
    }

    void update(uint64_t program_counter, bool taken) {
        for (int i = 0; i < _history_length; i++) {
            int& weight = _weights[program_counter % _table_entries][i];
            int sign = taken ? 1 : -1;
            if ((sign * weight < ((1 << (_weight_bits - 1)) - 1)) || (sign < 0 && weight > 0)) {
                weight += sign * _history[i];
            }
        }
        _history.erase(_history.begin());
        _history.push_back(taken ? 1 : -1);
    }

    std::string get_name() const override {
        return "Perceptron Predictor";
    }
};