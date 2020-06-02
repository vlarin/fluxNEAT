//
// Copyright (c) 2019-2020 Temporal Games Inc. All rights reserved.
//

#ifndef FLUXNEAT_HNEAT_ACTIVITY_UNIT_H
#define FLUXNEAT_HNEAT_ACTIVITY_UNIT_H

#include <flux/prereq.h>
#include <flux/context_unit.h>
#include <flux/activity_unit.h>

namespace flux
{
    /**
     * Represents generic usage Hyper NEAT activity unit. Chooses one of possible outputs with cube spatial inputs provided
     */
    class FLUX_API HyperNeatActivityUnit : public IActivityUnit
    {
    public:
        HyperNeatActivityUnit(const std::string &id, const std::shared_ptr<IContext> &context,
                         const std::set<NeuralNodeId> &inputIds,
                         const std::set<NeuralNodeId> &outputIds);

        std::set<NeuralNodeId> GetInputIds() const override;
        std::set<NeuralNodeId> GetOutputIds() const override;

        std::vector<NeuralNode> Activate(const std::vector<NeuralNode> &inputs) const override;

        std::shared_ptr<IContextUnit> Clone(std::shared_ptr<IContext> context) const override;
        void UpdateScheme(std::istream &istream) override;

        ~HyperNeatActivityUnit() override;

        HyperNeatActivityUnit(const HyperNeatActivityUnit &unit);
        HyperNeatActivityUnit(HyperNeatActivityUnit &&unit) noexcept;
        HyperNeatActivityUnit &operator=(const HyperNeatActivityUnit &other);
    private:
        std::set<NeuralNodeId> _inputIds;
        std::set<NeuralNodeId> _outputIds;

        class HyperNeatActivityUnitImpl;
        std::unique_ptr<HyperNeatActivityUnitImpl> _impl;
    };

}

#endif //FLUXNEAT_NEAT_ACTIVITY_UNIT_H
