#include "LIFNeuron.h"
#include <gtest/gtest.h>
#include <cmath>

class LIFNeuronTest : public ::testing::Test {
protected:
    // Example neuron parameters
    double tau_m = 10.0;       // membrane time constant
    double v_rest = 0.0;       // resting potential
    double v_reset = 0.0;      // reset potential after spike
    double v_thresh = 1.0;     // threshold
    double refractory = 2.0;   // refractory period

    LIFNeuron neuron{tau_m, v_rest, v_reset, v_thresh, refractory};

    double state = 0.5;        // initial membrane potential
    double last_spike = 0;     // set at the start of the timeline
};

// Test that decay brings voltage closer to v_rest
TEST_F(LIFNeuronTest, DecayTest) {
    double t = 1.0;
    neuron.update(t, &state, &last_spike, 0.0);
    EXPECT_LT(state, 0.5); // state should decrease towards v_rest
}

// Test that receiving input increases voltage
TEST_F(LIFNeuronTest, ReceiveInputTest) {
    double t = 1.0;
    double input = 0.3;
    std::cout << t;
    neuron.update(t, &state, &last_spike, input);
    double expected = 0.5*exp(-t/tau_m) + v_rest + input;
    EXPECT_NEAR(state, expected, 1e-6);
}

// Test that neuron spikes when threshold is exceeded
TEST_F(LIFNeuronTest, SpikeTest) {
    double t = 1.0;
    state = 0.9; // close to threshold
    bool spiked = neuron.update(t, &state, &last_spike, 0.2);
    EXPECT_TRUE(spiked);
    EXPECT_EQ(state, v_reset);
    EXPECT_EQ(last_spike, t);
}

// Test refractory period prevents spiking
TEST_F(LIFNeuronTest, RefractoryTest) {
    last_spike = 1.0;
    state = 2.0; // above threshold
    double t = 2.0; // within refractory (refractory = 2.0)
    bool spiked = neuron.update(t, &state, &last_spike, 0.0);
    EXPECT_FALSE(spiked);
    EXPECT_EQ(last_spike, 1.0); // last spike time unchanged
}

// Test getInitValue
TEST_F(LIFNeuronTest, GetInitValueTest) {
    EXPECT_EQ(neuron.getInitValue(), v_reset);
}
