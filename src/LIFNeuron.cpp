#include "LIFNeuron.h"
#include <cmath>
#include <iostream>

LIFNeuron::LIFNeuron(double tau_m, double C_m, double v_rest, double v_reset, double v_thresh, double refractory)
    : tau_m_(tau_m),
      C_m_(C_m),
      inv_C_m_(1/C_m),
      v_rest_(v_rest),
      v_reset_(v_reset),
      v_thresh_(v_thresh),
      refractory_(refractory) {}

bool LIFNeuron::update(double t, double* state, double* last_spike, double* last_update, double input=0.0) {
    // Leak - exponential decay
    double& v = *state;
    double& last_spike_time = *last_spike;
    double& last_update_time = *last_update;

    if ((t-last_spike_time) < refractory_) return false; // still refractory - stay at v_reset_

    // Apply exponential decay (first)
    v = v_rest_ + (v-v_rest_)*std::exp(-(t-last_update_time)/tau_m_);
    // Apply spike inputs (second)
    receive(input, state, last_spike, last_update);
    last_update_time = t;

    if (v >= v_thresh_) {
        v = v_reset_;
        last_spike_time = t;
        return true;
    }
    return false;
}

void LIFNeuron::receive(double value, double* state, double* last_spike, double* last_update) {
    *state += value/C_m_; // value is an instant current (charge) [C], inv_C_m_ is in [1/F], so state is in [V] 
}

double LIFNeuron::get_init_value() {
    return v_reset_;
}