#include "LIFNeuron.h"
#include <cmath>
#include <iostream>
#include <omp.h>

LIFNeuron::LIFNeuron(double tau_m, double C_m, double v_rest, double v_reset, double v_thresh, double refractory)
    : tau_m_(tau_m),
      C_m_(C_m),
      inv_C_m_(1/C_m),
      v_rest_(v_rest),
      v_reset_(v_reset),
      v_thresh_(v_thresh),
      refractory_(refractory) {}

bool LIFNeuron::receive(double t, double charge, double* state, double* last_spike, double* last_update) {
    double last_spk = *last_spike;
    if (__builtin_expect((t - last_spk) < refractory_, 0))
        return false;

    double v = *state + charge * inv_C_m_;
    if (__builtin_expect(v >= v_thresh_, 0)) {
        *state = v_reset_;
        *last_spike = t;
        return true;
    }

    *state = v;
    return false;
}

void LIFNeuron::decay(double t, double* state, double* last_spike, double* last_update, size_t n=1) {
    const double v_rest = v_rest_;
    const double tau_m  = tau_m_;

    if (omp_get_max_threads() == 1){ // single-threaded
        #pragma omp simd
        for (size_t i = 0; i < n; ++i) {
            double dt = t - last_update[i];
            double refractory_mask = (t - last_spike[i]) >= refractory_;  // 1.0 or 0.0
            double decay_factor = std::exp(-dt / tau_m);
            double v_new = v_rest + (state[i] - v_rest) * decay_factor;

            // Apply only if not refractory
            state[i] = refractory_mask * v_new + (1.0 - refractory_mask) * v_reset_;
            last_update[i] = refractory_mask * t + (1.0 - refractory_mask) * last_update[i];
        }
    } else { // multi-threaded
        #pragma omp parallel for simd schedule(static)
        for (size_t i = 0; i < n; ++i) {
            double dt = t - last_update[i];
            double refractory_mask = (t - last_spike[i]) >= refractory_;  // 1.0 or 0.0
            double decay_factor = std::exp(-dt / tau_m);
            double v_new = v_rest + (state[i] - v_rest) * decay_factor;

            // Apply only if not refractory
            state[i] = refractory_mask * v_new + (1.0 - refractory_mask) * v_reset_;
            last_update[i] = refractory_mask * t + (1.0 - refractory_mask) * last_update[i];
        }
    }
}

double LIFNeuron::get_init_value() {
    return v_reset_;
}