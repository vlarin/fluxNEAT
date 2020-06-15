#include <flux/manual/identity_activity_unit.h>

#include <utility>

flux::IdentityActivityUnit::IdentityActivityUnit(
        flux::NeuralNodeId target,
        flux::NeuralNodeId outputId,
        std::string id,
        std::shared_ptr<IContext> context) : flux::IActivityUnit(std::move(id), std::move(context)),
        _target(std::move(target)), _outputId(std::move(outputId)) {}

std::vector<flux::NeuralNodeId> flux::IdentityActivityUnit::GetInputIds() const
{
    return std::vector<NeuralNodeId> { _target };
}

std::vector<flux::NeuralNodeId> flux::IdentityActivityUnit::GetOutputIds() const
{
    return std::vector<NeuralNodeId> { _outputId };
}

std::vector<flux::NeuralNode> flux::IdentityActivityUnit::Activate(const std::vector<NeuralNode> &inputs) const
{
    for (const auto &input : inputs)
    {
        if (input.GetNodeId().GetId() == _target.GetId())
        {
            return std::vector<NeuralNode> { NeuralNode(_outputId, input.GetValue() )};
        }
    }
    return std::vector<NeuralNode> { NeuralNode(_outputId, 0.0 )};
}

std::shared_ptr<flux::IContextUnit> flux::IdentityActivityUnit::Clone(std::shared_ptr<IContext> context) const
{
    return CloneToContext<IdentityActivityUnit>(context);
}
