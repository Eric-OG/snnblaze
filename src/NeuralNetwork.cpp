#include "NeuralNetwork.h"
#include <memory>
#include <stdexcept>
#include <vector>

void NeuralNetwork::addNeuronPopulation(size_t size, std::shared_ptr<Neuron> neuron_type){
    size_t prev_size = neuron_states_.size();
    // Increase vectors to handle new state variables
    this->neuron_states_.resize(prev_size + size, neuron_type->getInitValue());
    this->neuron_last_spikes_.resize(prev_size + size, 0.0);
    this->neuron_types_.resize(prev_size + size, neuron_type);

    auto new_pop = std::make_unique<NeuronPopulation>(
        size,
        neuron_type,
        &(this->neuron_states_[prev_size]),
        &(this->neuron_last_spikes_[prev_size])
    );
    this->neuron_populations_.push_back(std::move(new_pop));
}

void NeuralNetwork::addSynapse(const Synapse& synapse) {
    if (synapse.srcId >= neuron_states_.size() || synapse.dstId >= neuron_states_.size()) {
        throw std::out_of_range("Neuron index out of bounds for synapse");
    }
    adjacency_[synapse.srcId].push_back(synapse);
}

void NeuralNetwork::setSpikeMonitor(std::unique_ptr<SpikeMonitor> monitor){
    spike_monitor_ = std::move(monitor);
}

void NeuralNetwork::scheduleSpikeEvent(double time, size_t neuronIndex, double weight) {
    if (neuronIndex >= neuron_states_.size()) throw std::out_of_range("Neuron index out of bounds");
    eventQueue_.push(SpikeEvent{time, neuronIndex, weight});
}

size_t NeuralNetwork::size() const {
    return neuron_states_.size();
}

void NeuralNetwork::run(double T) {
    while (!eventQueue_.empty()) {
        SpikeEvent e = eventQueue_.top();
        eventQueue_.pop();

        if (e.time > T) break;

        // Deliver input and update state
        if (neuron_types_[e.target_index]->update(
            e.time,
            &neuron_states_[e.target_index],
            &neuron_last_spikes_[e.target_index], 
            e.weight)
        ) {
            if (spike_monitor_) spike_monitor_->onSpike(e.time, e.target_index);
            
            // Schedules spike events to post-synaptic neurons
            for (const auto& syn : adjacency_[e.target_index]) {
                double arrivalTime = e.time + syn.delay;
                eventQueue_.push(SpikeEvent{arrivalTime, syn.dstId, syn.weight});
            }
        }
    }
}