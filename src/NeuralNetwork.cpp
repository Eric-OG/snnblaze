#include "NeuralNetwork.h"
#include <memory>
#include <stdexcept>
#include <vector>

void NeuralNetwork::add_neuron_population(size_t size, std::shared_ptr<Neuron> neuron_type) {
    size_t prev_size = neuron_states_.size();
    // Increase vectors to handle new state variables
    this->neuron_states_.resize(prev_size + size, neuron_type->get_init_value());
    this->neuron_last_spikes_.resize(prev_size + size, 0.0);
    this->neuron_types_.resize(prev_size + size, neuron_type);
    this->adjacency_.resize(prev_size + size);

    auto new_pop = std::make_unique<NeuronPopulation>(
        size,
        neuron_type,
        &(this->neuron_states_[prev_size]),
        &(this->neuron_last_spikes_[prev_size])
    );
    this->neuron_populations_.push_back(std::move(new_pop));
}

void NeuralNetwork::add_synapse(const Synapse& synapse) {
    if (synapse.srcId >= neuron_states_.size() || synapse.dstId >= neuron_states_.size()) {
        throw std::out_of_range("Neuron index out of bounds for synapse");
    }
    adjacency_[synapse.srcId].push_back(synapse);
}

void NeuralNetwork::set_spike_monitor(std::shared_ptr<SpikeMonitor> monitor) {
    spike_monitor_ = monitor;
}

void NeuralNetwork::set_state_monitor(std::shared_ptr<StateMonitor> monitor) {
    state_monitor_ = monitor;
}

void NeuralNetwork::schedule_spike_event(double time, size_t neuron_index, double weight) {
    if (neuron_index >= neuron_states_.size()) throw std::out_of_range("Neuron index out of bounds");
    eventQueue_.push(SpikeEvent{time, neuron_index, weight});
}

size_t NeuralNetwork::size() const {
    return neuron_states_.size();
}

void NeuralNetwork::run(double T) {
    // Schedule periodic update events
    if (state_monitor_) {
        for (double t = 0.0; t <= T; t += state_monitor_->get_reading_interval())
            eventQueue_.push(UpdateEvent{t});
    }

    // Main simulation loop
    while (!eventQueue_.empty()) {
        // Retrieve event and test if within simulation time
        Event e = eventQueue_.top();
        eventQueue_.pop();
        // Get the time regardless of event type
        auto get_time = [](const auto& ev) { return ev.time; };
        if (std::visit(get_time, e) > T) break;

        if (std::holds_alternative<SpikeEvent>(e)) {
            auto& spike = std::get<SpikeEvent>(e);

            if (neuron_types_[spike.target_index]->update(
                spike.time,
                &neuron_states_[spike.target_index],
                &neuron_last_spikes_[spike.target_index], 
                spike.weight)
            ) {
                if (spike_monitor_) spike_monitor_->on_spike(spike.time, spike.target_index);

                // Schedules spike events to post-synaptic neurons
                for (const auto& syn : adjacency_[spike.target_index]) {
                    double arrivalTime = spike.time + syn.delay;
                    eventQueue_.push(SpikeEvent{arrivalTime, syn.dstId, syn.weight});
                }
            }
        }
        if (std::holds_alternative<UpdateEvent>(e)) {
            auto& update = std::get<UpdateEvent>(e);
            // Update all neurons to current time
            for (size_t i = 0; i < neuron_states_.size(); ++i) {
                neuron_types_[i]->update(
                    update.time,
                    &neuron_states_[i],
                    &neuron_last_spikes_[i],
                    0.0
                );
            }
            state_monitor_->on_read(update.time, neuron_states_);
        }
    }
}