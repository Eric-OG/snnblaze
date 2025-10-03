#include "StateMonitor.h"

void StateMonitor::reset_recording() {
    this->state_vector_list.clear();
}

void StateMonitor::on_read(double time, std::vector<double> state_vector) {
    this->state_vector_list.push_back({time, state_vector});
}

double StateMonitor::get_reading_interval() {
    return this->reading_interval_;
}