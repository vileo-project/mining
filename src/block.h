// Copyright (c) 2019-2019 The Vileo Project developers, https://vileo.org/
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#pragma once
#include <vector>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include "mining.h"

using namespace std;

class CBlock {
    uhash hash;
    uhash work;
    uhash target = uhash (-1) >> 1;
    CBlock* prev = nullptr;
    vector <CBlock*> next;
    static vector <CBlock*> fork;
    static unsigned ntime;
    unsigned index = 0;
    unsigned time = 0;
    unsigned algo = 1;
    unsigned a51 = 0;
    void settarget();
    CBlock(CBlock* _prev, unsigned _algo);
    //void Print();
    uhash algopart();

public:
    void PrintAlgo();
    void PrintFork();
    //void PrintWork();
    //void PrintAll();
    void mining(unsigned _amount);
    CBlock();
    ~CBlock();

};
