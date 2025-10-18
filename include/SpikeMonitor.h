#pragma once
#include <vector>
#include <memory>
#include "Event.h"

class SpikeMonitor {
public:
    void on_spike(double time, size_t neuron_id);
    void reset_spikes();

    // Public for direct access from python
    // Pair: (time, neuron_id)
    std::vector<std::pair<double, size_t>> spike_list;
};