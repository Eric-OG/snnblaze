#pragma once
#include "Neuron.h"

class LIFNeuron : public Neuron {
public:
    LIFNeuron(double tau_m = 0.02, // [s]
              double C_m = 1e-6, // [F], from this value R = tau_m / C_m
              double v_rest = 0.07, // [V]
              double v_reset = 0.07, // [V]
              double v_thresh = 0.05, // [V]
              double refractory = 0.002 // [s]
    );

    bool update(double t, double* state, double* last_spike, double* last_update, double input) override;
    void receive(double value, double* state, double* last_spike, double* last_update) override;
    double get_init_value() override;

    // Public variable to make acess easier from Python
    double tau_m_;
    double C_m_;
    double inv_C_m_;
    double v_rest_;
    double v_reset_;
    double v_thresh_;
    double refractory_;
};