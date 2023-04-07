#include "branch-predictor.hpp"
#include <cstdint>
#include <vector>
#include <string>

class GSharePerceptronPredictor : public BranchPredictor {

private:
    const int _history_length;
    const int _table_entries; // a power of 2

    std::vector<std::vector<int>> _perceptron_table; // 2D vector of integer weights, |table_entries| X |history length|
    std::vector<std::vector<int>> _local_histories; // 2D table of local histories, indexed into by PC
    std::vector<int> _global_history;

    uint64_t history_to_num() {
        uint64_t out {0};
        std::vector<int> binary_vector(_global_history);

        for (auto it = binary_vector.begin(); it != binary_vector.end(); ++it) {
            if (*it == 1) {
                out += 1 << (binary_vector.end() - it - 1);
            }
        }

        return out;
    }

    
public:
    // _table_entries : number of perceptrons used, N
    // _history_length : number of bits used in the GHR, stores the number of previous branches remembered
    GSharePerceptronPredictor(int history_length, int table_entries)
        : _history_length(history_length),
          _table_entries(table_entries),
          // each weight set to 0, we need +1 size for a bias term
          // now multiply by 2 due to concat of local and global history
          // still +1 for bias term
          _perceptron_table(table_entries, std::vector<int>(2 * history_length + 1, 0)),
          _global_history(history_length, 0), // initialise length and zero out
          _local_histories(table_entries, std::vector<int>(history_length, 0))
    {
        _total_predictions = 0;
        _misspredictions = 0;

        for (auto perceptron : _perceptron_table) {
            perceptron.at(0) = 1; // setup bias weight
        }
    }

    void predict(uint64_t program_counter, bool taken) override {
        uint64_t g_index = history_to_num();
        uint64_t table_index = program_counter & (_table_entries - 1);
        table_index ^= g_index; // // XOR with global history to go to local perceptron

        auto& perceptron = _perceptron_table.at(table_index); // index into perceptron table
        int y = perceptron.at(0); // setup output of perceptron to bias weight

        for (int i = 0; i < _history_length; ++i) {
            int& weight = perceptron.at(i + 1); // +1 indexing as perceptron vector is one elem larger than Hybrid history vector
            y += weight * _local_histories.at(table_index).at(i); // dot product line, concat global and local history
        }
        
        bool prediction = y >= 0;
        int t = taken ? 1 : -1;

        // update perceptron is misspredict
        if (prediction != taken) {
            ++_misspredictions;
            _perceptron_table.at(table_index).at(0) += t; // update bias term
            
            for (int i = 0; i < _history_length; ++i) {
                int& weight = _perceptron_table.at(table_index).at(i+1); // get weight, +1 index for bias weight
                weight += t * _local_histories.at(table_index).at(i);
            }
        }

        _global_history.erase(_global_history.begin()); // update global history vector
        _global_history.push_back(taken ? 1 : -1);

        _local_histories.at(table_index).erase(_local_histories.at(table_index).begin()); // and local history vector
        _local_histories.at(table_index).push_back(taken ? 1 : -1);
        ++_total_predictions;
    }


    std::string get_name() const override {
        return "GShare Perceptron Predictor";
    }


};
