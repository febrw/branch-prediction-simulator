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
#include <algorithm> // std::sort
#include "../predictors/always-taken-predictor.cpp"

#include "../include/json.hpp"
using json = nlohmann::json;
extern "C" unsigned long hex2ull(const char *ptr);


typedef struct {
    uint64_t    total_branches, unique_branches,
                forward_branches, backward_branches,
                forward_taken, backward_taken,
                direct_branches, hapax_branches; 
} FileResult;

// proportion of unique branch addresses which account for prop% of all branch occurences
double get_branch_bias_proportion(const std::unordered_map<uint64_t, std::vector<int>>& branch_results, uint64_t total_branch_count, double prop)
{
    // PC to branch count
    std::vector<std::pair<uint64_t, uint64_t>> branch_counts;
    for (auto & res : branch_results)
    {
        uint64_t count = res.second.size();
        branch_counts.push_back(std::make_pair(res.first, count));
    }

    assert(branch_counts.size() == branch_results.size());

    std::sort(branch_counts.begin(), branch_counts.end(), [](auto &left, auto &right) {
        return left.second > right.second;
    });

    int maj_branch_count = prop * total_branch_count;
    int c{0}; // num of branches used

    for (auto& e : branch_counts) {
        if (maj_branch_count < 0) {break;}
        c++;
        maj_branch_count -= e.second;
    }
    
    return 100.0 * c / static_cast<double>(branch_results.size());
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


        // File results setup
        FileResult result{};


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
            bool is_forward_branch = program_counter < target_address; // only valid variable if is_direct_branch is true


            if (!is_conditional_branch) {
                counter += lineLength;
                continue;
            }

            // Update results index
            if (branch_results.count(program_counter) == 0) {
                std::vector<int> results_for_branch = {branch_result};
                branch_results.emplace(program_counter, results_for_branch);
            } else {
                branch_results.at(program_counter).push_back(branch_result);
            }

            // update forward_taken_info
            if (is_direct_branch) {
                ++ result . direct_branches;
                if (is_forward_branch) {
                    ++ result . forward_branches;
                    if (taken) {++ result . forward_taken;}
                     
                } else {
                    ++ result . backward_branches;
                    if (taken) {++ result . backward_taken;}
                }   
            }

            ++ result . total_branches;
			counter += lineLength;
		}

        uint64_t hapax{0};
        for (auto& e : branch_results) 
        { 
            if (e.second.size() == 1) {++hapax;}
        }

        result . unique_branches = branch_results.size();
        result . hapax_branches = hapax;
        //result . branch_bias_proportion = get_branch_bias_proportion(branch_results, result . total_branches);
        // Write to JSON
        json file_result = json::object(); // each result for a file
        file_result["file_name"] = file_path.substr(file_path.find_last_of("/") + 1);
        file_result["total_branches"] = result . total_branches;
        file_result["unique_branches"] = result . unique_branches;
        file_result["forward_branches"] = result . forward_branches;
        file_result["backward_branches"] = result . backward_branches;
        file_result["forward_taken"] = result . forward_taken;
        file_result["backward_taken"] = result . backward_taken;
        file_result["hapax_branches"] = result . hapax_branches;
        file_result["direct_branches"] = result . direct_branches;
        file_result["branch_bias_prop.95"] = get_branch_bias_proportion(branch_results, result . total_branches, 0.95);
        file_result["branch_bias_prop.99"] = get_branch_bias_proportion(branch_results, result . total_branches, 0.99);
        file_result["branch_bias_prop.999"] = get_branch_bias_proportion(branch_results, result . total_branches, 0.999);
        global_file_list.push_back(file_result);
	}

    std::cout << global_file_list.dump(2);
}