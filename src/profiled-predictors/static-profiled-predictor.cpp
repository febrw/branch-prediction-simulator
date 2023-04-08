#include <stdexcept>
#include <iostream>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>
#include <filesystem>
#include <string>
#include "../predictors/branch-predictor.hpp"

#include "../include/json.hpp"
using json = nlohmann::json;
extern "C" unsigned long hex2ull(const char *ptr);

class StaticProfiledPredictor : public BranchPredictor

{
private:

    double _forward_branches, _backward_branches;
    double _forward_taken, _backward_taken;
    bool _forward_pred, _backward_pred, _unk_pred;    

public:

    StaticProfiledPredictor(std::string file_path):
        _forward_branches(0),
        _backward_branches(0),
        _forward_taken(0),
        _backward_taken(0)
    {
        _misspredictions = 0;
        _total_predictions = 0;
        // File reading setup
		struct stat sb;
		uintmax_t counter = 0;
		int fd;
		const uintmax_t lineLength = 42;
		fd = open(file_path.data(), O_RDONLY);
		fstat(fd, &sb);
		const char * data = static_cast<const char*>(mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0u));

        while (counter < sb.st_size)
		{
			bool is_conditional_branch = *(data + counter + 38) == '1';
            bool is_direct_branch = *(data + counter + 36) == '1';
            bool taken = *(data + counter + 40) == '1';
			uint64_t program_counter = hex2ull(data + counter);
            uint64_t target_address = hex2ull(data + counter + 17);

			if (!(is_conditional_branch && is_direct_branch)) {counter += lineLength;}
                
            if (program_counter < target_address) {
                ++_forward_branches;
                if (taken) {++_forward_taken;}
            } else {
                ++_backward_branches;
                if (taken) {++_backward_taken;}
            }
			counter += lineLength;
		}

        // if we do not know branch direction, use prediction for whichever branch type is more probable
        _forward_pred = (get_forward_taken_proportion() > 0.5);
        _backward_pred = (get_backward_taken_proportion() > 0.5);
        _unk_pred = (get_forward_proportion() > 0.5) ? _forward_pred : _backward_pred;
    }
    
    double get_forward_taken_proportion() const
    {
        return _forward_taken / static_cast<double>(_forward_branches);
    }

    double get_backward_taken_proportion() const
    {
        return _backward_taken / static_cast<double>(_backward_branches);
    }

    double get_forward_proportion() const
    {
        return _forward_branches / (static_cast<double>(_forward_branches) + _backward_branches);
    }

    void predict(uint64_t info, bool taken) override
    {
        
        bool pred;
        switch (info) {
            case 0:
                pred = _unk_pred;
                break;
            case 1:
                pred = _forward_pred;
                break;
            case 2:
                pred = _backward_pred;
                break;
            default:
                std::cout << "Bad switch statement\n";
        }

        if (pred != taken) {
            ++_misspredictions;
            
        }
        ++_total_predictions;
    }

    std::string get_name() const override
    {
        return "Static Profiled Predictor";
    }

};


