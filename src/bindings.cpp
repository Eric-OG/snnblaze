#include <pybind11/pybind11.h>
#include <pybind11/stl.h>       // For std::vector
#include <pybind11/numpy.h>     // For NumPy arrays if needed
#include "Neuron.h"
#include "LIFNeuron.h"
#include "SpikeMonitor.h"
#include "Synapse.h"
#include "NeuralNetwork.h"

namespace py = pybind11;

PYBIND11_MODULE(pysnnblaze, m) {
    py::class_<Neuron, PyNeuron, std::shared_ptr<Neuron>>(m, "Neuron")
        .def("decay", [](Neuron &self, double t, py::array_t<double> state, py::array_t<double> lastSpike, py::array_t<double> lastUpdate, size_t n) {
            // Ensure arrays are contiguous and size=1
            auto state_ptr = static_cast<double*>(state.request().ptr);
            auto lastSpike_ptr = static_cast<double*>(lastSpike.request().ptr);
            auto lastUpdate_ptr = static_cast<double*>(lastUpdate.request().ptr);
            return self.decay(t, state_ptr, lastSpike_ptr, lastUpdate_ptr, n);
        })
        .def("receive", [](Neuron &self, double t, double charge, py::array_t<double> state, py::array_t<double> lastSpike, py::array_t<double> lastUpdate) {
            auto state_ptr = static_cast<double*>(state.request().ptr);
            auto lastSpike_ptr = static_cast<double*>(lastSpike.request().ptr);
            auto lastUpdate_ptr = static_cast<double*>(lastUpdate.request().ptr);
            self.receive(t, charge, state_ptr, lastSpike_ptr, lastUpdate_ptr);
        })
        .def("get_init_value", &Neuron::get_init_value);

    py::class_<LIFNeuron, Neuron, std::shared_ptr<LIFNeuron>>(m, "LIFNeuron")
        .def(py::init<double, double, double, double, double, double>(), 
             py::arg("tau_m"), py::arg("C_m"), py::arg("v_rest"), py::arg("v_reset"), py::arg("v_thresh"), py::arg("refractory"))
        .def_readwrite("C_m", &LIFNeuron::tau_m_)
        .def_readwrite("tau_m", &LIFNeuron::tau_m_)
        .def_readwrite("v_rest", &LIFNeuron::v_rest_)
        .def_readwrite("v_reset", &LIFNeuron::v_reset_)
        .def_readwrite("v_thresh", &LIFNeuron::v_thresh_)
        .def_readwrite("refractory", &LIFNeuron::refractory_)
        .def("decay", [](LIFNeuron &self, double t, py::array_t<double> state, py::array_t<double> lastSpike, py::array_t<double> lastUpdate, size_t n) {
            auto state_ptr = static_cast<double*>(state.request().ptr);
            auto lastSpike_ptr = static_cast<double*>(lastSpike.request().ptr);
            auto lastUpdate_ptr = static_cast<double*>(lastUpdate.request().ptr);
            return self.decay(t, state_ptr, lastSpike_ptr, lastUpdate_ptr, n);
        })
        .def("receive", [](LIFNeuron &self, double t, double charge, py::array_t<double> state, py::array_t<double> lastSpike, py::array_t<double> lastUpdate) {
            auto state_ptr = static_cast<double*>(state.request().ptr);
            auto lastSpike_ptr = static_cast<double*>(lastSpike.request().ptr);
            auto lastUpdate_ptr = static_cast<double*>(lastUpdate.request().ptr);
            self.receive(t, charge, state_ptr, lastSpike_ptr, lastUpdate_ptr);
        })
        .def("get_init_value", &LIFNeuron::get_init_value);
    
    py::class_<SpikeMonitor, std::shared_ptr<SpikeMonitor>>(m, "SpikeMonitor")
        .def(py::init<>())  // default constructor
        .def("on_spike", &SpikeMonitor::on_spike,
            py::arg("neuron_id"), py::arg("time"))
        .def("reset_spikes", &SpikeMonitor::reset_spikes)
        .def_readwrite("spike_list", &SpikeMonitor::spike_list,
                    "List of (time, neuron_id) pairs");

    py::class_<StateMonitor, std::shared_ptr<StateMonitor>>(m, "StateMonitor")
        .def(py::init<double>(), py::arg("reading_interval"))
        .def("on_read", &StateMonitor::on_read, py::arg("time"), py::arg("state_vector"))
        .def("reset_recording", &StateMonitor::reset_recording)
        .def("get_reading_interval", &StateMonitor::get_reading_interval)
        .def_readwrite("state_vector_list", &StateMonitor::state_vector_list);

    py::class_<Synapse>(m, "Synapse")
        .def(py::init<size_t, size_t, double, double>(),
             py::arg("srcId"), py::arg("dstId"), py::arg("weight"), py::arg("delay"))
        .def_readwrite("srcId", &Synapse::srcId)
        .def_readwrite("dstId", &Synapse::dstId)
        .def_readwrite("weight", &Synapse::weight)
        .def_readwrite("delay", &Synapse::delay);

    // Bind NeuralNetwork
    py::class_<NeuralNetwork>(m, "NeuralNetwork")
        .def(py::init<>())
        .def("add_neuron_population", &NeuralNetwork::add_neuron_population,
             py::arg("size"), py::arg("neuron_type"))
        .def("add_synapse", &NeuralNetwork::add_synapse,
             py::arg("synapse"))
        .def("schedule_spike_event", &NeuralNetwork::schedule_spike_event,
             py::arg("time"), py::arg("neuronIndex"), py::arg("weight"))
        .def("set_spike_monitor", &NeuralNetwork::set_spike_monitor, py::arg("monitor"))
        .def("set_state_monitor", &NeuralNetwork::set_state_monitor, py::arg("monitor"))
        .def("run", &NeuralNetwork::run, py::arg("T"))
        .def("size", &NeuralNetwork::size);
}
