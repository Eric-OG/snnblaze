#include <vector>
#include <list>
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
    std::vector<std::list<Event>> buckets;
    size_t bucket_count;
    TimeT bucket_width;
    TimeT min_time;      // Current simulation time (last popped event)
    size_t size_;
    size_t last_bucket;  // Index of last bucket processed

    static constexpr double DEFAULT_WIDTH_FACTOR = 2.0;
    static constexpr size_t DEFAULT_BUCKETS = 128;

public:
    explicit CalendarQueue(size_t initialBuckets = DEFAULT_BUCKETS, TimeT initialWidth = 1.0)
        : bucket_count(initialBuckets),
          bucket_width(initialWidth),
          min_time(std::numeric_limits<TimeT>::lowest()),
          size_(0),
          last_bucket(0)
    {
        buckets.resize(bucket_count);
    }

    bool empty() const noexcept { return size_ == 0; }
    size_t size() const noexcept { return size_; }

    // Return earliest event (does not remove)
    const Event& top() {
        if (empty())
            throw std::runtime_error("CalendarQueue::top(): empty queue");

        size_t idx = find_next_bucket();
        return buckets[idx].front();
    }

    // Remove earliest event
    void pop() {
        if (empty()) return;

        size_t idx = find_next_bucket();
        auto& bucket = buckets[idx];
        auto ev = bucket.front();
        bucket.pop_front();
        --size_;

        // Update internal clock and last bucket index
        min_time = ev.time;
        last_bucket = idx;

        // Resizes only after popping (accumulates events with push first)
        if (need_resize())
            resize_buckets(bucket_count * 2, bucket_width * DEFAULT_WIDTH_FACTOR);
    }

    // Insert new event
    void push(TimeT time, const T& value) { push(Event{time, value}); }

    void push(const Event& e) {
        size_t idx = bucketIndex(e.time);
        auto& bucket = buckets[idx];

        // Maintain order inside bucket
        auto it = std::find_if(bucket.begin(), bucket.end(),
                               [&](const Event& ev) { return ev.time > e.time; });
        bucket.insert(it, e);
        ++size_;
    }

private:
    // Compute which bucket an event belongs to
    size_t bucketIndex(TimeT time) const noexcept {
        TimeT dt = std::floor(time / bucket_width);
        return static_cast<size_t>(dt) % bucket_count;
    }

    // Incrementally find the next non-empty bucket
    size_t find_next_bucket() {
        size_t i = last_bucket;

        for (size_t scanned = 0; scanned < bucket_count; ++scanned) {
            auto& bucket = buckets[i];
            if (!bucket.empty()) {
                // Find first event >= min_time
                if (bucket.front().time >= min_time)
                    return i;
            }
            i = (i + 1) % bucket_count;
        }

        // Should never happen if queue is non-empty
        throw std::runtime_error("CalendarQueue::find_next_bucket(): no event found");
    }

    bool need_resize() const noexcept {
        return size_ > 2 * bucket_count;
    }

    // Rebuild with new bucket width and count
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
        size_ = 0;

        for (auto& e : all)
            push(e);
    }
};