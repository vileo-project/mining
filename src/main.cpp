// Copyright (c) 2019-2019 The Vileo Project developers, https://vileo.org/
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#include <iostream>
#include <iomanip>
#include "block.h"

int main() {
    CBlock c;
    c.mining(3000);
    c.PrintFork();
    c.PrintAlgo();
    return 0;
}