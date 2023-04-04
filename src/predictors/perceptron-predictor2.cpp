#include "branch-predictor.hpp"
#include <string>
#include <vector>
#include <cmath>


class PerceptronBranchPredictor : public BranchPredictor {
public:
    PerceptronBranchPredictor(int history_length, int table_entries, int weight_bits)
        : _history_length(history_length), _table_entries(table_entries), _weight_bits(weight_bits),
          _table_mask(table_entries - 1), _weights(table_entries, std::vector<int>(history_length, 0)),
          _bias(table_entries, 1 << (weight_bits - 1)) {}

    void predict(uint64_t program_counter, bool taken) override {
        _total_predictions++;
        std::vector<int>& perceptron = _weights[program_counter & _table_mask]; // step 1
        int prediction = _bias[program_counter & _table_mask];
        for (int i = 0; i < _history_length; i++) {
            int weight = perceptron[i];
            prediction += taken ? weight : -weight;
        }
        bool correct = (taken && prediction >= 0) || (!taken && prediction < 0);
        if (!correct) {
            _misspredictions++;
            for (int i = 0; i < _history_length; i++) {
                int weight = perceptron[i];
                if (taken) {
                    perceptron[i] = std::min(weight + 1, (1 << (_weight_bits - 1)) - 1);
                } else {
                    perceptron[i] = std::max(weight - 1, -(1 << (_weight_bits - 1)));
                }
            }
            _bias[program_counter & _table_mask] = std::min(_bias[program_counter & _table_mask] + (taken ? 1 : -1), (1 << (_weight_bits - 1)) - 1);
        }
    }

    double get_accuracy() const override {
        return (1.0 - (_misspredictions / static_cast<double>(_total_predictions))) * 100;
    }

    std::string get_name() const override {
        return "Perceptron";
    }

private:
    int _history_length;
    int _table_entries;
    int _weight_bits;
    int _table_mask;
    std::vector<std::vector<int>> _weights;
    std::vector<int> _bias;
};
