// Copyright 2022, Name of Student
// Author : Name of Student <iKnowCPlusPlus@cs.uni-freiburg.de>
//
// Based on template kindly provided by Prof. Dr. Hannah Bast
// and Johannes Kalmbach for the C++-course at the University of Freiburg.


#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>
// Implementation of your custom solver.
#include "./NerdleSolver.h"

int main(int argc, char** argv) {
    // Read the command lines from file. The first argument should always
    // be the expression length (8, 9, 10, 11). If your solver requires
    // additional arguments, they should be in argv[2], argv[3] etc.
    // Don't forget to update the Usage information below so that your
    // tutor knows, how to run your code.
    if (argc != 2) {
    std::cerr << "Usage ./ExpressionGeneratorMain <lengthOfExpressions>"
        << std::endl;
    std::exit(1);
    }

    int lengthOfExpressions = std::atoi(argv[1]);

    // Create an Object of your solver class. This might take some arguments
    // (the lengths of the expressions, additional data passed in from the
    // command line, etc.
    NerdleSolver solver(lengthOfExpressions);

    // Run all benchmarks.
    runNerdleBenchmark(&solver, lengthOfExpressions);

    // Alternatively for debugging you can run the benchmark with specified
    // expressions:
    // std::vector<std::string> expressions;
    // expressions.push_back("1*4+2+4-4=6");
    // runNerdleBenchmark(&solver, 11, expressions);
}

