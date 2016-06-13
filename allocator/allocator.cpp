#include "allocator.h"
#include<iostream>
#include<map>
#include<cstdlib>
#include<cstdio>
using namespace std;


Pointer::Pointer(int ind, Allocator * al)
{
        indx = ind;
        p_a = al;
        for(auto i = 0; i < al->pp[ind].sz; i++) {
            al->mask[(char *)al->pp[ind].p - (char *)al->b + i] = 1;
        }
}


Pointer Allocator::alloc(size_t N) {
    int counter = 0;
    void *place = nullptr;
    for (auto i = 0; i < s; i++) {
        if (mask[i] != 1) {
            if (counter == 0) place = (char *)b + i;
            counter++;
            if (counter == N) break;
        } else {
            counter = 0;
            place = nullptr;
        }
    }
    if (counter != N) place = nullptr;
    if (!place) throw AllocError(AllocErrorType::NoMemory, "Wrong alloc.");
    pp[index].p = place;
    pp[index].sz = N;
    Pointer pt(index, this);
    index++;
    return pt;
}


Pointer::~Pointer() {}


void Allocator::free(Pointer &p) {
    for(auto i = 0; i < pp[p.indx].sz; i++) {
        mask[(char *)pp[p.indx].p - (char *)b + i] = 0;
    }
    p.p_a->pp[p.indx].p = nullptr;
}


void * Pointer::get() const { return this->p_a->pp[indx].p; }


void Allocator::defrag()
{
    int i = 0;
    int flag = 0;
    char * tmp;
    while (i <= s) {
        if (mask[i] == 0) {
            flag = 0;
            for(auto k = i; k < s - 1; k++) {
                mask[k] = mask[k + 1];
                *((char *)b + k) = *((char *)b + k + 1);
            }
            for(auto k = 0; k < index; k++) {
                if((char *)pp[k].p - (char *)b >= i) {
                    flag = 1;
                    tmp = (char *)pp[k].p;
                    pp[k].p = --tmp;
                }
            }
            if (!flag) break;
        } else i++;
    }
}


void Allocator::realloc(Pointer &p, size_t N)
{
    int flag = 0, flag2 = 0;
    if (!p.p_a) {
        p = Pointer(index, this);
        pp[index].p = nullptr;
        pp[index].sz = N;
        index++;
        flag = 1;
        flag2 = 1;
    } else {
        for(auto i = (char *)p.get() - (char *)b + p.p_a->pp[p.indx].sz; i < N; i++) {
            if (mask[i] != 0) flag = 1;
        }
    }
    if (flag) {
        if (!flag2) free(p);
        int counter = 0;
        void *place = nullptr;
        for (auto i = 0; i < s; i++) {
            if (mask[i] != 1) {
                if (counter == 0) place = (char *)b + i;
                counter++;
                if (counter == N) break;
            } else {
               counter = 0;
               place = nullptr;
            }
        }
        if (counter != N) place = nullptr;
        if (!place) throw AllocError(AllocErrorType::NoMemory, "Wrong alloc.");
        p.p_a->pp[p.indx].p = place;
        p.p_a->pp[p.indx].sz = N;
    } else {
        for(auto i = (char *)p.get() - (char *)b + p.p_a->pp[p.indx].sz; i < N; i++) {
            mask[i] = 1;
        }
        p.p_a->pp[p.indx].sz = N;
    }
}
