#pragma once

class Synapse {
public:
    Synapse(int src, int dst, double w = 1.0, double delay = 0.0)
        : src_id(src), dst_id(dst), weight(w), delay(delay) {}

    size_t src_id;
    size_t dst_id;
    double weight;
    double delay;
};