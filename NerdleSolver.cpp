// Copyright 2022, Henry Herröder
// Author: Henry Herröder

#include <memory>
#include <vector>
#include <string>
#include <iostream>
#include "./NerdleSolver.h"

std::string NerdleSolver::nextGuess(const NerdleGameState& gameState) {
    if (gameState.size() < lastGSSize_) {
        board_ = Board(length_);
    }
    if (gameState.size() > 0) {
        if (checkWin(gameState.back()))
            board_ = Board(length_);
        else
            board_.update(gameState.back());
    }
    else if (gameState.size() == 0) {
        // the first guess for every game should include a huge viarty of different
        // symbols (save computation time)
        switch (length_) {
            case 8:
                return "1+7*9=64";
            case 9:
                return "20+7*9=83";
            case 10:
                return "1+7*9-0=64";
            case 11:
                return "21+7*9-0=84";
        }
    }
    string eq = board_.getEqCO();
    string lastTry = board_.getEqAddWP(eq);
    lastTry = board_.getEqGuessRest(lastTry);
    while (!checkSyntax(lastTry) || !checkCorrectEquation(lastTry)) {
        eq = board_.getEqCO();
        lastTry = board_.getEqAddWP(eq);
        // if only half of equation needs to be guess it can be brute-forced
        /*if (countUnderscores(lastTry) < length_ / 2) {
            vector<string> triedWPeqs;
            triedWPeqs.push_back(lastTry);
            board_.getEqBruteForce(lastTry);
            while (!checkSyntax(lastTry) || !checkCorrectEquation(lastTry)) {
                do {
                    lastTry = board_.getEqAddWP(eq);
                } while (!notTried(triedWPeqs, lastTry));
                triedWPeqs.push_back(lastTry);
                board_.getEqBruteForce(lastTry);
            }
        } else {*/
            lastTry = board_.getEqGuessRest(lastTry);
        //}
    }
    lastGSSize_ = gameState.size();
    return lastTry;
}

bool NerdleSolver::checkWin(const NerdleStatusRow& row) {
    for (CharacterAndStatus cas : row) {
        if (cas.status_ != NerdleStatus::Correct)
            return false;
    }
    return true;
}

bool NerdleSolver::checkSyntax(const string eq) const {
    if (eq.length() != length_ || !isNum(eq.at(0)) || !isNum(eq.back()))
        return false;
    if (eq.at(0) == '0' && isNum(eq.at(1)))
        return false;
    char lastC = eq.at(0);
    unsigned int equalsPos = 999;  // 999 means no '=' yet
    for (unsigned int i = 0; i < length_; ++i) {
        if (eq.at(i) == '=') {
            if (!isNum(eq.at(i - 1)) || !isNum(eq.at(i + 1)))
                return false;
            if (equalsPos != 999)
                return false;
            equalsPos = i;
        }
        if (i > equalsPos && !isNum(eq.at(i)))
            return false;
        if (eq.at(i) == '0' && i < eq.size() - 1 && isNum(eq.at(i + 1)) &&
                i > 0 && !isNum(eq.at(i - 1)))
            return false;
        if (!isNum(lastC) && !isNum(eq.at(i))) {
                return false;
        }
        lastC = eq.at(i);
    }
    if (equalsPos == 999)
        return false;
    return true;
}

bool NerdleSolver::checkCorrectEquation(const string eq) const {
    shared_ptr<vector<int>> nums = make_shared<vector<int>>();
    shared_ptr<vector<char>> ops = make_shared<vector<char>>();
    shared_ptr<vector<int>> order = make_shared<vector<int>>();
    splitEquation(eq, nums, ops, order);
    if (ops->size() == 0)
        return false;
    while (!order->empty()) {
        int i = order->at(0);
        if (ops->at(i) == '/' && !safeDivision(nums->at(i), nums->at(i + 1)))
            return false;
        else if (ops->at(i) == '*' &&
                !safeMultiplication(nums->at(i), nums->at(i + 1)))
            return false;
        nums->at(i) = performOp(nums->at(i), nums->at(i+1), ops->at(i));
        for (int& u : *order) {
            if (u > i)
                --u;
        }
        order->erase(order->begin());
        nums->erase(nums->begin() + i + 1);
        ops->erase(ops->begin() + i);
    }
    return nums->at(nums->size() - 2) == nums->at(nums->size() - 1);
}

const int NerdleSolver::countUnderscores(const string eq) const {
    int amount = 0;
    for (char c : eq) {
        if (c == '_')
            ++amount;
    }
    return amount;
}

bool NerdleSolver::notTried(const vector<string> tried, const string eq) const {
    for (string s : tried) {
        if (s == eq)
            return false;
    }
    return true;
}

bool NerdleSolver::isNum(const char c) const {
    if (c >= '0' && c <= '9')
        return true;
    return false;
}

bool NerdleSolver::safeDivision(const int lh, const int rh) const {
    return lh != 0 && rh != 0 && lh % rh == 0;
}

bool NerdleSolver::safeMultiplication(const int lh, const int rh) const {
    return lh != 0 && rh != 0;
}

void NerdleSolver::splitEquation(const string eq, shared_ptr<vector<int>> nums,
        shared_ptr<vector<char>> ops, shared_ptr<vector<int>> order) const {
    int curNum = 0;
    int dotOpAmount = 0;
    for (const char c : eq) {
        if (isNum(c)) {
            if (curNum == 0) {
                curNum = c - '0';
            } else {
                curNum *= 10;
                curNum += c - '0';
            }
        } else {
            nums->push_back(curNum);
            curNum = 0;
            if (c != '=') {
                ops->push_back(c);
                if (c == '*' || c == '/') {
                    order->insert(order->begin() +
                        dotOpAmount, ops->size() - 1);
                    ++dotOpAmount;
                } else {
                    order->push_back(ops->size() - 1);
                }
            }
        }
    }
    nums->push_back(curNum);
}

int NerdleSolver::performOp(const int lh, const int rh, const char op) const {
    switch (op) {
        case '*':
            return lh * rh;
        case '/':
            return lh / rh;
        case '+':
            return lh + rh;
        case '-':
            return lh - rh;
        default:
            return -1;
    }
}

/*    */

    /*ops.shrink_to_fit();
    nums.shrink_to_fit();
    if (ops.size() == 1) {
        if (ops.at(0) == '/' && !safeDivision(nums.at(0), nums.at(1)))
            return false;
        return performOp(nums.at(0), nums.at(1), ops.at(0)) == nums.back();
    }
    int oa = order.at(0);
    if (ops.at(oa) == '/' && !safeDivision(nums.at(oa), nums.at(oa + 1)))
        return false;
    nums.at(oa) = performOp(nums.at(oa), nums.at(oa + 1), ops.at(oa));
    nums.erase(nums.begin() + oa + 1);
    ops.erase(ops.begin() + oa);
    nums.shrink_to_fit();
    ops.shrink_to_fit();
    string neq;
    neq.push_back(*to_string(nums.at(0)).c_str());
    for (unsigned int i = 0; i < ops.size(); ++i) {
        neq.push_back(ops.at(i));
        neq.push_back(*to_string(nums.at(i + 1)).c_str());
    }
    neq.push_back('=');
    neq.push_back(*to_string(nums.back()).c_str());
    cout << neq << endl;*/
