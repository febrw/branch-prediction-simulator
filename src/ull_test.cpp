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
#include "predictors/two-bit-predictor.cpp"
#include "predictors/always-taken-predictor.cpp"
#include "predictors/gshare-predictor.cpp"
#include "predictors/global-perceptron-predictor.cpp"
#include "predictors/hybrid-perceptron-predictor.cpp"
#include "predictors/local-perceptron-predictor.cpp"

#include "include/json.hpp"
using json = nlohmann::json;

extern "C" unsigned long hex2ull(const char *ptr);

void f1();
void f2();
void equality_test();

int main(int argc, char * argv[])
{
    f2();
}

void equality_test() {

    // Get file path
    const std::string& file_path("/cs/studres/CS4202/Coursework/P2-BranchPredictor/branch_traces/wrf.out");

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

        uint64_t program_counter = strtoul(data + counter, nullptr, 16); // string to unsigned long, from base 16
        uint64_t new_pc = hex2ull(data + counter);

        assert(program_counter==new_pc);
        counter += lineLength;
    }
}

void f1() {
    // Get file path
    const std::string& file_path("/cs/studres/CS4202/Coursework/P2-BranchPredictor/branch_traces/wrf.out");

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

        uint64_t program_counter = strtoul(data + counter, nullptr, 16); // string to unsigned long, from base 16
        counter += lineLength;
    }
}

void f2() {
    // Get file path
    const std::string& file_path("/cs/studres/CS4202/Coursework/P2-BranchPredictor/branch_traces/wrf.out");

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

        uint64_t new_pc = hex2ull(data + counter);
        counter += lineLength;
    }
}