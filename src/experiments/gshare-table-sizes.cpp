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
#include "../predictors/gshare-predictor.cpp"

#include "../include/json.hpp"
using json = nlohmann::json;
extern "C" unsigned long hex2ull(const char *ptr);


int main(int argc, char * argv[])
{

	// Setup file path, predictor, and JSON output structures
    std::string traces_path("/cs/studres/CS4202/Coursework/P2-BranchPredictor/branch_traces/");

    GSharePredictor * gshare_predictors[] = {
        new GSharePredictor(512),
		new GSharePredictor(1024),
		new GSharePredictor(2048),
		new GSharePredictor(4096),
    };

    json output = json::array(); // global output object
    json file_summary = json::object(); // each result for a file

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
			uint64_t program_counter = hex2ull(data + counter); // string to unsigned long, from base 16
			bool taken = *(data + counter + 40) == '1'; // string to unsigned long, from base 10

			if (is_conditional_branch) {
                for (BranchPredictor * bp : gshare_predictors) {
                    bp -> predict(program_counter, taken);
                }
			}
			counter += lineLength;
		}

        // Format results
        file_summary["file_name"] = file_path.substr(file_path.find_last_of("/") + 1);
        json results_for_file = json::array();
        for (GSharePredictor * bp : gshare_predictors) {
            json result = json::object();
            result["table_size"] = bp -> get_table_size();
            result["missprediction_rate"] = bp -> get_missprediction_rate();
            results_for_file.push_back(result);
        }

        file_summary["outputs"] = results_for_file;
        output.push_back(file_summary);
	}

	std::cout << output.dump(2);

}