#include <flux/manual/manual_raw_input_unit.h>

using namespace flux;

ManualRawInputSensor::ManualRawInputSensor(
        std::string id,
        std::shared_ptr<IContext> context,
        std::set<NeuralInputId> inputIds)
        : IRawSensorUnit(std::move(id), std::move(context)), _inputIds(std::move(inputIds)), _currentSequenceId(0)
{}

std::vector<NeuralInput> ManualRawInputSensor::Fetch() const
{
    if (_inputSequences.empty())
    {
        throw std::exception("Manual Input sequences are not initialized!");
    }

    return _inputSequences[_currentSequenceId++ % _inputSequences.size()];
}

void ManualRawInputSensor::SetInputs(const std::vector<NeuralInput> &inputs)
{
    //TODO: ids validation?
    _inputSequences = std::vector<std::vector<NeuralInput>>();
    _inputSequences.emplace_back(inputs);
    _currentSequenceId = 0;
}

void ManualRawInputSensor::SetInputsSequence(std::vector<std::vector<NeuralInput>> inputSequence)
{
    //TODO: ids validation?
    _inputSequences = std::move(inputSequence);
    _currentSequenceId = 0;
}
