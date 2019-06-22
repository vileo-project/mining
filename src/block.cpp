// Copyright (c) 2019-2019 The Vileo Project developers, https://vileo.org/
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#include "block.h"

unsigned CBlock::ntime = 60;
vector <CBlock*> CBlock::fork;

void CBlock::settarget() {
    unsigned range = 300;
    auto atime = prev->time;
    uhash awork = 0, k = 1;
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
    switch (algo) {
    case 1: k = 3;
        break;
    case 2: k = 3;
        break;
    case 3: k = 6;
        break;
    case 4: k = 6;
        break;
    }
    if (atime && ntime * awork / atime > 1)
        //target = 2^256 / work - 1
        target = (~(k * ntime * awork / atime) + 1) / (k * ntime * awork / atime);
    if (target > uhash(-1) >> 1) target = uhash(-1) >> 1;
}



void CBlock::mining(unsigned _amount) {
    while (_amount) {
        auto root = fork.back();
        fork.pop_back();
        auto &curnext = root->next;
        for (auto i = 1; i <= 4; i++) {
            auto pnblock = new CBlock(root, i);
            curnext.push_back(pnblock);
        }
        sort(curnext.begin(), curnext.end(), [](CBlock* a, CBlock* b) -> bool {
            return a->time < b->time;
        });
        for (auto &i : curnext) if (i != *curnext.begin()) delete i;
        curnext.erase(curnext.begin() + 1, curnext.end());
        fork.push_back(*curnext.begin());
        --_amount;
    }
}



CBlock::CBlock(CBlock* _prev, unsigned _algo) {
    int ahashrate;
    algo = _algo;
    prev = _prev;
    index = prev->index + 1;
    switch (algo) {
    case 1: ahashrate = 40;
        break;
    case 2: ahashrate = 80;
        break;
    case 3: ahashrate = index > 4000 && index < 6000 ? 150 : 20;
        break;
    case 4: ahashrate = 100;
        break;
    }
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
    unsigned a1 = 0, a2 = 0, a3 = 0, a4 = 0, t1 = 0, t2 = 0, t3 = 0, t4 = 0;
    uhash w1 = 0, w2 = 0, w3 = 0, w4 = 0;
    while (ptr->prev) {
        switch (ptr->algo) {
        case 1: a1++, t1 += ptr->time - ptr->prev->time, w1 += ~ptr->target / (ptr->target + 1) + 1;
            break;
        case 2: a2++, t2 += ptr->time - ptr->prev->time, w2 += ~ptr->target / (ptr->target + 1) + 1;
            break;
        case 3: a3++, t3 += ptr->time - ptr->prev->time, w3 += ~ptr->target / (ptr->target + 1) + 1;
            break;
        case 4: a4++, t4 += ptr->time - ptr->prev->time, w4 += ~ptr->target / (ptr->target + 1) + 1;
            break;
        }
        ptr = ptr->prev;
    }
    cout << dec << "amount " << '\t' << a1 << '\t' << a2 << '\t' << a3 << '\t' << a4 << endl;
    cout << dec << "work   " << '\t' << setw(10) << w1 << '\t' << setw(10) << w2 << '\t' << setw(10) << w3 <<
        '\t' << setw(10) << w4 << endl;
    cout << dec << "time   " << '\t' << (unsigned)(t1 / a1) << '\t' << (unsigned)(t2 / a2) <<
        '\t' << (unsigned)(t3 / a3) << '\t' << (unsigned)(t4 / a4) << endl;
}

void CBlock::PrintWork() {
    auto ptr = this;
    cout << dec << setfill(' ');
    cout << "  algo 1  algo 2  algo 3  algo 4    time index" << endl;
    while (!next.empty()) {
        switch (ptr->algo) {
        case 1:
            cout << setw(7) << ~ptr->target / (ptr->target + 1) + 1 <<
                ";" << setw(8) << ";" << setw(8) << ";" << setw(8);
            break;
        case 2:
            cout << setw(8) << ";" << setw(7) << ~ptr->target / (ptr->target + 1) + 1 <<
                ";" << setw(8) << ";" << setw(8);
            break;
        case 3:
            cout << setw(8) << ";" << setw(8) << ";" << setw(7) << ~ptr->target / (ptr->target + 1) + 1 <<
                ";" << setw(8);
            break;
        case 4:
            cout << setw(8) << ";" << setw(8) << ";" << setw(8) << ";" << setw(7) <<
                ~ptr->target / (ptr->target + 1) + 1;
            break;
        }
        cout << ";" << setw(7) << ptr->time << ";" << setw(6) << ptr->index << endl;
        if (ptr->next.empty()) break;
        ptr = ptr->next[0];
    }
}

