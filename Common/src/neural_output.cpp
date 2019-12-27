#include "../include/flux/neural_output.h"

flux::NeuralOutputId::NeuralOutputId(std::string id) : id(std::move(id)) {
}

flux::NeuralOutput::NeuralOutput(flux::NeuralOutputId output) : output(std::move(output)), value(0) {
}

void flux::NeuralOutput::Reset() {
    value = 0;
}

void flux::NeuralOutput::Apply(float_fl signal) {
    value += signal;
}