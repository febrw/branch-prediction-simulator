# branch-prediction-simulator

## Running experiments

To produce an experiment, navigate to the location source cpp file in /src/experiments/

E.g. "pretrained-gshare-perceptron.cpp"

Create object file:

    g++ -c -o pretrained-gshare-perceptron.o pretrained-gshare-perceptron.cpp

Create executable, linked with object files:

    g++ -o pretrained-gshare-perceptron -g -O3 pretrained-gshare-perceptron.o hex.o

Run executable (dumps JSON to standard output) and save JSON to file:

    ./pretrained-gshare-perceptron > ../jsons/pretrained-gshare-perceptron.json

Navigate to src/plots, and find the python script with the matching name

Run:

    python3 pretrained-gshare-perceptron-results.py

You will need to import pandas and matplotlib to produce the figures.
