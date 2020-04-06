#include <flux/cortex/cortex_black_box.h>

static bool UpdateChildSchemeImpl(const std::string& childId, std::istream &istream, flux::IContextUnit &input);

flux::CortexBlackBox::CortexBlackBox(const std::string &id, std::shared_ptr<IContext> context)
   : IBlackBox(id, std::move(context)) {}

void flux::CortexBlackBox::AddRawInput(std::shared_ptr<IRawSensorUnit> input)
{
    _rawInputs.emplace_back(input);
}

void flux::CortexBlackBox::AddAugmentedInput(std::shared_ptr<IAugmentedSensorUnit> input)
{
    _augmentedInputs.emplace_back(input);
}

void flux::CortexBlackBox::AddActivity(std::shared_ptr<IActivityUnit> activity)
{
    if (_activityUnits.find(activity->GetId()) != _activityUnits.end())
    {
        setLastErrorF("Activity unit %s has already been set!", activity->GetId().c_str());
        return;
    }

    _activityUnits.insert(std::make_pair(activity->GetId(), activity));
}

void flux::CortexBlackBox::AddFeedback(std::shared_ptr<IFeedbackUnit> feedback)
{
    _feedbacks.emplace_back(feedback);
}

void flux::CortexBlackBox::AddOutput(std::shared_ptr<IOutputUnit> output)
{
    _outputs.emplace_back(output);
}

void flux::CortexBlackBox::Step()
{
    //TODO: Input scheme validation?

    //save context input from last run
    std::map<NeuralInputId, NeuralInput> savedContext;
    for (auto &contextInput : _contextInputIds)
    {
        const auto input = _sensors.find(contextInput);
        if (input != _sensors.end())
        {
            savedContext.insert(std::make_pair(input->first, input->second));
        }
    }

    //reset last sensors data
    for (auto &input : _sensors)
    {
        input.second.Reset();
    }

    //reapply saved context
    for (const auto &context : _contextInputIds)
    {
        const auto savedInput = savedContext.find(context);
        if (savedInput != savedContext.end())
        {
            _sensors[context] = savedInput->second;
        }
        else
        {
            _sensors.insert(std::make_pair(context, NeuralInput(context)));
        }
    }

    //fetch raw sensors data
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

    //fetch augmented sensors data
    for (const auto &input : _augmentedInputs)
    {
        std::vector<NeuralInput> augmentedPreInputs;
        for (const auto &augmentedId : input->GetAugmentedInputIds())
        {
            if (_sensors.find(augmentedId) == _sensors.end())
            {
                setLastError(("Unable to fetch raw input id of - " + augmentedId.GetId()).c_str());
                return;
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

    FetchFeedback(_sensors);
    _cortexDecisionLayer.Step(_sensors, _mediators);

    float_fl maxMediator = 0.0;
    for (const auto &mediator : _mediators)
    {
        if (mediator.second.GetValue() > maxMediator)
        {
            maxMediator = mediator.second.GetValue();
        }
    }

    if (maxMediator > 0.98)
    {
        _cortexLayer.GrowOrMerge(_sensors, _mediators);
    }
    _cortexLayer.Step(_sensors, _mediators);

    bool isTransitionAvailable = false;
    for (const auto &excitedColumn : _cortexLayer.GetExcitedColumns())
    {
        if (_cortexDecisionLayer.TryMakeDecision(_sensors, excitedColumn.GetContext(), _mediators))
        {
            isTransitionAvailable = true;
            break;
        }
    }

    if (!isTransitionAvailable)
    {
        _cortexDecisionLayer.StartWandering(_sensors, _mediators);
    }

    auto activityUnitId = _cortexDecisionLayer.GetCurrentTransition().GetActivityId();
    auto contextTarget = _cortexDecisionLayer.GetCurrentTransition().GetDesiredContext();

    //apply desired context target
    for (auto &contextInput : _contextInputIds)
    {
        const auto input = contextTarget.find(contextInput);
        if (input != contextTarget.end())
        {
            _sensors[contextInput] = input->second;
        }
    }

    //fetch updated feedback after desired action
    FetchFeedback(_sensors);
    maxMediator = 0.0;
    for (const auto &mediator : _mediators)
    {
        if (mediator.second.GetValue() > maxMediator)
        {
            maxMediator = mediator.second.GetValue();
        }
    }

    if (maxMediator > 0.98)
    {
        _cortexLayer.GrowOrMerge(_sensors, _mediators);
    }
    _cortexLayer.Step(_sensors, _mediators);


    auto activityUnit = _activityUnits[activityUnitId];

    std::vector<NeuralInput> preInputs;
    for (const auto &inputId : activityUnit->GetInputIds())
    {
        if (_sensors.find(inputId) == _sensors.end())
        {
            setLastError(("Unable to fetch input id of - " + inputId.GetId()).c_str());
            return;
        }

        preInputs.emplace_back(_sensors[inputId]);
    }

    std::vector<NeuralOutput> outputs = activityUnit->Activate(preInputs);
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

void flux::CortexBlackBox::UpdateChildScheme(std::string childId, std::istream &istream)
{
    for (const auto &activity : _activityUnits)
    {
        if (UpdateChildSchemeImpl(childId, istream, *activity.second)) return;
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

    for (const auto &feedback : _feedbacks)
    {
        if (UpdateChildSchemeImpl(childId, istream, *feedback)) return;
    }
}

std::shared_ptr<flux::IContextUnit> flux::CortexBlackBox::Clone(std::shared_ptr<IContext> context) const
{
    std::shared_ptr<CortexBlackBox> clone = CloneToContext<CortexBlackBox>(context);
    for (int i = 0; i < clone->_rawInputs.size(); i++)
    {
        clone->_rawInputs[i] = std::static_pointer_cast<IRawSensorUnit>(_rawInputs[i]->Clone(context));
    }
    for (int i = 0; i < clone->_augmentedInputs.size(); i++)
    {
        clone->_augmentedInputs[i] = std::static_pointer_cast<IAugmentedSensorUnit>(_augmentedInputs[i]->Clone(context));
    }
    for (int i = 0; i < clone->_outputs.size(); i++)
    {
        clone->_outputs[i] = std::static_pointer_cast<IOutputUnit>(_outputs[i]->Clone(context));
    }
    for (int i = 0; i < clone->_feedbacks.size(); i++)
    {
        clone->_feedbacks[i] = std::static_pointer_cast<IFeedbackUnit>(_feedbacks[i]->Clone(context));
    }

    clone->_activityUnits.clear();
    for (const auto &activity : _activityUnits)
    {
        clone->_activityUnits.insert(std::make_pair(activity.first,
                std::static_pointer_cast<IActivityUnit>(activity.second->Clone(context))));
    }

    return clone;
}

const flux::NeuralInput &flux::CortexBlackBox::GetInputOf(std::string id) const
{
    auto input = _sensors.find(NeuralInputId(id));
    if (input == _sensors.end())
    {
        static const NeuralInput empty(NeuralInputId("__invalid_input__"));
        setLastError(("Unable to fetch input id of - " + id).c_str());
        return empty;
    }

    return input->second;
}

const flux::NeuralOutput &flux::CortexBlackBox::GetOutputOf(std::string id) const
{
    auto output = _responses.find(NeuralOutputId(id));
    if (output == _responses.end())
    {
        static const NeuralOutput empty(NeuralOutputId("__invalid_output__"));
        setLastError(("Unable to fetch output id of - " + id).c_str());
        return empty;
    }

    return output->second;
}

const flux::MediatorValue &flux::CortexBlackBox::GetMediatorOf(std::string id) const
{
    auto mediator = _mediators.find(MediatorId(id));
    if (mediator == _mediators.end())
    {
        static const MediatorValue empty(MediatorId("__invalid_mediator_id"));
        setLastErrorF("Unable to fetch mediator value of %s", id.c_str());
        return empty;
    }

    return mediator->second;
}

void flux::CortexBlackBox::InjectMediatorValue(flux::MediatorValue mediatorValue)
{
    auto mediator = _mediators.find(mediatorValue.GetMediator());
    if (mediator == _mediators.end())
    {
        _mediators.insert(std::make_pair(mediatorValue.GetMediator(), mediatorValue));
    }
    else
    {
        _mediators[mediatorValue.GetMediator()].Merge(mediatorValue);
    }
}

void flux::CortexBlackBox::FetchFeedback(const std::map<NeuralInputId, NeuralInput> &context)
{
    auto &mediatorsDelta = _mediators;
    for (const auto &feedbackUnit : _feedbacks)
    {
        std::vector<NeuralInput> preInputs;
        for (const auto &inputId : feedbackUnit->GetInputIds())
        {
            if (_sensors.find(inputId) == _sensors.end())
            {
                setLastError(("Unable to fetch input id of - " + inputId.GetId()).c_str());
                break;
            }

            preInputs.emplace_back(_sensors[inputId]);
        }
        auto deltas = feedbackUnit->Activate(preInputs);

        for (const auto &delta : deltas)
        {
            if (mediatorsDelta.find(delta.GetMediator()) != mediatorsDelta.end())
            {
                mediatorsDelta[delta.GetMediator()] = delta;
            }
            else
            {
                mediatorsDelta.insert(std::make_pair(delta.GetMediator(), delta));
            }
        }
    }

    //TODO: implement tolerance and hunger mechanics
}

void flux::CortexBlackBox::AddBuiltinTransition(const flux::CortexTransition &transition)
{
    _cortexDecisionLayer.AddBuiltinTransition(transition);
}

void flux::CortexBlackBox::AddContextInput(flux::NeuralInputId id)
{
    _contextInputIds.push_back(id);
}

void flux::CortexBlackBox::SetActivityBound(std::string activityId)
{
    _cortexDecisionLayer.SetActivityBound(activityId);
}

void flux::CortexBlackBox::ResetActivityBound()
{
    _cortexDecisionLayer.ResetActivityBound();
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