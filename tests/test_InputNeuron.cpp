#include "InputNeuron.h"
#include <gtest/gtest.h>
#include <cmath>
#include <limits>

class InputNeuronTest : public ::testing::Test {
protected:
    InputNeuron neuron;

    double state = 0.0;
    double last_spike = -std::numeric_limits<double>::infinity();
    double last_update = 0;
};

// Test empty decay does not change state
TEST_F(InputNeuronTest, DecayTest) {
    double t = 1.0;
    neuron.decay(t, &state, &last_spike, &last_update, 1);
    EXPECT_EQ(state, 0.0); // state should never change
}

// Test that receiving input does not change voltage
TEST_F(InputNeuronTest, ReceiveInputTest) {
    double t = 1.0;
    double input = 0.3;
    std::cout << t;
    neuron.decay(t, &state, &last_spike, &last_update, 1);
    neuron.receive(t, input, &state, &last_spike, &last_update);
    EXPECT_EQ(state, 0.0);
}

// Test that neuron spikes when any input is received
TEST_F(InputNeuronTest, SpikeTest) {
    double t = 1.0;
    neuron.decay(t, &state, &last_spike, &last_update, 1);
    bool spiked = neuron.receive(t, 0.001, &state, &last_spike, &last_update);
    EXPECT_TRUE(spiked);
}

// Test getInitValue
TEST_F(InputNeuronTest, GetInitValueTest) {
    EXPECT_EQ(neuron.get_init_value(), 0.0);
}
