#include<iostream>
#include<map>
#include<cstdlib>
using namespace std;


struct item{
    void * p;
    int size;
};

class Palloc {
   void * p;
   struct item * pp[256];
   pp[0]->p = p;
   int index = 0;
   public:
   Palloc(int x) { p = malloc(x); }
   Palloc() { p = NULL; }
   //void *operator->() { return;}
   void *new_alloc(int);
   //void *new_realloc(void *, int);
   int new_free(void *);


};

void *Palloc::new_alloc(int size) {
    void *new_item = pp[index]->p;
    pp[index]->size = size;
    pp[++index] = p + size;
    return pp[index]->p;
}

int Palloc::new_free(void * p) {
    for(auto i = 0; i < index; i++) {
        if (pp[i]->p == p) {
            p[i]->size = 0;
            break;
        }
    }
    p[i] = p[index--];
}

int
main(void)
{
    Palloc smartp (256);
    void *p1  = smartp.new_alloc(30);


}

