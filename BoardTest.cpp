// Copyright 2022, Henry Herröder
// Author: Henry Herröder

#include <gtest/gtest.h>
#include <string>
#include <memory>
#include <vector>
#include "./Board.h"

using namespace std;  // NOLINT

TEST(Board, hasUsageCaps) {
    SymbolPos c;
    c.character_ = '0';
    c.index_ = 1;
    SymbolPos wp;
    wp.character_ = '0';
    wp.index_ = 2;
    char w = '0';
    vector<SymbolPos> correct = {c};
    vector<SymbolPos> wrongPos = {wp};
    vector<char> wrong = {w};
    shared_ptr<vector<SymbolPos>> spCorrect =
        make_shared<vector<SymbolPos>>(correct);
    shared_ptr<vector<SymbolPos>> spWrongPos =
        make_shared<vector<SymbolPos>>(wrongPos);
    shared_ptr<vector<char>> spWrong =
        make_shared<vector<char>>(wrong);
    shared_ptr<vector<SymbolPos>> caps =
        make_shared<vector<SymbolPos>>();
    Board b(11);
    b.hasUsageCaps(spCorrect, spWrongPos, spWrong, caps);
    ASSERT_EQ(caps->at(0).character_, '0');
    ASSERT_EQ(caps->at(0).index_, 2);
}

// 1. set cap for 0 to 1
// 2. set cap for 1 to 3
// 3. set cap for 0 and 1 (at same time) on new board
TEST(Board, updateUsageCaps) {
    Board b(11);
    SymbolPos nCap;
    nCap.character_ = '0';
    nCap.index_ = 1;
    vector<SymbolPos> allNCaps = {nCap};
    shared_ptr<vector<SymbolPos>> capUpdate =
        make_shared<vector<SymbolPos>>(allNCaps);
    b.updateUsageCaps(capUpdate);
    ASSERT_EQ(b.usageCaps_->at(0).allowed_, 1);

    SymbolPos nCap2;
    nCap2.character_ = '1';
    nCap2.index_ = 3;
    vector<SymbolPos> allNCaps2 = {nCap2};
    shared_ptr<vector<SymbolPos>> capUpdate2 =
        make_shared<vector<SymbolPos>>(allNCaps2);
    b.updateUsageCaps(capUpdate2);
    ASSERT_EQ(b.usageCaps_->at(1).allowed_, 3);

    b = Board(10);
    allNCaps.push_back(nCap2);
    shared_ptr<vector<SymbolPos>> capUpdate3 =
        make_shared<vector<SymbolPos>>(allNCaps);
    b.updateUsageCaps(capUpdate3);
    ASSERT_EQ(b.usageCaps_->at(0).allowed_, 1);
    ASSERT_EQ(b.usageCaps_->at(1).allowed_, 3);
}
