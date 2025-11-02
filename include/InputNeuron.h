#pragma once
#include "Neuron.h"

class InputNeuron : public Neuron {
public:
    InputNeuron();

    void decay(double t, double* state, double* last_spike, double* last_update, size_t n) override;
    bool receive(double t, double charge, double* state, double* last_spike, double* last_update) override;
    double get_init_value() override;
};