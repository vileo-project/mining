// Copyright (c) 2019-2019 The Vileo Project developers, https://vileo.org/
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#include <iostream>
#include <iomanip>
#include "block.h"

int main() {
    using namespace std;
    CBlock c;
    /*uhash a;
    int b;
    vector <uhash> vec;
    for (auto i = 1; i < 100; i++) {
        tie(a, b) = fakemining(1000, 1);
        vec.push_back(a);
    }
    for (auto &i : vec)cout << uppercase << hex << setw(16) << setfill('0') << i << endl;*/
    //while (true) fakemining(1, 1);
    c.mining(10000);
    c.PrintAll();
    //c.PrintFork();
    c.PrintAlgo();
    return 0;
}