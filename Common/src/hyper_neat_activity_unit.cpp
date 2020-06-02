#include "flux/neat/hyper_neat_activity_unit.h"
#include "include/hyper_neat_activity_unit_impl.h"

std::set<flux::NeuralNodeId> flux::HyperNeatActivityUnit::GetInputIds() const
{
    return _inputIds;
}

std::set<flux::NeuralNodeId> flux::HyperNeatActivityUnit::GetOutputIds() const
{
    return _outputIds;
}

std::vector<flux::NeuralNode> flux::HyperNeatActivityUnit::Activate(const std::vector<NeuralNode> &inputs) const
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

flux::HyperNeatActivityUnit::HyperNeatActivityUnit(const std::string &id, const std::shared_ptr<IContext> &context,
                                         const std::set<NeuralNodeId> &inputIds,
                                         const std::set<NeuralNodeId> &outputIds)
                                         : IActivityUnit(id, context),
                                         _inputIds(inputIds),
                                         _outputIds(outputIds)

{
    _impl = std::make_unique<HyperNeatActivityUnitImpl>();
}

flux::HyperNeatActivityUnit::~HyperNeatActivityUnit() = default;

flux::HyperNeatActivityUnit::HyperNeatActivityUnit(const flux::HyperNeatActivityUnit &unit) :
        IActivityUnit(unit.GetId(), unit.GetContext()), _inputIds(unit._inputIds), _outputIds(unit._outputIds)
{
    _impl = std::make_unique<HyperNeatActivityUnitImpl>();
    _impl->CopySchemeFrom(*unit._impl);
}

flux::HyperNeatActivityUnit::HyperNeatActivityUnit(flux::HyperNeatActivityUnit &&unit) noexcept :
        IActivityUnit(unit.GetId(), unit.GetContext()),
        _inputIds(std::move(unit._inputIds)), _outputIds(std::move(unit._outputIds))
{
    unit._impl.swap(_impl);
}

void flux::HyperNeatActivityUnit::UpdateScheme(std::istream &istream)
{
    _impl->UpdateScheme(istream);
}

flux::HyperNeatActivityUnit &flux::HyperNeatActivityUnit::operator=(const flux::HyperNeatActivityUnit &other)
{
    if (&other == this)
    {
        return *this;
    }

    Reset(other.GetId(), other.GetContext());
    _inputIds = other._inputIds;
    _outputIds = other._outputIds;

    _impl = std::make_unique<HyperNeatActivityUnitImpl>();
    _impl->CopySchemeFrom(*other._impl);

    return *this;
}

std::shared_ptr<flux::IContextUnit> flux::HyperNeatActivityUnit::Clone(std::shared_ptr<IContext> context) const
{
    return CloneToContext<HyperNeatActivityUnit>(context);
}
