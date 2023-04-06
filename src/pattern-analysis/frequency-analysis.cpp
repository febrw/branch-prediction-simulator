#include <stdexcept>
#include <iostream>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>
#include <unordered_map>
#include <vector>
#include <stdint.h>

#include "../include/json.hpp"
using json = nlohmann::json;

typedef struct {
    bool taken; // ground truth
	uint64_t index; // line occurence in trace file
} Posting;


int main(int argc, char * argv[])
{
	// Path setup
    std::string traces_path("/cs/studres/CS4202/Coursework/P2-BranchPredictor/branch_traces/");
    std::string path_to_trace_file = traces_path + "wrf.out";

	// File reading setup
    struct stat sb;
	uintmax_t counter = 0;
	int fd;
	const uintmax_t lineLength = 42;
	fd = open(path_to_trace_file.data(), O_RDONLY);
	fstat(fd, &sb);
	const char * data = static_cast<const char*>(mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0u));

	uint64_t branch_instr_counter {0};

    std::unordered_map<uint64_t, std::vector<Posting>> branch_frequencies; // positional inverted index

	// File reading loop
	while (counter < sb.st_size)
	{
		bool is_conditional_branch = *(data + counter + 38) == '1';
		uint64_t program_counter = strtoul(data + counter, nullptr, 16); // string to unsigned long, from base 16
		bool taken = *(data + counter + 40) == '1'; // string to unsigned long, from base 10

		if (!is_conditional_branch) {
			counter += lineLength;
			continue;
		}

		Posting p = {taken, branch_instr_counter};

		if (branch_frequencies.count(program_counter) == 0) {
			// create new vector of size 1, containing only p for now
			std::vector<Posting> posting_list;
			posting_list.push_back(p);
			branch_frequencies.emplace(program_counter, posting_list);
		} else {
			branch_frequencies.at(program_counter).push_back(p); // or add this posting to this postings list
		}

		++branch_instr_counter; // update at end, so first line is 0
		counter += lineLength;
	}
    

	// Results
	
	uint64_t unique_branch_count = branch_frequencies.size();

	json results = json::object();
	json branch_freqs_json = json::array();
	json branch_count = json::object();

	results.at("total_branch_instructions") = branch_instr_counter;
	results.at("unique_branch_addresses") = unique_branch_count;

	for (auto& b : branch_frequencies) {
		branch_freqs_json.push_back({b.first, b.second.size()});
	}

	results.at("branch_freqs") = branch_freqs_json;
	std::cout << results.dump(2);
}