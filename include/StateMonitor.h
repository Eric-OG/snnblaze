#pragma once
#include <vector>
#include <memory>

class StateMonitor {
public:
    StateMonitor(double reading_interval) : reading_interval_(reading_interval) {}
    void on_read(double time, std::vector<double> state_vector);
    void reset_recording();
    double get_reading_interval();

    // Public for direct access from python
    // Pair: (time, state vector)
    std::vector<std::pair<double, std::vector<double>>> state_vector_list;

    double reading_interval_;
};