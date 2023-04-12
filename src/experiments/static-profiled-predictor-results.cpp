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
#include "../profiled-predictors/static-profiled-predictor.cpp"

#include "../include/json.hpp"
using json = nlohmann::json;
extern "C" unsigned long hex2ull(const char *ptr);


int main(int argc, char * argv[])
{

	// Setup file path, predictor, and JSON output structures
    std::string traces_path("/cs/studres/CS4202/Coursework/P2-BranchPredictor/branch_traces/");
    
    
    json output = json::array(); // global output object
    json file_summary = json::object(); // each result for a file

	for (const auto & f : std::filesystem::directory_iterator(traces_path)) {
        const std::string& file_path = f.path();
        StaticProfiledPredictor spp(file_path);
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
            bool is_direct_branch = *(data + counter + 36) == '1';
            bool taken = *(data + counter + 40) == '1';
			uint64_t program_counter = hex2ull(data + counter);
            uint64_t target_address = hex2ull(data + counter + 17);

            uint64_t const info = is_direct_branch ? (program_counter < target_address ? 1 : 2) : 0;
            spp.predict(info, taken);
			counter += lineLength;
		}

        // Format results
        file_summary["file_name"] = file_path.substr(file_path.find_last_of("/") + 1);
        file_summary["proportion_forward"] = spp . get_forward_proportion();
        file_summary["proportion_forward_taken"] = spp . get_forward_taken_proportion();
        file_summary["proportion_backward_taken"] = spp . get_backward_taken_proportion();
        file_summary["missprediction_rate"] = spp . get_missprediction_rate();
        output.push_back(file_summary);
	}

	std::cout << output.dump(2);

}