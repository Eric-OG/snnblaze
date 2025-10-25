#include <gtest/gtest.h>
#include "CalendarQueue.h"
#include <vector>
#include <string>
#include <algorithm>

// ---------- Basic order test ----------
TEST(CalendarQueueTest, BasicOrder) {
    CalendarQueue<std::string> cq(8, 1.0);

    cq.push(3.2, "Event A");
    cq.push(1.5, "Event B");
    cq.push(2.8, "Event C");
    cq.push(4.7, "Event D");

    std::vector<std::string> expected = {"Event B", "Event C", "Event A", "Event D"};
    for (const auto& exp : expected) {
        ASSERT_FALSE(cq.empty());
        auto ev = cq.top();
        std::cout << exp << " " << ev.value << std::endl;
        EXPECT_EQ(ev.value, exp);
        cq.pop();
    }
    EXPECT_TRUE(cq.empty());
}

// ---------- Continuous times ----------
TEST(CalendarQueueTest, ContinuousTimes) {
    CalendarQueue<int> cq(4, 1.0);

    cq.push(0.1, 1);
    cq.push(0.9, 2);
    cq.push(1.1, 3);
    cq.push(2.05, 4);

    std::vector<int> expected = {1, 2, 3, 4};
    for (int exp : expected) {
        auto ev = cq.top();
        EXPECT_EQ(ev.value, exp);
        cq.pop();
    }
    EXPECT_TRUE(cq.empty());
}

// ---------- Empty queue behavior ----------
TEST(CalendarQueueTest, EmptyQueue) {
    CalendarQueue<int> cq;
    EXPECT_TRUE(cq.empty());
    EXPECT_THROW(cq.top(), std::runtime_error);
}

// ---------- Resize behavior ----------
TEST(CalendarQueueTest, ResizeBehavior) {
    CalendarQueue<int> cq(4, 1.0); // small bucket count to trigger resize

    for (int i = 0; i < 10; ++i)
        cq.push(i * 0.5, i);

    for (int i = 0; i < 10; ++i) {
        auto ev = cq.top();
        EXPECT_EQ(ev.value, i);
        cq.pop();
    }
    EXPECT_TRUE(cq.empty());
}

// ---------- Randomized inserts ----------
TEST(CalendarQueueTest, RandomizedInsert) {
    CalendarQueue<int> cq(8, 1.0);

    std::vector<double> times = {3.1, 0.5, 2.7, 1.8, 0.2};
    std::vector<int> values = {10, 20, 30, 40, 50};

    for (size_t i = 0; i < times.size(); ++i)
        cq.push(times[i], values[i]);

    std::vector<int> expected = {50, 20, 40, 30, 10};
    for (int exp : expected) {
        auto ev = cq.top();
        EXPECT_EQ(ev.value, exp);
        cq.pop();
    }
    EXPECT_TRUE(cq.empty());
}

// ---------- Stress test (optional) ----------
TEST(CalendarQueueTest, StressTest) {
    CalendarQueue<int> cq(64, 1.0);
    const int N = 50000;

    std::vector<std::pair<double, int>> events;
    events.reserve(N);
    for (int i = 0; i < N; ++i) {
        double t = static_cast<double>(rand()) / RAND_MAX * 1000.0;
        events.emplace_back(t, i);
    }

    // Push all events
    for (auto& e : events)
        cq.push(e.first, e.second);

    // Sort by time
    std::sort(events.begin(), events.end(),
              [](const auto& a, const auto& b) { return a.first < b.first; });

    // Pop all events and check order
    for (auto& e : events) {
        auto ev = cq.top();
        EXPECT_EQ(ev.value, e.second);
        cq.pop();
    }
    EXPECT_TRUE(cq.empty());
}