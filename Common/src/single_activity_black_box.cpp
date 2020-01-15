#include <flux/manual/single_activity_black_box.h>

bool UpdateChildSchemeImpl(const std::string& childId, std::istream &istream, flux::IContextUnit &input);

flux::SingleActivityBlackBox::SingleActivityBlackBox(
        const std::string &id,
        std::shared_ptr<IContext> context)
        : IBlackBox(id, std::move(context)) {}

void flux::SingleActivityBlackBox::AddRawInput(std::shared_ptr<IRawSensorUnit> input)
{
    _rawInputs.emplace_back(input);
}

void flux::SingleActivityBlackBox::AddAugmentedInput(std::shared_ptr<IAugmentedSensorUnit> input)
{
    _augmentedInputs.emplace_back(input);
}

void flux::SingleActivityBlackBox::AddActivity(std::shared_ptr<IActivityUnit> activity)
{
    if (_activityUnit)
    {
        throw std::exception("Activity unit has already been set!");
    }

    _activityUnit = activity;
}

void flux::SingleActivityBlackBox::AddOutput(std::shared_ptr<IOutputUnit> output)
{
    _outputs.emplace_back(output);
}

void flux::SingleActivityBlackBox::Step()
{
    //TODO: validation
    for (auto &input : _sensors)
    {
        input.second.Reset();
    }

    for (const auto &input : _rawInputs)
    {
        std::vector<NeuralInput> raw = input->Fetch();
        for (const auto &rawInput : raw)
        {
            if (_sensors.find(rawInput.GetInputId()) == _sensors.end())
            {
                _sensors.insert(std::make_pair(rawInput.GetInputId(), rawInput));
            }
            else
            {
                _sensors[rawInput.GetInputId()].Apply(rawInput.GetValue());
            }
        }
    }

    for (const auto &input : _augmentedInputs)
    {
        std::vector<NeuralInput> augmentedPreInputs;
        for (const auto &augmentedId : input->GetAugmentedInputIds())
        {
            if (_sensors.find(augmentedId) == _sensors.end())
            {
                throw std::exception(("Unable to fetch raw input id of - " + augmentedId.GetId()).c_str());
            }

            augmentedPreInputs.emplace_back(_sensors[augmentedId]);
        }

        std::vector<NeuralInput> raw = input->ApplyAugmentation(augmentedPreInputs);
        for (const auto &rawInput : raw)
        {
            if (_sensors.find(rawInput.GetInputId()) == _sensors.end())
            {
                _sensors.insert(std::make_pair(rawInput.GetInputId(), rawInput));
            }
            else
            {
                _sensors[rawInput.GetInputId()].Apply(rawInput.GetValue());
            }
        }
    }

    if (!_activityUnit)
    {
        throw std::exception("Activity unit has to be set!");
    }

    std::vector<NeuralInput> preInputs;
    for (const auto &inputId : _activityUnit->GetInputIds())
    {
        if (_sensors.find(inputId) == _sensors.end())
        {
            throw std::exception(("Unable to fetch input id of - " + inputId.GetId()).c_str());
        }

        preInputs.emplace_back(_sensors[inputId]);
    }

    std::vector<NeuralOutput> outputs = _activityUnit->Activate(preInputs);
    for (const auto &output : _outputs)
    {
        output->Apply(outputs);
    }

    for (auto &output : _responses)
    {
        output.second.Reset();
    }
    for (const auto &outputValue : outputs)
    {
        if (_responses.find(outputValue.GetOutputId()) == _responses.end())
        {
            _responses.insert(std::make_pair(outputValue.GetOutputId(), outputValue));
        }
		else
		{
			_responses[outputValue.GetOutputId()].Apply(outputValue.GetValue());			
		}
    }
}

void flux::SingleActivityBlackBox::UpdateChildScheme(std::string childId, std::istream &istream)
{
    if (_activityUnit && UpdateChildSchemeImpl(childId, istream, *_activityUnit))
    {
        return;
    }

    for (const auto &input : _rawInputs)
    {
        if (UpdateChildSchemeImpl(childId, istream, *input)) return;
    }

    for (const auto &input : _augmentedInputs)
    {
        if (UpdateChildSchemeImpl(childId, istream, *input)) return;
    }

    for (const auto &output : _outputs)
    {
        if (UpdateChildSchemeImpl(childId, istream, *output)) return;
    }
}

std::shared_ptr<flux::IContextUnit> flux::SingleActivityBlackBox::Clone(std::shared_ptr<IContext> context) const
{
    std::shared_ptr<SingleActivityBlackBox> clone = CloneToContext<SingleActivityBlackBox>(context);
    for (int i = 0; i < clone->_rawInputs.size(); i++)
    {
        clone->_rawInputs[i] = std::reinterpret_pointer_cast<IRawSensorUnit>(_rawInputs[i]->Clone(context));
    }
    for (int i = 0; i < clone->_augmentedInputs.size(); i++)
    {
        clone->_augmentedInputs[i] = std::reinterpret_pointer_cast<IAugmentedSensorUnit>(_augmentedInputs[i]->Clone(context));
    }
    for (int i = 0; i < clone->_outputs.size(); i++)
    {
        clone->_outputs[i] = std::reinterpret_pointer_cast<IOutputUnit>(_outputs[i]->Clone(context));
    }

    if (_activityUnit)
    {
        clone->_activityUnit = std::reinterpret_pointer_cast<IActivityUnit>(_activityUnit->Clone(context));
    }

    return clone;
}

const flux::NeuralInput &flux::SingleActivityBlackBox::GetInputOf(std::string id) const
{
    auto input = _sensors.find(NeuralInputId(id));
    if (input == _sensors.end())
    {
        throw std::exception(("Unable to fetch input id of - " + id).c_str());
    }

    return input->second;
}

const flux::NeuralOutput &flux::SingleActivityBlackBox::GetOutputOf(std::string id) const
{
    auto output = _responses.find(NeuralOutputId(id));
    if (output == _responses.end())
    {
        throw std::exception(("Unable to fetch output id of - " + id).c_str());
    }

    return output->second;
}

static bool UpdateChildSchemeImpl(const std::string& childId, std::istream &istream, flux::IContextUnit &input)
{
    if (input.GetId() == childId)
    {
        input.UpdateScheme(istream);
        return true;
    }

    return false;
}
