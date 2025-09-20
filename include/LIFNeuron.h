#pragma once
#include "Neuron.h"

class LIFNeuron : public Neuron {
public:
    LIFNeuron(double tau_m = 0.02,
              double v_rest = 0.0,
              double v_reset = 0.0,
              double v_thresh = 1.0,
              double refractory = 0.002);

    bool update(double t, double* state, double* lastSpike) override;
    void receive(double value, double* state, double* lastSpike) override;
    double getInitValue() override;

private:
    double tau_m_;
    double v_rest_;
    double v_reset_;
    double v_thresh_;
    double refractory_;
};