#include "ThreadSafeDeque.h"

int main(int argc, char const *argv[]) {
    threadsafe_deque<int> a;
    int x = 0, y = 0;
    a.push(1);
    a.push(2);
    a.try_pop(x);
    std::cout << x << std::endl;

    a.emplace(3);
    a.wait_and_pop(x);
    std::cout << x << std::endl;

    return 0;
}
