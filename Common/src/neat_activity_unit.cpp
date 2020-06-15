#include "flux/neat/neat_activity_unit.h"
#include "include/neat_activity_unit_impl.h"

std::vector<flux::NeuralNodeId> flux::NeatActivityUnit::GetInputIds() const
{
    return _inputIds;
}

std::vector<flux::NeuralNodeId> flux::NeatActivityUnit::GetOutputIds() const
{
    return _outputIds;
}

std::vector<flux::NeuralNode> flux::NeatActivityUnit::Activate(const std::vector<NeuralNode> &inputs) const
{
    arma::vec inputVector(inputs.size());
    for (int i = 0; i < inputs.size(); i++)
    {
        inputVector[i] = inputs[i].GetValue();
    }

    arma::vec outputVector = _impl->Activate(inputVector);
    std::vector<NeuralNode> outputs;

    int index = 0;
    for (const auto &outputId : _outputIds)
    {
        outputs.emplace_back(NeuralNode(outputId, outputVector[index]));
        index++;
    }

    return outputs;
}

flux::NeatActivityUnit::NeatActivityUnit(const std::string &id, const std::shared_ptr<IContext> &context,
                                         const std::vector<NeuralNodeId> &inputIds,
                                         const std::vector<NeuralNodeId> &outputIds)
                                         : IActivityUnit(id, context),
                                         _inputIds(inputIds),
                                         _outputIds(outputIds)
{
    _impl = std::make_unique<NeatActivityUnitImpl>();
}

flux::NeatActivityUnit::~NeatActivityUnit() = default;

flux::NeatActivityUnit::NeatActivityUnit(const flux::NeatActivityUnit &unit) :
        IActivityUnit(unit.GetId(), unit.GetContext()), _inputIds(unit._inputIds), _outputIds(unit._outputIds)
{
    _impl = std::make_unique<NeatActivityUnitImpl>();
    _impl->CopySchemeFrom(*unit._impl);
}

flux::NeatActivityUnit::NeatActivityUnit(flux::NeatActivityUnit &&unit) noexcept :
        IActivityUnit(unit.GetId(), unit.GetContext()),
        _inputIds(std::move(unit._inputIds)), _outputIds(std::move(unit._outputIds))
{
    unit._impl.swap(_impl);
}

void flux::NeatActivityUnit::UpdateScheme(std::istream &istream)
{
    _impl->UpdateScheme(istream);
}

flux::NeatActivityUnit &flux::NeatActivityUnit::operator=(const flux::NeatActivityUnit &other)
{
    if (&other == this)
    {
        return *this;
    }

    Reset(other.GetId(), other.GetContext());
    _inputIds = other._inputIds;
    _outputIds = other._outputIds;

    _impl = std::make_unique<NeatActivityUnitImpl>();
    _impl->CopySchemeFrom(*other._impl);

    return *this;
}

std::shared_ptr<flux::IContextUnit> flux::NeatActivityUnit::Clone(std::shared_ptr<IContext> context) const
{
    return CloneToContext<NeatActivityUnit>(context);
}
