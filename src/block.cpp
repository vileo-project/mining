// Copyright (c) 2019-2019 The Vileo Project developers, https://vileo.org/
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#include "block.h"

unsigned CBlock::ntime = 60;
vector <CBlock*> CBlock::fork;

void CBlock::settarget() {
    unsigned range = 300;
    auto atime = prev->time;
    uhash awork = 0;
    CBlock* block = this;
    range = index <= range ? index : range;
    for (; range && block->prev; range--) {
        block = block->prev;
        if (block->algo == algo) {
            //work = 2^256 / (target + 1)
            awork += ~block->target / (block->target + 1) + 1;
        }
    }
    atime -= block->prev ? block->prev->time : 0;
    if (atime && ntime * awork / atime > 1)
        //target = 2^256 / work - 1
        target = (~(ntime * awork / atime) + 1) / (ntime * awork / atime);
    if (target > uhash(-1) >> 1) target = uhash(-1) >> 1;
}



void CBlock::mining(unsigned _amount) {
    while (_amount) {
        auto root = fork.back();
        auto a = new CBlock(root, 1);
        root->next.push_back(a);
        a->work = a->prev->work + ~a->target / (a->target + 1) + 1;
        fork = root->next;
        --_amount;
    }
}



CBlock::CBlock(CBlock* _prev, unsigned _algo) {
    int ahashrate = 4;
    algo = _algo;
    prev = _prev;
    index = prev->index + 1;
    settarget();
    tie(hash, time) = fakemining(target, ahashrate);
    if (prev) time += prev->time;
}

CBlock::CBlock() {
    tie(hash, time) = fakemining(target, 1);
    work = ~target / (target + 1) + 1;
    fork.push_back(this);
}

CBlock::~CBlock() {
}

void CBlock::Print() {
    cout << uppercase << dec << setw(6) << setfill(' ') << index << "\t";
    cout << uppercase << dec << setw(5) <<  algo << "\t";
    cout << uppercase << dec << setw(8) << setfill('0') << work << "\t";
    cout << uppercase << hex << setw(16) << target << " (" << dec << time - (prev ? prev->time : 0) << ")" << '\n';
}

void CBlock::PrintAll() {
    auto ftmp = this;
    cout << " index   algo       work           target    time" << endl;
    ftmp->Print();
    while (!ftmp->next.empty()) {
        for (auto &i : ftmp->next) i->Print();
        ftmp = ftmp->next.front();
    }
}

void CBlock::PrintAlgo() {
    auto ptr = fork.back();
    unsigned a1 = 1, t1 = time;
    uhash w1 = ~target / (target + 1) + 1;
    while (ptr->prev) {
        a1++, t1 += ptr->time - ptr->prev->time, w1 += ~ptr->target / (ptr->target + 1) + 1;
        ptr = ptr->prev;
    }
    cout << dec << "amount " << '\t' << setw(10) << setfill(' ') << a1 << endl;
    cout << dec << "work   " << '\t' << setw(10) << w1 << endl;
    cout << dec << "time   " << '\t' << setw(10) << (unsigned)(t1 / a1) << endl;
}
