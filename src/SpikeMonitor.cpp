#include "SpikeMonitor.h"
#include <fstream>
#include <stdexcept>

namespace {
    // Keep the file stream hidden in the .cpp (not exposed in the header)
    std::ofstream spike_file;
}

void SpikeMonitor::setOutputFile(const std::string& filename) {
    output_file = filename;
    spike_file.open(output_file, std::ios::out | std::ios::trunc);
    if (!spike_file.is_open()) {
        throw std::runtime_error("Failed to open output file: " + output_file);
    }
}

void SpikeMonitor::onSpike(int neuron_id, double time) {
    if (!spike_file.is_open()) {
        throw std::runtime_error("Output file not open. Call setOutputFile() first.");
    }

    // Assuming SpikeEvent has fields like neuronId and time
    spike_file << neuron_id << "," << time << "\n";
}

void SpikeMonitor::closeFile() {
    if (spike_file.is_open()) {
        spike_file.close();
    }
}
