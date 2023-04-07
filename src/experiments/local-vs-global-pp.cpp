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
#include "../predictors/global-perceptron-predictor.cpp"
#include "../predictors/hybrid-perceptron-predictor.cpp"
#include "../predictors/local-perceptron-predictor.cpp"

#include "../include/json.hpp"
using json = nlohmann::json;
extern "C" unsigned long hex2ull(const char *ptr);


int main(int argc, char * argv[])
{

	// Path setup
    std::string traces_path("/cs/studres/CS4202/Coursework/P2-BranchPredictor/branch_traces/");

	BranchPredictor * predictors[] = {
		new GlobalPerceptronPredictor(8, 1 << 12),
		new GlobalPerceptronPredictor(12, 1 << 14),
		new LocalPerceptronPredictor(8, 1 << 12),
		new LocalPerceptronPredictor(12, 1 << 14)
	};

	json output = json::array();
	json result_for_file = json::object();
	
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

		// File reading loop
		while (counter < sb.st_size)
		{
			bool is_conditional_branch = *(data + counter + 38) == '1';
			uint64_t program_counter = hex2ull(data + counter); // string to unsigned long, from base 16
			bool taken = *(data + counter + 40) == '1'; // string to unsigned long, from base 10

			if (is_conditional_branch) {
				for (BranchPredictor * bp : predictors)
				{
					bp -> predict(program_counter, taken);
				}
			}
			counter += lineLength;
		}

		result_for_file["file_name"] = file_path.substr(file_path.find_last_of("/") + 1);
		json predictor_results_list = json::array();
		// print results
		for (BranchPredictor * bp : predictors)
		{
			json result_for_predictor = json::object();
			result_for_predictor["name"] = bp -> get_name();
			result_for_predictor["missprediction_rate"] = bp -> get_missprediction_rate();
			predictor_results_list.push_back(result_for_predictor);
		}

		result_for_file["results"] = predictor_results_list;
		output.push_back(result_for_file);
	}

	std::cout << output.dump(2) << '\n';
}