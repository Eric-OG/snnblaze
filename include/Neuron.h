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
    virtual bool update(double t, double* state, double* lastSpike, double input)=0;

    // Receive synaptic input (current)
    virtual void receive(double value, double* state, double* lastSpike)=0;

    // Must return the value to which the neuron is initialized
    virtual double getInitValue()=0;
};


// Trampoline class for Python overrides
class PyNeuron : public Neuron {
public:
    using Neuron::Neuron;

    bool update(double t, double* state, double* lastSpike, double input) override {
        PYBIND11_OVERRIDE_PURE(
            bool,
            Neuron,
            update,
            t, state, lastSpike, input
        );
    }

    void receive(double value, double* state, double* lastSpike) override {
        PYBIND11_OVERRIDE_PURE(
            void,
            Neuron,
            receive,
            value, state, lastSpike
        );
    }

    double getInitValue() override {
        PYBIND11_OVERRIDE_PURE(
            double,
            Neuron,
            getInitValue
        );
    }
};