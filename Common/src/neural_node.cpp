#include <flux/neural_node.h>

flux::NeuralNodeId::NeuralNodeId(std::string id) : id(std::move(id))
{
}

flux::NeuralNode::NeuralNode(flux::NeuralNodeId input) : input(std::move(input)), value(0)
{
}

flux::NeuralNode::NeuralNode(flux::NeuralNodeId input, float_fl value) : input(std::move(input)), value(value)
{
}

void flux::NeuralNode::Reset() {
    value = 0;
}

void flux::NeuralNode::Apply(float_fl signal) {
    value += signal;
}

