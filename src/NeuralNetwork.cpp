#include "NeuralNetwork.h"
#include <memory>
#include <stdexcept>
#include <vector>
#include <iostream>
#include <limits>
#include <omp.h>

// Always initialize with 1 thread
NeuralNetwork::NeuralNetwork() : num_exec_threads_(1) {
    omp_set_num_threads(num_exec_threads_);
    sim_time = 0.0;
}

void NeuralNetwork::add_neuron_population(size_t size, std::shared_ptr<Neuron> neuron_type) {
    size_t prev_size = neuron_states_.size();
    // Increase vectors to handle new state variables
    neuron_states_.resize(prev_size + size, neuron_type->get_init_value());
    neuron_last_spikes_.resize(prev_size + size, -std::numeric_limits<double>::infinity());
    neuron_last_updates_.resize(prev_size + size, 0.0);
    neuron_types_.resize(prev_size + size, neuron_type);
    adjacency_.resize(prev_size + size);

    // Recalculate pointers to new vector position
    size_t offset = 0;
    for (auto& pop : neuron_populations_) {
        pop->state_addr       = &(neuron_states_[offset]);
        pop->last_spike_addr  = &(neuron_last_spikes_[offset]);
        pop->last_update_addr = &(neuron_last_updates_[offset]);
        offset += pop->n_neurons;   // move to next block
    }

    auto new_pop = std::make_unique<NeuronPopulation>(
        size,
        neuron_type,
        &(neuron_states_[prev_size]),
        &(neuron_last_spikes_[prev_size]),
        &(neuron_last_updates_[prev_size])
    );
    neuron_populations_.push_back(std::move(new_pop));
}

void NeuralNetwork::add_synapse(const Synapse& synapse) {
    if (synapse.src_id >= neuron_states_.size() || synapse.dst_id >= neuron_states_.size()) {
        throw std::out_of_range("Neuron index out of bounds for synapse");
    }
    adjacency_[synapse.src_id].push_back(synapse);
}

void NeuralNetwork::set_spike_monitor(std::shared_ptr<SpikeMonitor> monitor) {
    spike_monitor_ = monitor;
}

void NeuralNetwork::set_state_monitor(std::shared_ptr<StateMonitor> monitor) {
    state_monitor_ = monitor;
}

void NeuralNetwork::schedule_spike_event(double time, size_t neuron_index, double weight) {
    if (neuron_index >= neuron_states_.size()) throw std::out_of_range("Neuron index out of bounds");
    // Events added after current sim_time
    event_queue_.push(SpikeEvent{sim_time + time, neuron_index, weight});
}

size_t NeuralNetwork::size() const {
    return neuron_states_.size();
}

void NeuralNetwork::run(double T) {
    // Schedule periodic update events
    if (state_monitor_) {
        for (double t = sim_time; t <= sim_time+T; t += state_monitor_->get_reading_interval())
            event_queue_.push(UpdateEvent{t});
    }

    // Main simulation loop
    while (!event_queue_.empty()) {
        // Retrieve event and test if within simulation time
        Event e = event_queue_.top();
        event_queue_.pop();
        // Get the time regardless of event type
        auto get_time = [](const auto& ev) { return ev.time; };
        if (std::visit(get_time, e) > sim_time+T) break;

        if (std::holds_alternative<SpikeEvent>(e)) {
            auto& spike = std::get<SpikeEvent>(e);

            neuron_types_[spike.target_index]->decay(
                spike.time,
                &neuron_states_[spike.target_index],
                &neuron_last_spikes_[spike.target_index], 
                &neuron_last_updates_[spike.target_index],
                1
            );

            if (neuron_types_[spike.target_index]->receive(
                spike.time,
                spike.weight,
                &neuron_states_[spike.target_index],
                &neuron_last_spikes_[spike.target_index], 
                &neuron_last_updates_[spike.target_index]
            )) {
                if (spike_monitor_) spike_monitor_->on_spike(spike.time, spike.target_index);

                // Schedules spike events to post-synaptic neurons
                for (const auto& syn : adjacency_[spike.target_index]) {
                    double arrivalTime = spike.time + syn.delay;
                    event_queue_.push(SpikeEvent{arrivalTime, syn.dst_id, syn.weight});
                }
            }
        }
        if (std::holds_alternative<UpdateEvent>(e)) {
            auto& update = std::get<UpdateEvent>(e);

            // Update all neurons to current time
            for (const auto& pop : neuron_populations_) {
                pop->neuron_class->decay(
                    update.time,
                    pop->state_addr,
                    pop->last_spike_addr,
                    pop->last_update_addr,
                    pop->n_neurons
                );
            }
            if (state_monitor_)
                state_monitor_->on_read(update.time, neuron_states_);
        }
    }

    // Update simulation time for subsequent runs
    sim_time += T;
}

void NeuralNetwork::reset_monitors() {
    // Reset monitors
    if (spike_monitor_) spike_monitor_->reset_spikes();
    if (state_monitor_) state_monitor_->reset_recording();
}

void NeuralNetwork::set_num_exec_threads(size_t n) {
    num_exec_threads_ = n;
    omp_set_num_threads(num_exec_threads_);
}

size_t NeuralNetwork::get_num_exec_threads() const {
    return num_exec_threads_;
}