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

	// Path setup
    std::string traces_path("/cs/studres/CS4202/Coursework/P2-BranchPredictor/branch_traces/");

	BranchPredictor * predictors[] = {
		new TwoBitPredictor(),
		new PerceptronPredictor(12, 1 << 14, 10),
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
			uint64_t program_counter = strtoul(data + counter, nullptr, 16); // string to unsigned long, from base 16
			bool taken = *(data + counter + 40) == '1'; // string to unsigned long, from base 10

			if (is_conditional_branch) {
				for (PerceptronPredictor * bp : predictors)
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

        result_for_file["results_before_training"] = predictor_results_list;

        counter = 0;
        // File reading loop

        // reset prediction stats for next run
        for (PerceptronPredictor * bp : predictors) { bp -> reset_stats();}
		while (counter < sb.st_size)
		{
			bool is_conditional_branch = *(data + counter + 38) == '1';
			uint64_t program_counter = strtoul(data + counter, nullptr, 16); // string to unsigned long, from base 16
			bool taken = *(data + counter + 40) == '1'; // string to unsigned long, from base 10

			if (is_conditional_branch) {
				for (PerceptronPredictor * bp : predictors)
				{
					bp -> predict(program_counter, taken);
				}
			}
			counter += lineLength;
		}

		json predictor_results_list_after = json::array();
		// print results
		for (BranchPredictor * bp : predictors)
		{
			json result_for_predictor = json::object();
			result_for_predictor["name"] = bp -> get_name();
			result_for_predictor["missprediction_rate"] = bp -> get_missprediction_rate();
			predictor_results_list_after.push_back(result_for_predictor);
		}

		result_for_file["results_after_training"] = predictor_results_list_after;
		output.push_back(result_for_file);
	}

	std::cout << output.dump(2) << '\n';
}