# Branch Prediction Simulator

This project explores several branch prediction strategies and the extent to which profile data can be used to improve branch predictor performance for a given program.

## Predictors Implemented

- (Static) Always Taken
- (Dynamic) 2-bit Saturating Counter
- GShare
- Local Perceptron
- Global Perceptron
- GShare Perceptron

## Running experiments

To produce an experiment, navigate to the location source cpp file in /src/experiments/

E.g. "pretrained-gshare-perceptron.cpp"

Create object file:
```bash
g++ -c -o pretrained-gshare-perceptron.o pretrained-gshare-perceptron.cpp
```
Create executable, linked with object files:
```bash
g++ -o pretrained-gshare-perceptron -g -O3 pretrained-gshare-perceptron.o hex.o
```
Run executable (dumps JSON to standard output) and save JSON to file:
```bash
./pretrained-gshare-perceptron > ../jsons/pretrained-gshare-perceptron.json
```
Within src/plots, find the python script with the matching name

Run:
```
python3 pretrained-gshare-perceptron-results.py
```
Python Libraries for producing graphs:
- Matplotlib
- Pandas
