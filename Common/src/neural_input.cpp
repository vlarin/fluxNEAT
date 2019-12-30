#include <flux/neural_input.h>

flux::NeuralInputId::NeuralInputId(std::string id) : id(std::move(id)) {
}

flux::NeuralInput::NeuralInput(flux::NeuralInputId input) : input(std::move(input)), value(0) {
}

void flux::NeuralInput::Reset() {
    value = 0;
}

void flux::NeuralInput::Apply(float_fl signal) {
    value += signal;
}
