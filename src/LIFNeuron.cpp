#include "LIFNeuron.h"
#include <cmath>

LIFNeuron::LIFNeuron(double tau_m, double v_rest, double v_reset, double v_thresh, double refractory)
    : tau_m_(tau_m),
      v_rest_(v_rest),
      v_reset_(v_reset),
      v_thresh_(v_thresh),
      refractory_(refractory) {}

bool LIFNeuron::update(double t, double* state, double* last_spike, double input=0.0) {
    // Leak - exponential decay
    double& v = *state;
    double& last_spike_time = *last_spike;
    double dt = t-last_spike_time;

    if ((dt < refractory_) && (last_spike_time!=0)) return false; // still refractory

    // Apply exponential decay
    v = v_rest_ + (v-v_rest_)*exp(-tau_m_*dt);
    // Apply spike inputs
    receive(input, state, last_spike);

    if (v >= v_thresh_) {
        v = v_reset_;
        last_spike_time = t;
        return true;
    }
    return false;
}

void LIFNeuron::receive(double value, double* state, double* last_spike) {
    *state += value; 
}

double LIFNeuron::getInitValue() {
    return v_reset_;
}