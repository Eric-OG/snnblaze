#pragma once
#include <string>
#include "SpikeEvent.h"

class SpikeMonitor {
public:
    void setOutputFile(const std::string& filename);
    void onSpike(int neuron_id, double time);
    void closeFile();

private:
    std::string output_file;
};