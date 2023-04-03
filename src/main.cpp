#include <stdexcept>
#include <iostream>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>
#include "predictors/two-bit-predictor.cpp"
#include "predictors/always-taken-predictor.cpp"
#include "predictors/gshare-predictor.cpp"
#include "predictors/perceptron-predictor.cpp"



int main(int argc, char * argv[])
{

	// Path setup
    std::string traces_path("/cs/studres/CS4202/Coursework/P2-BranchPredictor/branch_traces/");
    std::string path_to_trace_file = traces_path + "xz.out";

	// File reading setup
    struct stat sb;
	uintmax_t counter = 0;
	int fd;
	const uintmax_t lineLength = 42;
	fd = open(path_to_trace_file.data(), O_RDONLY);
	fstat(fd, &sb);
	const char * data = static_cast<const char*>(mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0u));

	// Branch Predictor setup

	//BranchPredictor ** predictors = new BranchPredictor*[6];leela.out
	//BranchPredictor *always_taken = new AlwaysTakenPredictor();
	//BranchPredictor *two_bit_512 = new TwoBitPredictor(512);
	//BranchPredictor *two_bit_1024 = new TwoBitPredictor(1024);
	//BranchPredictor *two_bit_2048 = new TwoBitPredictor(2048);
	//BranchPredictor *two_bit_4096 = new TwoBitPredictor(4096);

	BranchPredictor * predictors[] = {
		new AlwaysTakenPredictor(),
		new TwoBitPredictor(512),
		new TwoBitPredictor(1024),
		new TwoBitPredictor(2048),
		new TwoBitPredictor(4096),
		new GSharePredictor(512),
		new GSharePredictor(1024),
		new GSharePredictor(2048),
		new GSharePredictor(4096),
		new PerceptronPredictor(12, 1 << 14, 10)
	};

	// File reading loop
	while (counter < sb.st_size)
	{
		bool is_conditional_branch = *(data + counter + 38) == '1';
		if (!is_conditional_branch) {counter += lineLength;}
		auto program_counter = strtoul(data + counter, nullptr, 16); // string to unsigned long, from base 16
		bool taken = strtoul(data + counter + 40, nullptr, 10); // string to unsigned long, from base 10

		// Run trace on always taken predictor
		//bp -> predict(program_counter, taken);
		for (BranchPredictor * bp : predictors)
		{
			bp -> predict(program_counter, taken);
		}

		counter += lineLength;
	}

	// Print after finish
	for (BranchPredictor * bp : predictors)
		{
			std::cout << bp -> get_name() << "\n";
			std::cout << "Accuracy: " << bp -> get_accuracy() << "\n";
			std::cout << "Total Predictions: " << bp -> get_total_predictions() << "\n\n";
		}
	//std::cout << "Accuracy: " << bp -> get_accuracy() << "\n";
	//std::cout << "Total Predictions: " << bp -> get_total_predictions() << "\n";
}