#include <vector>
#include <limits>
#include <cmath>
#include <stdexcept>
#include <algorithm>

template<class T, class TimeT = double>
class CalendarQueue {
public:
    struct Event {
        TimeT time;
        T value;
    };

private:
    std::vector<std::vector<Event>> buckets;
    size_t bucket_count;
    TimeT bucket_width;
    TimeT min_time;        // Current simulation time (last popped event)
    size_t size_;
    size_t last_bucket;    // Index of last bucket processed
    static constexpr double DEFAULT_WIDTH_FACTOR = 1.0;
    static constexpr size_t DEFAULT_BUCKETS = 10000;
    static constexpr TimeT EPS = 1e-12; // small epsilon for floating-point safety

public:
    explicit CalendarQueue(size_t initialBuckets = DEFAULT_BUCKETS, TimeT initialWidth = 0.01)
        : bucket_count(initialBuckets),
          bucket_width(initialWidth),
          min_time(std::numeric_limits<TimeT>::lowest()),
          size_(0),
          last_bucket(0)
    {
        buckets.resize(bucket_count);
        for (auto& b : buckets)
            b.reserve(4); // small initial reserve for cache efficiency
    }

    bool empty() const noexcept { return size_ == 0; }
    size_t size() const noexcept { return size_; }

    const Event& top() {
        if (empty())
            throw std::runtime_error("CalendarQueue::top(): empty queue");
        size_t idx = find_next_bucket();
        auto& bucket = buckets[idx];
        // Ensure bucket is sorted
        std::sort(bucket.begin(), bucket.end(), [](const Event& a, const Event& b){ return a.time < b.time; });
        return bucket.front();
    }

    void pop() {
        if (empty()) return;
        size_t idx = find_next_bucket();
        auto& bucket = buckets[idx];
        // Sort before popping if not already
        std::sort(bucket.begin(), bucket.end(), [](const Event& a, const Event& b){ return a.time < b.time; });
        auto ev = bucket.front();
        bucket.erase(bucket.begin());
        --size_;
        min_time = ev.time;
        last_bucket = idx;
    }

    void push(TimeT time, const T& value) { push(Event{time, value}); }

    void push(const Event& e) {
        if (empty()) {
            min_time = e.time;
            last_bucket = 0;
        }
        if (need_resize())
            resize_buckets(bucket_count * 2, bucket_width * DEFAULT_WIDTH_FACTOR);

        size_t idx = bucketIndex(e.time);
        buckets[idx].push_back(e); // append, no per-bucket sorting
        ++size_;
    }

private:
    size_t bucketIndex(TimeT time) const noexcept {
        TimeT dt = (time / bucket_width) + EPS;
        return static_cast<size_t>(std::floor(dt)) % bucket_count;
    }

    size_t find_next_bucket() {
        size_t idx = last_bucket;
        for (size_t scanned = 0; scanned < bucket_count; ++scanned) {
            if (!buckets[idx].empty())
                return idx;
            idx = (idx + 1) % bucket_count;
        }
        throw std::runtime_error("CalendarQueue::find_next_bucket(): no event found");
    }

    bool need_resize() const noexcept {
        return size_ > 2 * bucket_count;
    }

    void resize_buckets(size_t new_count, TimeT new_width) {
        std::vector<Event> all;
        all.reserve(size_);
        for (auto& b : buckets)
            for (auto& e : b)
                all.push_back(e);

        bucket_count = new_count;
        bucket_width = new_width;
        buckets.clear();
        buckets.resize(bucket_count);
        for (auto& b : buckets)
            b.reserve(4);
        size_ = 0;
        last_bucket = 0;

        for (auto& e : all)
            push(e);
    }
};
