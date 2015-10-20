#include <stdexcept>
#include <string>
#include <cstdio>

using namespace std;

enum class AllocErrorType {
    InvalidFree,
    NoMemory,
};

struct item {
    void * p;
    size_t sz;
};

class AllocError: std::runtime_error {
private:
    AllocErrorType type;

public:
    AllocError(AllocErrorType _type, std::string message):
            runtime_error(message),
            type(_type)
    {}

    AllocErrorType getType() const { return type; }
};

class Allocator;

class Pointer {
public:
    Allocator * p_a = nullptr;
    size_t indx = 0;
    Pointer(void) {};
    Pointer(int ind, Allocator * al);
    void delete_me(void) { delete this; }
    ~Pointer();
    void * get() const;
};

class Allocator {
public:
    void * b;
    size_t s;
    int *mask;
    size_t index = 0;
    struct item pp[65536] = {};
    Allocator(void *base, size_t size) { b = base; s = size; mask = (int *)malloc(size * sizeof(int)); }
    Pointer alloc(size_t);
    void realloc(Pointer &, size_t);
    void free(Pointer &);
    void defrag();
    std::string dump() {}
};
