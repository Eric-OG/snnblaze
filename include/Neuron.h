#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

// We take a mixed strategy, the neuron performs fixed operations over a fixed size array - we do this because we want SIMD operations
// Advantage of a contiguous representation - cache
// Key idea: we use a lazy update rule - only update state if a spike perturbs the neuron or if we take a measure (recorder)
// If many events happen at once, execute them in parallel (e.g. many decays on )
// We adopt a SoA (Structure of Arrays) paradigm because of this choice
class Neuron {
public:
    virtual ~Neuron() = default;

    // Advance the state with an input at time t
    virtual bool update(double t, double* state, double* last_spike, double* last_update, double input)=0;

    // Receive synaptic input (current)
    virtual void receive(double value, double* state, double* last_spike, double* last_update)=0;

    // Must return the value to which the neuron is initialized
    virtual double get_init_value()=0;
};


// Trampoline class for Python overrides
class PyNeuron : public Neuron {
public:
    using Neuron::Neuron;

    bool update(double t, double* state, double* last_spike, double* last_update, double input) override {
        PYBIND11_OVERRIDE_PURE(
            bool,
            Neuron,
            update,
            t, state, last_spike, last_update, input
        );
    }

    void receive(double value, double* state, double* last_spike, double* last_update) override {
        PYBIND11_OVERRIDE_PURE(
            void,
            Neuron,
            receive,
            value, state, last_spike, last_update
        );
    }

    double get_init_value() override {
        PYBIND11_OVERRIDE_PURE(
            double,
            Neuron,
            get_init_value
        );
    }
};