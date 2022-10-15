// Copyright 2022, Henry Herröder
// Author: Henry Herröder

#ifndef NERDLESOLVER_H_
#define NERDLESOLVER_H_

#include <gtest/gtest.h>
#include <math.h>
#include <memory>
#include <vector>
#include <string>
#include "./NerdleBenchmark.h"
#include "./Board.h"

using namespace std;  // NOLINT

class NerdleSolver : public NerdleSolverBase {
 public:
    // For testing
    FRIEND_TEST(NerdleSolver, checkSyntax);
    FRIEND_TEST(NerdleSolver, checkCorrectEquation);
    // setup solver for nerdle game where length is the lenght of the equations
    explicit NerdleSolver(int length) : length_(length) { board_ = Board(length); }
    // generate the next guess for the nerdle game
    string nextGuess(const NerdleGameState& gameState) override;

 private:
    // saves the last size of gamestate (needed for new game detection)
    unsigned int lastGSSize_ = 0;
    // keeps track of the Game
    Board board_;
    // lenght of equations
    unsigned int length_;

    // check if current game was won
    bool checkWin(const NerdleStatusRow& row);
    // returns true if equation has correct syntax
    bool checkSyntax(const string eq) const;
    // returns true if equation is mathamatically correct
    bool checkCorrectEquation(const string eq) const;
    // counts amount of _ in equation
    const int countUnderscores(const string eq) const;
    // returns true if equation was not tried yet
    bool notTried(const vector<string> tried, const string eq) const;
    // checks if char is a number
    bool isNum(const char c) const;
    // checks if int division is possible
    bool safeDivision(const int lh, const int rh) const;
    // check is one of the multiplication operands is 0
    bool safeMultiplication(const int lh, const int rh) const;
    // returns all numbers and symbols of given equation by reference
    // also gives the order of operations
    void splitEquation(const string eq, shared_ptr<vector<int>> nums,
        shared_ptr<vector<char>> ops, shared_ptr<vector<int>> order) const;
    // performs given operation with given operands
    int performOp(const int lh, const int rh, const char op) const;
};

#endif  // NERDLESOLVER_H_
