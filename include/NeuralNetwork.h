#pragma once

#include <vector>
#include <memory>
#include <queue>
#include "Neuron.h"
#include "NeuronPopulation.h"
#include "Synapse.h"
#include "SpikeEvent.h"
#include "SpikeMonitor.h"

// NeuralNetwork: event-driven simulation engine
class NeuralNetwork {
public:
    NeuralNetwork() = default;
    ~NeuralNetwork() = default;

    void add_neuron_population(size_t size, std::shared_ptr<Neuron> neuron_type);

    void add_synapse(const Synapse& synapse);

    // Schedule external input event
    void schedule_spike_event(double time, size_t neuronIndex, double weight);

    void set_spike_monitor(std::shared_ptr<SpikeMonitor> monitor);

    // Run simulation until time T
    void run(double T);

    size_t size() const;

private:
    // Each population may have different types (properties)
    std::vector<std::unique_ptr<NeuronPopulation>> neuron_populations_; 
    // State vectors aggregate all populations - exploiting cache locality
    std::vector<double> neuron_last_spikes_;
    std::vector<double> neuron_states_;
    // Fast (O(1)) lookup to neuron types
    std::vector<std::shared_ptr<Neuron>> neuron_types_;

    std::vector<std::vector<Synapse>> adjacency_;
    std::priority_queue<SpikeEvent> eventQueue_;

    // Monitors (optional)
    std::shared_ptr<SpikeMonitor> spike_monitor_;
};