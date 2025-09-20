#pragma once

class Synapse {
public:
    Synapse(int src, int dst, double w = 1.0, double delay = 0.0)
        : srcId(src), dstId(dst), weight(w), delay(delay) {}

    size_t srcId;
    size_t dstId;
    double weight;
    double delay;
};