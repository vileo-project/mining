// Copyright (c) 2019-2019 The Vileo Project developers, https://vileo.org/
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#include "block.h"

unsigned CBlock::ntime = 60;
vector <CBlock*> CBlock::fork;

uhash CBlock::algopart() {
    switch (algo) {
    case 1: return 3;
        break;
    case 2: return  3;
        break;
    case 3: return  6;
        break;
    case 4: return  6;
        break;
    }
    return 1;
}
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
    if (atime && ntime * awork / atime > 1)
        //target = 2^256 / work - 1
        target = (~(algopart() * ntime * awork / atime) + 1) / (algopart() * ntime * awork / atime);
    if (target > uhash(-1) >> 1) target = uhash(-1) >> 1;
}



void CBlock::mining(unsigned _amount) {
    while (_amount) {
        auto a51block = 1000;
        vector <CBlock*> tmp;
        while (!fork.empty()) {
            auto root = fork.back();
            fork.pop_back();
            double awork = 1;
            auto &curnext = root->next;
            for (auto i = 1; i <= 4; i++) {
                auto pnblock = new CBlock(root, i);
                curnext.push_back(pnblock);
                awork *= pow(~pnblock->target / (pnblock->target + 1) + 1, 1.0 / pnblock->algopart());
            }
            sort(curnext.begin(), curnext.end(), [](CBlock* a, CBlock* b) -> bool {
                return a->time < b->time;
            });
            curnext[1]->a51 = 1;
            for (auto &i : curnext) {
                if (i != *curnext.begin() &&
                    i != *(curnext.begin() + ((*curnext.begin())->index == a51block ? 1 : 0))) delete i;
                else i->work = i->prev->work + (uhash)awork;
            }
            curnext.erase(curnext.begin() + 1 + ((*curnext.begin())->index == a51block ? 1 : 0), curnext.end());
            tmp.insert(tmp.end(), curnext.begin(), curnext.end());
        }
        fork = tmp;
        --_amount;
    }
}



CBlock::CBlock(CBlock* _prev, unsigned _algo) {
    int ahashrate;
    algo = _algo;
    prev = _prev;
    index = prev->index + 1;
    if (prev->a51) a51 = 1;
    switch (algo) {
    case 1: ahashrate = a51 ? 160 : 40;
        break;
    case 2: ahashrate = a51 ? 40 : 80;
        break;
    case 3: ahashrate = a51 ? 10 : 20;
        break;
    case 4: ahashrate = a51 ? 50 : 100;
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

void CBlock::PrintFork() {
    vector <CBlock*> vp{ this, this };
    cout << dec << setfill(' ');
    cout << "        basic         fork   time basic    time fork       index" << endl;
    while (!next.empty()) {
        for (auto &i : vp) {
            cout << setw(12) << i->work - (i->prev ? i->prev->work : 0) << ";";
        }
        for (auto &i : vp) {
            cout << setw(12) << i->time << ";";
            if (i->next.size() > 1 && vp[1] != vp[0]) i = i->next[1];
            else i = i->next[0];
        }
        cout << setw(12) << vp[0]->index << endl;
        if (vp[1]->next.empty()) break;
    }
}

/*void CBlock::Print() {
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
}*/

void CBlock::PrintAlgo() {
    for (auto ptr : fork) {//auto ptr = fork.back();
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
    cout << "average work \t";
    for (auto &i : fork) cout << i->work << " \t";
    cout << endl;
}

/*void CBlock::PrintWork() {
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
}*/

