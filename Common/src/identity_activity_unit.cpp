#include <flux/manual/identity_activity_unit.h>

#include <utility>

flux::IdentityActivityUnit::IdentityActivityUnit(
        flux::NeuralInputId target,
        flux::NeuralOutputId outputId,
        std::string id,
        std::shared_ptr<IContext> context) : flux::IActivityUnit(std::move(id), std::move(context)),
        _target(std::move(target)), _outputId(std::move(outputId)) {}

std::set<flux::NeuralInputId> flux::IdentityActivityUnit::GetInputIds() const
{
    return std::set<NeuralInputId> { _target };
}

std::set<flux::NeuralOutputId> flux::IdentityActivityUnit::GetOutputIds() const
{
    return std::set<NeuralOutputId> { _outputId };
}

std::vector<flux::NeuralOutput> flux::IdentityActivityUnit::Activate(const std::vector<NeuralInput> &inputs) const
{
    for (const auto &input : inputs)
    {
        if (input.GetInputId().GetId() == _target.GetId())
        {
            return std::vector<NeuralOutput> { NeuralOutput(_outputId, input.GetValue() )};
        }
    }
    return std::vector<NeuralOutput> { NeuralOutput(_outputId, 0.0 )};
}

std::shared_ptr<flux::IContextUnit> flux::IdentityActivityUnit::Clone(std::shared_ptr<IContext> context) const
{
    return CloneToContext<IdentityActivityUnit>(context);
}
