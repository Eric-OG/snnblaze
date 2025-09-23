#include <pybind11/pybind11.h>
#include <pybind11/stl.h>       // For std::vector
#include <pybind11/numpy.h>     // For NumPy arrays if needed
#include "Neuron.h"
#include "LIFNeuron.h"

namespace py = pybind11;

PYBIND11_MODULE(pysnnblaze, m) {
    py::class_<Neuron, PyNeuron>(m, "Neuron")
        .def("update", [](Neuron &self, double t, py::array_t<double> state, py::array_t<double> lastSpike, double input) {
            // Ensure arrays are contiguous and size=1
            auto state_ptr = static_cast<double*>(state.request().ptr);
            auto lastSpike_ptr = static_cast<double*>(lastSpike.request().ptr);
            return self.update(t, state_ptr, lastSpike_ptr, input);
        })
        .def("receive", [](Neuron &self, double value, py::array_t<double> state, py::array_t<double> lastSpike) {
            auto state_ptr = static_cast<double*>(state.request().ptr);
            auto lastSpike_ptr = static_cast<double*>(lastSpike.request().ptr);
            self.receive(value, state_ptr, lastSpike_ptr);
        })
        .def("getInitValue", &Neuron::getInitValue);

    py::class_<LIFNeuron, Neuron>(m, "LIFNeuron")
        .def(py::init<>())
        .def("update", [](LIFNeuron &self, double t, py::array_t<double> state, py::array_t<double> lastSpike, double input) {
            auto state_ptr = static_cast<double*>(state.request().ptr);
            auto lastSpike_ptr = static_cast<double*>(lastSpike.request().ptr);
            return self.update(t, state_ptr, lastSpike_ptr, input);
        })
        .def("receive", [](LIFNeuron &self, double value, py::array_t<double> state, py::array_t<double> lastSpike) {
            auto state_ptr = static_cast<double*>(state.request().ptr);
            auto lastSpike_ptr = static_cast<double*>(lastSpike.request().ptr);
            self.receive(value, state_ptr, lastSpike_ptr);
        })
        .def("getInitValue", &LIFNeuron::getInitValue);
}
