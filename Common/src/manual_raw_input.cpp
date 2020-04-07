#include <flux/manual/manual_raw_input_unit.h>
#include <iostream>

using namespace flux;

ManualRawInputSensor::ManualRawInputSensor(
        std::string id,
        std::shared_ptr<IContext> context,
        std::set<NeuralNodeId> inputIds,
        bool verbose)
        : IRawSensorUnit(std::move(id), std::move(context)), _inputIds(std::move(inputIds)), _currentSequenceId(0),
        _verbose(verbose),
        _isAutoPlayed(true)
{}

std::vector<NeuralNode> ManualRawInputSensor::Fetch() const
{
    if (_inputSequences.empty())
    {
        setLastError("Manual Input sequences are not initialized!");
        return std::vector<NeuralNode>();
    }

    if (_isAutoPlayed)
    {
        _currentSequenceId++;
    }
    std::vector<NeuralNode> inputs = _inputSequences[_currentSequenceId % _inputSequences.size()];

    //TODO: Own logger?
    if (_verbose)
    {
        std::cout << "Manual black box input " << GetId() << ": ";
        for (const auto &input : inputs)
        {
            std::cout << "[" << input.GetNodeId().GetId() << ": " << input.GetValue() << "] ";
        }
        std::cout << std::endl;
    }

    return inputs;
}

void ManualRawInputSensor::SetInputs(const std::vector<NeuralNode> &inputs)
{
    //TODO: ids validation?
    _inputSequences = std::vector<std::vector<NeuralNode>>();
    _inputSequences.emplace_back(inputs);
    _currentSequenceId = 0;
}

void ManualRawInputSensor::SetInputsSequence(std::vector<std::vector<NeuralNode>> inputSequence)
{
    //TODO: ids validation?
    _inputSequences = std::move(inputSequence);
    _currentSequenceId = 0;
}

std::shared_ptr<IContextUnit> ManualRawInputSensor::Clone(std::shared_ptr<IContext> context) const
{
    return CloneToContext<ManualRawInputSensor>(context);
}

void ManualRawInputSensor::Step()
{
    _currentSequenceId++;
}
