#include "gtest/gtest.h"
#include "NeuralNetwork.h"
#include "LIFNeuron.h"
#include <memory>
#include <vector>

class NeuralNetworkTest : public ::testing::Test {
protected:
    // Test neuron parameters
    double tau_m = 10.0;       // membrane time constant
    double C_m = 1.0;          // membrane capacitance
    double v_rest = 0.0;       // resting potential
    double v_reset = 0.0;      // reset potential after spike
    double v_thresh = 1.0;     // threshold
    double refractory = 2.0;   // refractory period
};

// Adding neuron populations increases network size
TEST_F(NeuralNetworkTest, AddNeuronPopulation) {
    NeuralNetwork net;
    auto neuron_type = std::make_shared<LIFNeuron>(tau_m, C_m, v_rest, v_reset, v_thresh, refractory);

    net.add_neuron_population(5, neuron_type);
    EXPECT_EQ(net.size(), 5);

    net.add_neuron_population(3, neuron_type);
    EXPECT_EQ(net.size(), 8);
}

// Adding synapses and verifying out-of-range throws
TEST_F(NeuralNetworkTest, AddSynapse) {
    NeuralNetwork net;
    auto neuron_type = std::make_shared<LIFNeuron>(tau_m, C_m, v_rest, v_reset, v_thresh, refractory);
    net.add_neuron_population(2, neuron_type);

    // Valid synapse
    Synapse syn{0, 1, 5.0, 1.0};
    EXPECT_NO_THROW(net.add_synapse(syn));

    // Invalid synapse (out of bounds)
    Synapse bad_syn{0, 5, 5.0, 1.0};
    EXPECT_THROW(net.add_synapse(bad_syn), std::out_of_range);
}

// Scheduling spike events and checking out-of-range
TEST_F(NeuralNetworkTest, ScheduleSpikeEvent) {
    NeuralNetwork net;
    auto neuron_type = std::make_shared<LIFNeuron>(tau_m, C_m, v_rest, v_reset, v_thresh, refractory);
    net.add_neuron_population(2, neuron_type);

    EXPECT_NO_THROW(net.schedule_spike_event(0.0, 0, 10.0));
    EXPECT_THROW(net.schedule_spike_event(1.0, 5, 10.0), std::out_of_range);
}

// Running network propagates spikes correctly
TEST_F(NeuralNetworkTest, SpikePropagation) {
    NeuralNetwork net;
    auto neuron_type = std::make_shared<LIFNeuron>(tau_m, C_m, v_rest, v_reset, v_thresh, refractory);

    // Create 2 neurons
    net.add_neuron_population(2, neuron_type);
    // Connect neuron 0 -> neuron 1
    Synapse syn{0, 1, 1.5, 1.0};
    net.add_synapse(syn);

    // Set spike monitor
    auto monitor = std::make_shared<SpikeMonitor>();
    SpikeMonitor* monitor_ptr = monitor.get();
    net.set_spike_monitor(monitor);

    // Schedule initial spike for neuron 0
    net.schedule_spike_event(0.0, 0, 1.5);  // strong enough to spike

    // Run network
    net.run(5.0);

    // Expect both neuron 0 and neuron 1 to have spiked
    EXPECT_EQ(monitor_ptr->spike_list.size(), 2);
    EXPECT_EQ(monitor_ptr->spike_list[0].first, 0.);
    EXPECT_EQ(monitor_ptr->spike_list[1].first, 1.);
}

// Clear the monitors and verify reset
TEST_F(NeuralNetworkTest, MonitorReset) {
    NeuralNetwork net;
    auto neuron_type = std::make_shared<LIFNeuron>(tau_m, C_m, v_rest, v_reset, v_thresh, refractory);

    // Create 2 neurons
    net.add_neuron_population(2, neuron_type);
    // Connect neuron 0 -> neuron 1
    Synapse syn{0, 1, 1.5, 1.0};
    net.add_synapse(syn);

    // Set monitors
    auto spike_monitor = std::make_shared<SpikeMonitor>();
    SpikeMonitor* spike_monitor_ptr = spike_monitor.get();
    net.set_spike_monitor(spike_monitor);
    auto state_monitor = std::make_shared<StateMonitor>(1.0);
    StateMonitor* state_monitor_ptr = state_monitor.get();
    net.set_state_monitor(state_monitor);

    // Schedule initial spike for neuron 0
    net.schedule_spike_event(0.0, 0, 1.5);  // strong enough to spike

    // Run network
    net.run(5.0);

    // Expect data in monitors
    EXPECT_GT(spike_monitor_ptr->spike_list.size(), 1);
    EXPECT_GT(state_monitor_ptr->state_vector_list.size(), 1);

    // Reset monitors
    net.reset_monitors();

    EXPECT_EQ(spike_monitor_ptr->spike_list.size(), 0);
    EXPECT_EQ(state_monitor_ptr->state_vector_list.size(), 0);
}

// Run simulation in two parts and verify time continuity
TEST_F(NeuralNetworkTest, TimeContinuity) {
    NeuralNetwork net;
    auto neuron_type = std::make_shared<LIFNeuron>(tau_m, C_m, v_rest, v_reset, v_thresh, refractory);

    // Create 2 neurons
    net.add_neuron_population(2, neuron_type);
    // Connect neuron 0 -> neuron 1
    Synapse syn{0, 1, 1.5, 0.5};
    net.add_synapse(syn);

    // Set spike monitor
    auto monitor = std::make_shared<SpikeMonitor>();
    SpikeMonitor* monitor_ptr = monitor.get();
    net.set_spike_monitor(monitor);

    // Schedule initial spike for neuron 0
    net.schedule_spike_event(0.0, 0, 1.5);

    // Run network
    net.run(5.0);

    EXPECT_EQ(monitor_ptr->spike_list.size(), 2);
    EXPECT_EQ(monitor_ptr->spike_list[0].first, 0.);
    EXPECT_EQ(monitor_ptr->spike_list[1].first, 0.5);

    // Schedule second spike for neuron 0
    net.schedule_spike_event(0.0, 0, 1.5);

    // Run network
    net.run(5.0);

    EXPECT_EQ(monitor_ptr->spike_list.size(), 4);
    EXPECT_EQ(monitor_ptr->spike_list[2].first, 5.);
    EXPECT_EQ(monitor_ptr->spike_list[3].first, 5.5);
}


