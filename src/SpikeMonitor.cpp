#include "SpikeMonitor.h"

void SpikeMonitor::on_spike(int neuron_id, double time) {
    this->spike_list.emplace_back(time, neuron_id);
}

void SpikeMonitor::reset_spikes() {
    this->spike_list.clear();
}