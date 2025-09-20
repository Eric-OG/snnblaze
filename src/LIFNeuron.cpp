#include "LIFNeuron.h"
#include <algorithm> // for std::max
#include <cmath>

LIFNeuron::LIFNeuron(double tau_m, double v_rest, double v_reset, double v_thresh, double refractory)
    : tau_m_(tau_m),
      v_rest_(v_rest),
      v_reset_(v_reset),
      v_thresh_(v_thresh),
      refractory_(refractory) {}

bool LIFNeuron::update(double t, double* state, double* lastSpike) {
    // Leak - exponential decay
    double& v = *state;
    double& last_spike_time = *lastSpike;
    double dt = t-last_spike_time;

    if (dt < refractory_) return false; // still refractory

    v = v_rest_ + (v-v_rest_)*exp(-tau_m_*dt);

    if (v >= v_thresh_) {
        v = v_reset_;
        last_spike_time = t;
        return true;
    }
    return false;
}

void LIFNeuron::receive(double value, double* state, double* lastSpike) {
    *state += value; 
}

double LIFNeuron::getInitValue() {
    return v_reset_;
}