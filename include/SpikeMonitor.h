#pragma once
#include <string>
#include <vector>
#include <memory>
#include "SpikeEvent.h"

class SpikeMonitor {
public:
    void on_spike(int neuron_id, double time);
    void reset_spikes();

    // Public for direct access from python
    std::vector<std::pair<double, size_t>> spike_list;
};