#include "SpikeMonitor.h"

void SpikeMonitor::on_spike(double time, size_t neuron_id) {
    this->spike_list.emplace_back(time, neuron_id);
}

void SpikeMonitor::reset_spikes() {
    this->spike_list.clear();
}