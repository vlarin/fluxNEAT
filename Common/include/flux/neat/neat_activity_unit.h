//
// Copyright (c) 2019-2020 Temporal Games Inc. All rights reserved.
//

#ifndef FLUXNEAT_NEAT_ACTIVITY_UNIT_H
#define FLUXNEAT_NEAT_ACTIVITY_UNIT_H

#include <flux/prereq.h>
#include <flux/context_unit.h>
#include <flux/activity_unit.h>

namespace flux {
    /**
     * Represents generic usage NEAT activity unit a.k.a. ActionSwitcher. Chooses one of possible outputs with inputs provided
     */
    class FLUX_API NeatActivityUnit : public IActivityUnit
    {
    public:
        NeatActivityUnit(const std::string &id, const std::shared_ptr<IContext> &context,
                         const std::set<NeuralInputId> &inputIds,
                         const std::set<NeuralOutputId> &outputIds);

        std::set<NeuralInputId> GetInputIds() const override;
        std::set<NeuralOutputId> GetOutputIds() const override;

        std::vector<NeuralOutput> Activate(const std::vector<NeuralInput> &inputs) const override;

        std::shared_ptr<IContextUnit> Clone(std::shared_ptr<IContext> context) const override;
        void UpdateScheme(std::istream &istream) override;

        ~NeatActivityUnit() override;

        NeatActivityUnit(const NeatActivityUnit &unit);
        NeatActivityUnit(NeatActivityUnit &&unit) noexcept;
        NeatActivityUnit &operator=(const NeatActivityUnit &other);
    private:
        std::set<NeuralInputId> _inputIds;
        std::set<NeuralOutputId> _outputIds;

        class NeatActivityUnitImpl;
        std::unique_ptr<NeatActivityUnitImpl> _impl;
    };

}

#endif //FLUXNEAT_NEAT_ACTIVITY_UNIT_H
