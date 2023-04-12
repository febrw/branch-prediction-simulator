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
#include <vector>
#include <unordered_map>
#include "../include/json.hpp"

using json = nlohmann::json;
extern "C" unsigned long hex2ull(const char *ptr);


// Measures the proportion of outcome switches for a branch's history
// A history consisting of entirely taken branches has a volatility of 0
// {0, 0, 0, 1, 1, 1} -> 0.2 
// Out of 5 transitions from one results to the next, only 1 was a switch from the previous outcome
// Giving a volatility of 1.0 / 5.0
// {0, 1, 0, 1, 0, 1} -> 1
double branch_volatility(const std::vector<int>& results)
{
    if (results.size() <= 1) {return 1.0;}
    
    int flip_flops{0};
    for (auto it = results.begin(); it != results.end() - 1; ++it)
    {
        int curr = *it, next = *(it+1);
        if (next != curr) {++flip_flops;}
    }
    
    return flip_flops / static_cast<double>(results.size() - 1);
}

int main(int argc, char * argv[])
{

	// Setup file path, predictor, and JSON output structures
    std::string traces_path("/cs/studres/CS4202/Coursework/P2-BranchPredictor/branch_traces/");   
    
    json global_file_list = json::array();

	for (const auto & f : std::filesystem::directory_iterator(traces_path)) {
		// Get file path
		const std::string& file_path = f.path();

		// File reading setup
		struct stat sb;
		uintmax_t counter = 0;
		int fd;
		const uintmax_t lineLength = 42;
		fd = open(file_path.data(), O_RDONLY);
		fstat(fd, &sb);
		const char * data = static_cast<const char*>(mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0u));


        // Maps branch addresses to branch results list
        std::unordered_map<uint64_t, std::vector<int>> branch_results;



		// File reading loop
		while (counter < sb.st_size)
		{
            char branch_type = *(data + counter + 34);
            bool is_direct_branch = *(data + counter + 36) == '1';
            bool is_conditional_branch = *(data + counter + 38) == '1';
            bool taken = *(data + counter + 40) == '1';
            int branch_result = taken ? 1 : -1;
			uint64_t program_counter = hex2ull(data + counter);
            uint64_t target_address = hex2ull(data + counter + 17);
            bool forward_branch = program_counter < target_address; // only valid variable if is_direct_branch is true


            if (!is_conditional_branch) {counter += lineLength; continue;}

            // Update results index
            if (branch_results.count(program_counter) == 0) {
                std::vector<int> results_for_branch = {branch_result};
                branch_results.emplace(program_counter, results_for_branch);
            } else {
                branch_results.at(program_counter).push_back(branch_result);
            }
			counter += lineLength;
		}

        // After file reading
        json file_result = json::object(); // each result for a file
        json volatilities = json::array();
        // Write to JSON
        for (auto& e : branch_results)
        {
            volatilities.push_back(branch_volatility(e.second));
        }
        file_result["volatilities"] = volatilities;
        file_result["file_name"] = file_path.substr(file_path.find_last_of("/") + 1);
        global_file_list.push_back(file_result);
	}

    std::cout << global_file_list.dump(2);
}