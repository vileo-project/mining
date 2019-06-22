// Copyright (c) 2019-2019 The Vileo Project developers, https://vileo.org/
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#pragma once

#include <tuple>
#include <random>
#include <chrono>

typedef unsigned long long uhash;

namespace {
    auto fakemining(uhash _target, unsigned int _hashrate) {
        unsigned work = 0;
        using namespace std;
        using namespace std::chrono;
        static mt19937_64 gen(duration_cast<nanoseconds>(system_clock::now().time_since_epoch()).count() / 100);
        uhash hash = 0 - 1;
        for (; hash > _target; work++) {
            hash = gen();
        }
        return  make_tuple(hash, work > _hashrate ? work / _hashrate : 1);
    }
}
