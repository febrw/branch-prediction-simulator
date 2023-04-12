#include "branch-predictor.hpp"
#include <cstdint>
#include <vector>
#include <string>
#include <cmath> // floor
class GlobalPerceptronPredictor : public BranchPredictor {

private:
    const int _history_length;
    const int _table_entries;


    std::vector<std::vector<int>> _perceptron_table; // 2D vectror of integer weights, |table_entries| X |history length|
    std::vector<int> _history;
    
public:
    // _table_entries : number of perceptrons used, N
    // _history_length : number of bits used in the GHR, stores the number of previous branches remembered
    GlobalPerceptronPredictor(int history_length, int table_entries)
        : _history_length(history_length),
          _table_entries(table_entries),
          _perceptron_table(table_entries, std::vector<int>(history_length + 1, 0)), // each weight set to 0, we need +1 size for a bias term
          _history(history_length, 0) // initialise length and zero out
          
    {
        _total_predictions = 0;
        _misspredictions = 0;

        for (auto perceptron : _perceptron_table) {
            perceptron.at(0) = 1; // setup bias weight
        }
    }

    void predict(uint64_t program_counter, bool taken) override {
        auto& perceptron = _perceptron_table.at(program_counter & (_table_entries-1)); // index into perceptron table
        int y = perceptron.at(0); // setup output of perceptron

        for (int i = 0; i < _history_length; ++i) {
            int& weight = perceptron.at(i + 1); // +1 indexing as perceptron vector is one elem larger than global history vector
            y += weight * _history.at(i); // dot product line
        }
        
        bool prediction = y >= 0;
        int t = taken ? 1 : -1;

        // update perceptron is misspredict
        if (prediction != taken) {
            ++_misspredictions;
            _perceptron_table.at(program_counter & (_table_entries-1)).at(0) += t; // update bias term
            for (int i = 0; i < _history_length; ++i) {
                int& weight = _perceptron_table.at(program_counter & (_table_entries-1)).at(i+1); // get weight, +1 index for bias weight
                weight += t * _history.at(i);
            }
        }
        
        _history.erase(_history.begin()); // update history vector
        _history.push_back(taken ? 1 : -1);
        ++_total_predictions;
    }


    std::string get_name() const override {
        return "Global Perceptron Predictor. Table Size: " + std::to_string(_table_entries) + " , History Length (Bits): " + std::to_string(_history_length);
    }


};
