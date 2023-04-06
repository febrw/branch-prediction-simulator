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
#include "../predictors/two-bit-predictor.cpp"
#include "../predictors/always-taken-predictor.cpp"
#include "../predictors/gshare-predictor.cpp"
#include "../predictors/perceptron-predictor.cpp"

#include "../include/json.hpp"
using json = nlohmann::json;

int main(int argc, char * argv[])
{

	// Setup file path, predictor, and JSON output structures
    std::string traces_path("/cs/studres/CS4202/Coursework/P2-BranchPredictor/branch_traces/");

    BranchPredictor * always_taken = new AlwaysTakenPredictor();

   
    json result = json::object(); // each result for a file
    json arr = json::array();

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

		// Run loop

		// File reading loop
		while (counter < sb.st_size)
		{
			bool is_conditional_branch = *(data + counter + 38) == '1';
			uint64_t program_counter = strtoul(data + counter, nullptr, 16); // string to unsigned long, from base 16
			bool taken = *(data + counter + 40) == '1'; // string to unsigned long, from base 10

			if (is_conditional_branch) {
			    always_taken -> predict(program_counter, taken);	
			}
			counter += lineLength;
		}

		// print results
        result["file_name"] = file_path.substr(file_path.find_last_of("/") + 1);
		result["missprediction_rate"] = always_taken -> get_missprediction_rate();
        arr.push_back(result);
	}

	std::cout << arr.dump(2);

}