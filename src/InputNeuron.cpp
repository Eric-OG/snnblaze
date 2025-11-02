#include "InputNeuron.h"
#include <cmath>
#include <iostream>
#include <omp.h>

InputNeuron::InputNeuron(){
    // do nothing
}

bool InputNeuron::receive(double t, double charge, double* state, double* last_spike, double* last_update) {
    // input neurons always propagate spikes to their synapses
    return true;
}

void InputNeuron::decay(double t, double* state, double* last_spike, double* last_update, size_t n=1) {
    // do nothing
}

double InputNeuron::get_init_value() {
    return 0;
}