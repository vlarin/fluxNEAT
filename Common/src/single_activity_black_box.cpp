#include <flux/manual/single_activity_black_box.h>

bool UpdateChildScheme(const std::string& childId, std::istream &istream, flux::IContextUnit &input);

flux::SingleActivityBlackBox::SingleActivityBlackBox(
        const std::string &id,
        std::shared_ptr<IContext> context)
        : IBlackBox(id, context) {}

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
        throw std::exception("Activity unit have been already set!");
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
    std::map<NeuralInputId, NeuralInput> inputs;
    for (const auto &input : _rawInputs)
    {
        std::vector<NeuralInput> raw = input->Fetch();
        for (const auto &rawInput : raw)
        {
            if (inputs.find(rawInput.GetInputId()) == inputs.end())
            {
                inputs.insert(std::make_pair(rawInput.GetInputId(), rawInput));
            }
            else
            {
                inputs[rawInput.GetInputId()].Apply(rawInput.GetValue());
            }
        }
    }

    for (const auto &input : _augmentedInputs)
    {
        std::vector<NeuralInput> augmentedPreInputs;
        for (const auto &augmentedId : input->GetAugmentedInputIds())
        {
            if (inputs.find(augmentedId) == inputs.end())
            {
                throw std::exception(("Unable to fetch raw input id of - " + augmentedId.GetId()).c_str());
            }

            augmentedPreInputs.emplace_back(inputs[augmentedId]);
        }

        std::vector<NeuralInput> raw = input->ApplyAugmentation(augmentedPreInputs);
        for (const auto &rawInput : raw)
        {
            if (inputs.find(rawInput.GetInputId()) == inputs.end())
            {
                inputs.insert(std::make_pair(rawInput.GetInputId(), rawInput));
            }
            else
            {
                inputs[rawInput.GetInputId()].Apply(rawInput.GetValue());
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
        if (inputs.find(inputId) == inputs.end())
        {
            throw std::exception(("Unable to fetch input id of - " + inputId.GetId()).c_str());
        }

        preInputs.emplace_back(inputs[inputId]);
    }


    std::vector<NeuralOutput> outputs = _activityUnit->Activate(preInputs);

    for (const auto &output : _outputs)
    {
        output->Apply(outputs);
    }
}

void flux::SingleActivityBlackBox::UpdateScheme(std::string childId, std::istream &istream)
{
    if (_activityUnit && UpdateChildScheme(childId, istream, *_activityUnit))
    {
        return;
    }

    for (const auto &input : _rawInputs)
    {
        if (UpdateChildScheme(childId, istream, *input)) return;
    }

    for (const auto &input : _augmentedInputs)
    {
        if (UpdateChildScheme(childId, istream, *input)) return;
    }

    for (const auto &output : _outputs)
    {
        if (UpdateChildScheme(childId, istream, *output)) return;
    }
}

static bool UpdateChildScheme(const std::string& childId, std::istream &istream, flux::IContextUnit &input)
{
    if (input.GetId() == childId)
    {
        input.UpdateScheme(istream);
        return true;
    }

    return false;
}
