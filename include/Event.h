#pragma once

#include <variant>

struct SpikeEvent {
    double time;
    size_t target_index;
    double weight;
};

struct UpdateEvent {
    double time;
};

using Event = std::variant<SpikeEvent, UpdateEvent>;

// Needed to stablish priority in the event queue - time field is obligatory
struct EventCompare {
    bool operator()(const Event& a, const Event& b) const {
        auto get_time = [](const auto& ev) { return ev.time; };
        return std::visit(get_time, a) > std::visit(get_time, b);
    }
};