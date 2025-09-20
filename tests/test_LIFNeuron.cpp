#include "LIFNeuron.h"
#include <gtest/gtest.h>

// Fixture for LIFNeuron tests
class LIFNeuronTest : public ::testing::Test {
protected:
    void SetUp() override {
        tau = 10.0;
        v_reset = 0.0;
        v_thresh = 1.0;
        refractory = 2.0;

        neuron = new LIFNeuron(tau, v_reset, v_thresh, refractory);
        state = neuron->getInitValue();
        lastSpike = -refractory;  // ensure neuron is not initially refractory
    }

    void TearDown() override {
        delete neuron;
    }

    double tau;
    double v_reset;
    double v_thresh;
    double refractory;

    LIFNeuron* neuron;
    double state;
    double lastSpike;
};

// Test initialization
TEST_F(LIFNeuronTest, InitValue) {
    EXPECT_EQ(state, v_reset);
}

// Test receiving input
TEST_F(LIFNeuronTest, ReceiveInput) {
    neuron->receive(0.5, &state, &lastSpike);
    EXPECT_DOUBLE_EQ(state, 0.5);

    neuron->receive(0.3, &state, &lastSpike);
    EXPECT_DOUBLE_EQ(state, 0.8);
}

// Test update without spike
TEST_F(LIFNeuronTest, UpdateNoSpike) {
    state = 0.5;
    bool spiked = neuron->update(1.0, &state, &lastSpike);
    EXPECT_FALSE(spiked);
    EXPECT_LT(state, 0.5);  // state decays due to leak
}

// Test spike generation
TEST_F(LIFNeuronTest, Spike) {
    state = 1.0;  // set above threshold
    bool spiked = neuron->update(1.0, &state, &lastSpike);
    EXPECT_TRUE(spiked);
    EXPECT_EQ(state, v_reset);
    EXPECT_EQ(lastSpike, 1.0);
}

// Test refractory period
TEST_F(LIFNeuronTest, Refractory) {
    state = 1.0;
    lastSpike = 0.0;
    // within refractory period
    bool spiked = neuron->update(1.0, &state, &lastSpike);
    EXPECT_FALSE(spiked);
    EXPECT_EQ(lastSpike, 0.0);  // lastSpike should not change

    // after refractory period
    spiked = neuron->update(3.0, &state, &lastSpike);
    // Depending on state dynamics, may or may not spike
    EXPECT_EQ(lastSpike == 3.0, spiked);
}

// Test getInitValue
TEST_F(LIFNeuronTest, InitValueFunction) {
    EXPECT_EQ(neuron->getInitValue(), v_reset);
}
