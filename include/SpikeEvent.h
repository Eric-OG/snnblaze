#pragma once

#include <memory>

struct SpikeEvent {
    double time;
    size_t target_index;
    double weight;

    // Needed for priority_queue (min-heap)
    bool operator<(const SpikeEvent& other) const {
        return time > other.time;
    }
};