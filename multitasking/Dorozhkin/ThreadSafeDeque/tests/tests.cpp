#include "stdafx.h"
#include "../include/ThreadSafeDeque.h"

void get(threadsafe_deque<int>& deque, int& sum)
{
    int x;
    sum = 0;
    for (auto i = 0; i < 1000; i++) {
        deque.wait_and_pop(x);
        sum += x;
    }
}

void insert(threadsafe_deque<int>& deque, int& sum)
{
    sum = 0;
    for (auto i = 0; i < 1000; i++) {
        sum += i;
        // std::cout << i << std::endl;
        // std::this_thread::sleep_for(std::chrono::milliseconds(100));
        if (i % 2) {
            deque.push(i);
        } else {
            auto tmp = i;
            deque.emplace(std::move(tmp));
        }
    }
}

void stress(threadsafe_deque<int>& stressdeque)
{
    int tmp;
    for (auto i = 0; i < 1000; i++) {
        stressdeque.push(i);
        stressdeque.wait_and_pop(tmp);
    }
}

TEST(SafeDequeTesting, OneThread) {
    threadsafe_deque<int> a;
    int x = 0;
    a.push(1);
    a.push(2);
    EXPECT_TRUE(a.try_pop(x));
    EXPECT_EQ(x, 2);

    a.emplace(3);
    a.wait_and_pop(x);
    EXPECT_EQ(x, 3);
    a.wait_and_pop(x);
    EXPECT_EQ(x, 1);
    EXPECT_FALSE(a.try_pop(x));
}

TEST(SafeDequeTesting, MultiThread) {
    int sum1 = 0, sum2 = 0;
    threadsafe_deque<int> deque;
    std::thread insertTread(insert, std::ref(deque), std::ref(sum1));
    std::thread getTread(get, std::ref(deque), std::ref(sum2));

    insertTread.join();
    getTread.join();
    EXPECT_EQ(sum1, sum2);
    // insertTread.detach();
    // getTread.detach();
    // 
    // std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

TEST(SafeDequeTesting, StressMultiThread) {
    try {
        threadsafe_deque<int> stressdeque;
        std::vector<std::future<void>> vf;
        for (auto i = 0; i < 100; i++) {
            vf.push_back(std::async(std::launch::async, stress, std::ref(stressdeque)));
        }
        for (auto& f : vf) {
            f.wait();
        }
    } catch(...) {
        EXPECT_FALSE(true);
    }
}


TEST(SafeDequeTesting, Vector) {
    threadsafe_deque<int, std::vector<int>> a;
    int x = 0;
    a.push(1);
    a.push(2);
    EXPECT_TRUE(a.try_pop(x));
    EXPECT_EQ(x, 2);

    a.emplace(3);
    a.wait_and_pop(x);
    EXPECT_EQ(x, 3);
    a.wait_and_pop(x);
    EXPECT_EQ(x, 1);
    EXPECT_FALSE(a.try_pop(x));
}
