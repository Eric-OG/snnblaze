// Here we instantiate a neural population composed of neurons of the same type, without including synapses.
// This approach improves parallelism and cache locality.
#include <memory>
#include "Neuron.h"

struct NeuronPopulation {
    NeuronPopulation(size_t n_neurons,
                     std::shared_ptr<Neuron> neuron_class,
                     double* state_addr,
                     double* last_spike_addr,
                     double* last_update_addr)
        : n_neurons(n_neurons),
          neuron_class(std::move(neuron_class)), // Neuron class can only belong to a single population
          state_addr(state_addr),
          last_spike_addr(last_spike_addr),
          last_update_addr(last_update_addr) {}

    // Address in the AoS where the states start
    double* state_addr;
    double* last_spike_addr;
    double* last_update_addr;

    size_t n_neurons;     // Number of neurons (state array size)
    std::shared_ptr<Neuron> neuron_class;  // Neuron class - defines computational model and parameters
};