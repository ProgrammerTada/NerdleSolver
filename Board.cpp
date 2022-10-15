// Copyright 2022, Henry Herröder
// Author: Henry Herröder

#include <vector>
#include <memory>
#include "./Board.h"

Board::Board(int length) : defMaxTries(length * 5){
    length_ = length;
    usageCaps_ = make_shared<vector<UsageCap>>();
    for (char c : defNums) {
        UsageCap uc;
        uc.character_ = c;
        uc.allowed_ = length_;
        uc.used_ = 0;
        usageCaps_->push_back(uc);
    }
    for (char c : defOps) {
        UsageCap uc;
        uc.character_ = c;
        uc.allowed_ = length_;
        uc.used_ = 0;
        usageCaps_->push_back(uc);
    }
    UsageCap uc;
    uc.character_ = '=';
    uc.allowed_ = 1;
    uc.used_ = 0;
    usageCaps_->push_back(uc);

    allowedAtPos_ = make_shared<vector<shared_ptr<vector<char>>>>();
    for (int i = 0; i < length_; ++i) {
        shared_ptr<vector<char>> base = make_shared<vector<char>>(defNums);
        allowedAtPos_->push_back(base);
        // first and last 2 posses cant have defOps
        if (i >= 1 && i < length_ - 2) {
            allowedAtPos_->at(i)->insert(
                allowedAtPos_->at(i)->end(), defOps.begin(), defOps.end());
        }
        // first half and last pos cant have =
        if (i >= length_ / 2 && i < length_ - 1) {
            allowedAtPos_->at(i)->push_back('=');
        }
    }
}

Board &Board::operator=(const Board &b) {
    length_ = b.length_;
    allowedAtPos_ = b.allowedAtPos_;
    usageCaps_ = b.usageCaps_;
    return *this;
}

void Board::update(const NerdleStatusRow& row) {
    shared_ptr<vector<SymbolPos>> correct = make_shared<vector<SymbolPos>>();
    //shared_ptr<vector<SymbolPos>> wrongPos = make_shared<vector<SymbolPos>>();
    lastWrongPos_->clear();
    shared_ptr<vector<char>> wrong = make_shared<vector<char>>();
    collectResults(row, correct, lastWrongPos_, wrong);
    shared_ptr<vector<SymbolPos>> caps = make_shared<vector<SymbolPos>>();
    hasUsageCaps(correct, lastWrongPos_, wrong, caps);
    updateUsageCaps(caps);
    filterWrong(wrong, caps);
    lockCorrect(correct);
    deleteWrongPos(lastWrongPos_);
    deleteWrong(wrong);
    optimizeAllowedAtPos(correct, caps);
}

/*const string Board::getEq() {
    resetUsage();
    string eq;
    bool opFlag = false;
    for (shared_ptr<vector<char>> cv : *allowedAtPos_) {
        char c;
        if (cv->size() > 1) {
            do {
                c = cv->at(rand_r(&randSeed) % cv->size());
                // printf("C: %c\n", c);
            } while (!isUsable(c));
            if (!(c >= '0' && c <= '9')) {
                if (opFlag) {
                    do {
                        c = cv->at(rand_r(&randSeed) % cv->size());
                    } while (!(c >= '0' && c <= '9') && isUsable(c));
                    opFlag = false;
                } else {
                    opFlag = true;
                }
            }
        } else {
            c = cv->at(0);
        }
        addUsage(c);
        eq.push_back(c);
    }
    return eq;
}*/

string Board::getEqCO() {
    resetUsage();
    string eq;
    for (shared_ptr<vector<char>> cv : * allowedAtPos_) {
        if (cv->size() == 1) {
            eq.push_back(cv->at(0));
            addUsage(cv->at(0));
        } else {
            eq.push_back('_');
        }
    }
    return eq;
}

string Board::getEqAddWP(string eq) {
    if (lastWrongPos_->size() > 0) {
        // get all possible postions for wrongpos symbols
        vector<WrongPosUsage> wpusages;
        for (SymbolPos sp : *lastWrongPos_) {
            WrongPosUsage wpu;
            wpu.character_ = sp.character_;
            for (size_t i = 0; i < allowedAtPos_->size(); ++i) {
                //allowedAtPos_->at(i)->size() > 1 &&

                if (isInVec(allowedAtPos_->at(i), sp.character_)) {
                    wpu.posses.push_back(i);
                }
            }
            wpusages.push_back(wpu);
        }
        // set wrongpos symbols with priority: lowest possible positions first
        size_t min = 1;
        while (wpusages.size() > 0) {
            for (auto i = wpusages.begin(); i != wpusages.end(); ++i) {
                if (i->posses.size() == min) {
                    int pos;
                    int maxTries = i->posses.size() * 5;
                    do {
                        pos = i->posses.at(rand_r(&randSeed) % i->posses.size());
                        --maxTries;
                    } while ((eq.at(pos) != '_' ||
                        !validForEq(eq, pos, i->character_)) && maxTries > 0);
                    if (maxTries == 0)
                        continue;
                    eq.at(pos) = i->character_;
                    addUsage(i->character_);
                    wpusages.erase(i);
                    --i;
                }
            }
            ++min;

            // if no valid position can befound exit
            if (min > defMaxTries)
                break;
        }
        // << "B" << endl;
    }
    return eq;
}

/*string Board::getEqAddWP(string eq, string lastTry) {
    if (lastWrongPos_->size() > 0) {
        shared_ptr<vector<SymbolPos>> notUsed =
            make_shared<vector<SymbolPos>>(*lastWrongPos_);
        
        for (SymbolPos sp : *notUsed) {
            for (shared_ptr<vector<char>> vc : *allowedAtPos_) {
                if (isInVec(vc, sp.character_)) {

                }
            }
        }
        while (notUsed->size() > 0) {
            int i = rand_r(&randSeed) % eq.size();
            if (eq.at(i) == '_') {
                //printf("a\n");
                SymbolPos sp;
                for (auto u = notUsed->begin(); u != notUsed->end(); ++u) {
                    sp = *u;
                    if (sp.index_ != static_cast<int>(i) &&
                            isInVec(allowedAtPos_->at(i), sp.character_)) {
                        eq.at(i) = sp.character_;
                        addUsage(sp.character_);
                        notUsed->erase(u);
                        break;
                    }
                }
            }
        }
    }
    return eq;
}*/

string Board::getEqGuessRest(string eq) {
    for (size_t i = 0; i < eq.size(); ++i) {
        if (eq.at(i) == '_') {
            shared_ptr<vector<char>> atp = make_shared<vector<char>>(*allowedAtPos_->at(i));

            // filter out all symbols that arent useable any more
            for (auto u = atp->begin(); u != atp->end(); u++) {
                if (!isUsable(*u)) {
                    atp->erase(u);
                    u--;
                }
            }
            if (atp->size() == 0)
                return "";

            // now guess with allowed symbols
            char c;
            // if a valid char cant be found within maxTries just accept a
            // wrong eq
            size_t maxTries = defMaxTries;
            do {
                c = atp->at(rand_r(&randSeed) % atp->size());
                --maxTries;
            } while (!validForEq(eq, i, c) && maxTries > 0);
            // its important to only addUsage if a valid char was found
            // otherwise the solver will get stuck
            if (maxTries > 0) {
                if (!isNum(c) && !isNum(eq.at(i - 1))) {
                    do {
                        c = atp->at(rand_r(&randSeed) % atp->size());
                    } while (!isNum(c) || !validForEq(eq, i, c));
                }
                addUsage(c);
            }
            eq.at(i) = c;
        }
    }
    return eq;
}

const string Board::getEq() {
    resetUsage();
    string eq;
    bool opFlag = false;
    for (shared_ptr<vector<char>> cv : *allowedAtPos_) {
        char c;
        if (cv->size() > 1) {
            shared_ptr<vector<char>> notUsed = make_shared<vector<char>>(*cv);
            do {
                c = notUsed->at(rand_r(&randSeed) % notUsed->size());
                deleteFromVec(notUsed, c);
            } while (!isUsable(c));
            if (!isNum(c)) {
                if (opFlag) {
                    do {
                        c = notUsed->at(rand_r(&randSeed) % notUsed->size());
                        deleteFromVec(notUsed, c);
                    } while (!isNum(c) || !isUsable(c));
                    opFlag = false;
                } else {
                    opFlag = true;
                }
            }
        } else {
            c = cv->at(0);
        }
        addUsage(c);
        eq.push_back(c);
    }
    return eq;
}

void Board::addUsage(const char c) {
    ++usageCaps_->at(getUsageCapIndex(c)).used_;
}

bool Board::isUsable(const char c) const {
    UsageCap uc = usageCaps_->at(getUsageCapIndex(c));
    return uc.allowed_ > uc.used_;
}

int Board::getUsageCapIndex(const char c) const {
    for (unsigned int i = 0; i < usageCaps_->size(); ++i) {
        if (usageCaps_->at(i).character_ == c)
            return i;
    }
    return -1;
}

void Board::collectResults(const NerdleStatusRow& row,
        shared_ptr<vector<SymbolPos>> correct,
        shared_ptr<vector<SymbolPos>> wrongPos,
        shared_ptr<vector<char>> wrong) const {
    for (unsigned int i = 0; i < row.size(); ++i) {
        SymbolPos sp;
        sp.character_ = row.at(i).character_;
        sp.index_ = i;
        switch (row.at(i).status_) {
            case NerdleStatus::Correct:
                correct->push_back(sp);
                break;
            case NerdleStatus::WrongPosition:
                wrongPos->push_back(sp);
                break;
            case NerdleStatus::Wrong:
                wrong->push_back(sp.character_);
                break;
        }
    }
}

void Board::hasUsageCaps(const shared_ptr<vector<SymbolPos>> correct,
        const shared_ptr<vector<SymbolPos>> wrongPos,
        const shared_ptr<vector<char>> wrong,
        shared_ptr<vector<SymbolPos>> caps) const {
    for (char c : *wrong) {
        int cap = 0;
        for (SymbolPos spc : *correct) {
            if (c == spc.character_) {
                ++cap;
            }
        }
        for (SymbolPos spwp : *wrongPos) {
            if (c == spwp.character_) {
                ++cap;
            }
        }
        if (cap > 0) {
            SymbolPos sp;
            sp.character_ = c;
            sp.index_ = cap;
            caps->push_back(sp);
        }
    }    
}

void Board::filterWrong(shared_ptr<vector<char>> wrong,
        const shared_ptr<vector<SymbolPos>> caps) const {
    for (SymbolPos sp : *caps) {
        deleteFromVec(wrong, sp.character_);
    }
}

void Board::lockCorrect(const shared_ptr<vector<SymbolPos>> correct) {
    for (SymbolPos sp : *correct) {
        allowedAtPos_->at(sp.index_)->clear();
        allowedAtPos_->at(sp.index_)->push_back(sp.character_);
    }
}

void Board::optimizeAllowedAtPos(const shared_ptr<vector<SymbolPos>> correct,
                                    const shared_ptr<vector<SymbolPos>> caps) {
    shared_ptr<vector<char>> sDO = make_shared<vector<char>>(defOps);
    shared_ptr<vector<char>> sDN = make_shared<vector<char>>(defNums);
    for (size_t i = 0; i < correct->size(); ++i) {
        SymbolPos sp = correct->at(i);
        if (!isNum(sp.character_)) {
            // if op is correct there cant be one next to it
            if (sp.index_ > 0) {
                deleteFromVec(allowedAtPos_->at(sp.index_ - 1), sDO);
            }
            if (sp.index_ < length_ - 1) {
                deleteFromVec(allowedAtPos_->at(sp.index_ + 1), sDO);
            }
            // if op is followed by a 0 the next symbol has to be a op
            if (sp.index_ < length_ - 2 && correct->size() < i + 1)
                if (correct->at(i + 1).character_ == '0') {
                    deleteFromVec(allowedAtPos_->at(sp.index_ + 2), sDN);
                }
            // if there is an = than there cant be an op after it
            if(!equalsFlag && sp.character_ == '=') {
                equalsFlag = true;
                for (size_t u = sp.index_ + 1; u < allowedAtPos_->size(); ++u) {
                    deleteFromVec(allowedAtPos_->at(u), sDO);
                    deleteFromVec(allowedAtPos_->at(u), '=');
                }
                // an = with 2 or more numbers to the right cant be followed
                // by a 0
                if (sp.index_ < length_ - 2) {
                    deleteFromVec(allowedAtPos_->at(sp.index_ + 1), '0');
                }
            }
            // if the amount of correct symbols is equal to the allowed cap it
            // can be deleted from all other posses
            for (SymbolPos s : *caps) {
                int amount = 0;
                for (SymbolPos c : *correct) {
                    if (c.character_ == s.character_)
                        ++amount;
                }
                if (amount == s.index_) {
                    for (size_t u = 0; u < allowedAtPos_->size(); ++u) {
                        for (SymbolPos cor : *correct) {
                            // 80 chars line limit is not enough sry :(
                            if (cor.index_ != u &&
                                    cor.character_ == s.character_ &&
                                    allowedAtPos_->at(u)->at(0) !=
                                    cor.character_)
                                deleteFromVec(allowedAtPos_->at(u),
                                              s.character_);
                        }
                    }
                }
            }
        }
    }
    // check if any posses that werent in correct got optimized to size 1
    // and update lastWrongPos accordingly
    for(size_t i = 0; i < allowedAtPos_->size(); ++i) {
        if (allowedAtPos_->at(i)->size() == 1) {
            bool wasCor = false;
            for (SymbolPos sp: *correct) {
                if (static_cast<int>(sp.index_) == i) {
                    wasCor = true;
                    break;
                }
            }
            if (!wasCor)
                optimizeLastWrongPos(allowedAtPos_->at(i)->at(0));
        }
    }
}

void Board::optimizeLastWrongPos(const char c) {
    for (auto i = lastWrongPos_->begin(); i != lastWrongPos_->end(); ++i) {
        if (i->character_ == c) {
            lastWrongPos_->erase(i);
            return;
        }
    }
}

bool Board::validForEq(const string eq, const size_t pos, const char c) const {
    if (!isNum(c)) {
        if ((!isUnderscore(eq.at(pos - 1)) && !isNum(eq.at(pos - 1))) ||
                (!isUnderscore(eq.at(pos + 1)) && !isNum(eq.at(pos + 1)) &&
                eq.at(pos + 1) != '0'))
            return false;
    }
    if (pos == 1 && eq.at(0) == '0' && isNum(c))
        return false;
    if (c == '0') {
        if (pos < eq.size() - 1 && pos > 0 &&
            !isUnderscore(eq.at(pos - 1)) && !isNum(eq.at(pos - 1)) &&
            isNum(eq.at(pos + 1)))
            return false;
        if (pos == 0 && isNum(eq.at(pos + 1)))
            return false;
    }
    if (pos > 0 && !isUnderscore(eq.at(pos - 1)) &&
            !isNum(eq.at(pos - 1)) && !isNum(c)) {
        return false;
    }
    return true;
}

bool Board::isInVec(const shared_ptr<vector<char>> vec, const char c) const {
    for (char vc : *vec) {
        if (vc == c)
            return true;
    }
    return false;
}

void Board::deleteWrongPos(const shared_ptr<vector<SymbolPos>> wrongPos) {
    for (SymbolPos sp : *wrongPos) {
        deleteFromVec(allowedAtPos_->at(sp.index_), sp.character_);
    }
}

void Board::deleteWrong(const shared_ptr<vector<char>> wrong) {
    for (shared_ptr<vector<char>> &cv : *allowedAtPos_) {
        deleteFromVec(cv, wrong);
    }
}

void Board::updateUsageCaps(shared_ptr<vector<SymbolPos>> caps) {
    for (SymbolPos sp : *caps) {
        for (UsageCap &uc : *usageCaps_) {
            if (uc.character_ == sp.character_) {
                uc.allowed_ = sp.index_;
                break;
            }
        }
    }
}

void Board::resetUsage() {
    for (UsageCap &uc : *usageCaps_) {
        uc.used_ = 0;
    }
}

bool Board::deleteFromVec(shared_ptr<vector<char>> vec, const char c) const {
    for (vector<char>::iterator i = vec->begin(); i != vec->end(); ++i) {
        if (*i == c) {
            vec->erase(i);
            return true;
        }
    }
    return false;
}

bool Board::deleteFromVec(shared_ptr<vector<char>> vec,
        const shared_ptr<vector<char>> chars) const {
    bool removedItem = false;
    for (char c : *chars) {
        for (vector<char>::iterator i = vec->begin(); i != vec->end(); ++i) {
            if (*i == c) {
                vec->erase(i);
                removedItem = true;
                break;
            }
        }
    }
    return removedItem;
}

bool Board::isNum(const char c) const {
    return (c >= '0' && c <= '9');
}
