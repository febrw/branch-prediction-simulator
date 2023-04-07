#include "branch-predictor.hpp"
#include <cstdint>
#include <vector>
#include <string>

class HybridPerceptronPredictor : public BranchPredictor {

private:
    const int _history_length; // should be roughly half the global
    const int _table_entries; // a power of 2
    const int _weight_bits;

    std::vector<std::vector<int>> _perceptron_table; // 2D vector of integer weights, |table_entries| X |history length|
    std::vector<std::vector<int>> _local_histories; // 2D table of local histories, indexed into by PC
    std::vector<int> _global_history;

    
public:
    // _table_entries : number of perceptrons used, N
    // _history_length : number of bits used in the GHR, stores the number of previous branches remembered
    HybridPerceptronPredictor(int history_length, int table_entries, int weight_bits)
        : _history_length(history_length),
          _table_entries(table_entries),
          _weight_bits(weight_bits),
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
        const auto & table_index = program_counter & (_table_entries - 1);
        auto& perceptron = _perceptron_table.at(table_index); // index into perceptron table
        int y = perceptron.at(0); // setup output of perceptron to bias weight

        std::vector<int> input_vector(_global_history); // copy global history into new input vector
        // concat global history with local history
        input_vector.insert(input_vector.end(), _local_histories.at(table_index).begin(), _local_histories.at(table_index).end());

        for (int i = 0; i < 2 * _history_length; ++i) {
            int& weight = perceptron.at(i + 1); // +1 indexing as perceptron vector is one elem larger than Hybrid history vector
            y += weight * input_vector.at(i); // dot product line, concat global and local history
        }
        
        bool prediction = y >= 0;
        int t = taken ? 1 : -1;

        // update perceptron is misspredict
        if (prediction != taken) {
            ++_misspredictions;
            _perceptron_table.at(table_index).at(0) += t; // update bias term
            
            for (int i = 0; i < 2 * _history_length; ++i) {
                int& weight = _perceptron_table.at(table_index).at(i+1); // get weight, +1 index for bias weight
                weight += t * input_vector.at(i);
            }
        }

        
        _global_history.erase(_global_history.begin()); // update global history vector
        _global_history.push_back(taken ? 1 : -1);

        _local_histories.at(table_index).erase(_local_histories.at(table_index).begin()); // and local history vector
        _local_histories.at(table_index).push_back(taken ? 1 : -1);
        ++_total_predictions;
    }


    std::string get_name() const override {
        return "Hybrid Perceptron Predictor";
    }


};
