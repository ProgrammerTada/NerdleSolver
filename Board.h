// Copyright 2022, Henry Herröder
// Author: Henry Herröder

#ifndef BOARD_H_
#define BOARD_H_

#include <gtest/gtest.h>
#include <vector>
#include <memory>
#include <random>

#include "./NerdleBenchmark.h"

using namespace std;  // NOLINT

// saves a char and its position
struct SymbolPos {
    char character_;
    int index_;
};

// saves how often a character is allowed to be used and how often it is
// currently used
struct UsageCap {
    char character_;
    int allowed_;
    int used_;
};

// saves character and where its still allowed for wrong pos symbols
struct WrongPosUsage {
    char character_;
    vector<size_t> posses;
};

// class for keeping track of hints of the nerdle game
// and generating different steps of equation
class Board {
 public:
    // For testing:
    FRIEND_TEST(Board, updateUsageCaps);
    FRIEND_TEST(Board, hasUsageCaps);
    // setup board for given equation length
    // default constructor will use equation length = 8
    Board(const int length = 8);
    Board &operator=(const Board &b);
    // Updates the allowed symbols
    void update(const NerdleStatusRow& row);
    // return a valid eq based on given hints
    const string getEq();
    // returns an eq where only correct symbols are set, others are '_'
    string getEqCO();
    string getEqAddWP(string eq);
    string getEqGuessRest(string eq);
    string getEqBruteForce(string eq);

 private:
    // default set of numbers and operations
    const vector<char> defNums =
        {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
    const vector<char> defOps = {'+', '-', '*', '/'};
    // equation length
    int length_;
    // default value for maximum tries
    const size_t defMaxTries;
    // save if eq was already optimized for =
    bool equalsFlag = false;
    // saves last set of wrongPos symbols
    shared_ptr<vector<SymbolPos>> lastWrongPos_ =
        make_shared<vector<SymbolPos>>();
    // saves how often all (non correct) characters are allowed
    shared_ptr<vector<UsageCap>> usageCaps_;
    // all positions and allowed characters at those positions
    shared_ptr<vector<shared_ptr<vector<char>>>> allowedAtPos_;
    // seed for rand_r
    unsigned int randSeed = (unsigned int)time(NULL);

    // finds correct usagecap and adds 1 to used
    void addUsage(const char c);
    // returns true if symbol is below usage cap i.e. can be used
    bool isUsable(const char c) const;
    // returns index of usagecap in vector
    int getUsageCapIndex(const char c) const;
    // sorts last statusrow into correct wrongpos and wrong
    void collectResults(const NerdleStatusRow& row,
        shared_ptr<vector<SymbolPos>> correct,
        shared_ptr<vector<SymbolPos>> wrongPos,
        shared_ptr<vector<char>> wrong) const;
    // checks if usagecap is triggert and what the cap is
    void hasUsageCaps(const shared_ptr<vector<SymbolPos>> correct,
        const shared_ptr<vector<SymbolPos>> wrongPos,
        const shared_ptr<vector<char>> wrong,
        shared_ptr<vector<SymbolPos>> caps) const;
    // makes sure that symbols marked as wrong because of amount cap dont get 
    // deleted completly
    void filterWrong(shared_ptr<vector<char>> wrong,
        const shared_ptr<vector<SymbolPos>> caps) const;
    // lock correct chars at their position
    void lockCorrect(const shared_ptr<vector<SymbolPos>> correct);
    // optimize allowed symbols to make less wrong guesses
    void optimizeAllowedAtPos(const shared_ptr<vector<SymbolPos>> correct,
                                const shared_ptr<vector<SymbolPos>> caps);
    // if optimizeAllowedAtPos has figured out that a lastWrongPos can only be
    // in one spot it will be removed from lastWrong pos because its added
    // always anyway by default to the equaiton
    void optimizeLastWrongPos(const char c);
    // returns true if character at given pos is allowed in equation
    bool validForEq(const string eq, const size_t pos, const char c) const;
    // delete chars at positions where they arent allowed
    void deleteWrongPos(const shared_ptr<vector<SymbolPos>> wrongPos);
    // deletes chars that arent allowed in the equation
    void deleteWrong(const shared_ptr<vector<char>> wrong);
    // updates the usage caps for given symbol index of symbolpos is new cap
    void updateUsageCaps(const shared_ptr<vector<SymbolPos>> caps);
    // reset how often a character is used (needed before a
    // new equation is generated)
    void resetUsage();
    // returns true if char is in vector
    bool isInVec(const shared_ptr<vector<char>> vec, const char c) const;
    // deletes a char from vector of chars (no duplicates) true if item
    // was deleted
    bool deleteFromVec(shared_ptr<vector<char>> vec, const char c) const;
    // delete chars from vector of chars (no duplicates) true if item
    // was deleted
    bool deleteFromVec(shared_ptr<vector<char>> vec,
        shared_ptr<vector<char>> chars) const;
    // returns true if char is a number
    bool isNum(const char c) const;
    // returns true if char is _
    bool isUnderscore(const char c) const { return  c == '_'; }
};

#endif  // BOARD_H_
